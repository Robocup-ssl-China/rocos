#include "matrix2d.h"
#include <stdexcept>
#include <cassert>
#include <algorithm>
#define EPS 1e-10
using std::ostream;
using std::istream;
using std::endl;
using std::domain_error;

/* PUBLIC MEMBER FUNCTIONS
 ********************************/

Matrix2d::Matrix2d(int rows, int cols) : _row(rows), _column(cols) {
    allocSpace();
    for (int i = 0; i < _row; ++i) {
        for (int j = 0; j < _column; ++j) {
            _value[i][j] = 0;
        }
    }
}
Matrix2d::Matrix2d(int row, int column, std::initializer_list<double> vals): _row(row), _column(column) {
    allocSpace();
    assert(row * column == vals.size());
    int i = 0;
    for (auto v : vals) {
        _value[i / column][i % column] = v;
        i++;
    }
}

Matrix2d::Matrix2d() : _row(1), _column(1) {
    allocSpace();
    _value[0][0] = 0;
}

Matrix2d::~Matrix2d() {
    for (int i = 0; i < _row; ++i) {
        delete[] _value[i];
    }
    delete[] _value;
}

Matrix2d::Matrix2d(const Matrix2d& m) : _row(m._row), _column(m._column) {
    allocSpace();
    for (int i = 0; i < _row; ++i) {
        for (int j = 0; j < _column; ++j) {
            _value[i][j] = m._value[i][j];
        }
    }
}

Matrix2d& Matrix2d::operator=(const Matrix2d& m) {
    if (this == &m) {
        return *this;
    }

    if (_row != m._row || _column != m._column) {
        for (int i = 0; i < _row; ++i) {
            delete[] _value[i];
        }
        delete[] _value;

        _row = m._row;
        _column = m._column;
        allocSpace();
    }

    for (int i = 0; i < _row; ++i) {
        for (int j = 0; j < _column; ++j) {
            _value[i][j] = m._value[i][j];
        }
    }
    return *this;
}

Matrix2d& Matrix2d::operator+=(const Matrix2d& m) {
    for (int i = 0; i < _row; ++i) {
        for (int j = 0; j < _column; ++j) {
            _value[i][j] += m._value[i][j];
        }
    }
    return *this;
}

Matrix2d& Matrix2d::operator-=(const Matrix2d& m) {
    for (int i = 0; i < _row; ++i) {
        for (int j = 0; j < _column; ++j) {
            _value[i][j] -= m._value[i][j];
        }
    }
    return *this;
}

Matrix2d& Matrix2d::operator*=(const Matrix2d& m) {
    Matrix2d temp(_row, m._column);
    for (int i = 0; i < temp._row; ++i) {
        for (int j = 0; j < temp._column; ++j) {
            for (int k = 0; k < _column; ++k) {
                temp._value[i][j] += (_value[i][k] * m._value[k][j]);
            }
        }
    }
    return (*this = temp);
}

Matrix2d& Matrix2d::operator*=(double num) {
    for (int i = 0; i < _row; ++i) {
        for (int j = 0; j < _column; ++j) {
            _value[i][j] *= num;
        }
    }
    return *this;
}

Matrix2d& Matrix2d::operator/=(double num) {
    for (int i = 0; i < _row; ++i) {
        for (int j = 0; j < _column; ++j) {
            _value[i][j] /= num;
        }
    }
    return *this;
}

Matrix2d Matrix2d::operator^(int num) {
    Matrix2d temp(*this);
    return expHelper(temp, num);
}

void Matrix2d::swapRows(int r1, int r2) {
    double *temp = _value[r1];
    _value[r1] = _value[r2];
    _value[r2] = temp;
}

Matrix2d Matrix2d::transpose() {
    Matrix2d ret(_column, _row);
    for (int i = 0; i < _row; ++i) {
        for (int j = 0; j < _column; ++j) {
            ret._value[j][i] = _value[i][j];
        }
    }
    return ret;
}


/* STATIC CLASS FUNCTIONS
 ********************************/

Matrix2d Matrix2d::createIdentity(int size) {
    Matrix2d temp(size, size);
    for (int i = 0; i < temp._row; ++i) {
        for (int j = 0; j < temp._column; ++j) {
            if (i == j) {
                temp._value[i][j] = 1;
            } else {
                temp._value[i][j] = 0;
            }
        }
    }
    return temp;
}
Matrix2d Matrix2d::createIdentity(int row, int column) {
    Matrix2d temp(row, column);
    for (int i = 0; i < std::min(row, column); i++) {
        temp._value[i][i] = 1;
    }
    return temp;
}
Matrix2d Matrix2d::solve(Matrix2d A, Matrix2d b) {
    // Gaussian elimination
    for (int i = 0; i < A._row; ++i) {
        if (A._value[i][i] == 0) {
            // pivot 0 - throw error
            throw domain_error("Error: the coefficient Matrix2d has 0 as a pivot. Please fix the input and try again.");
        }
        for (int j = i + 1; j < A._row; ++j) {
            for (int k = i + 1; k < A._column; ++k) {
                A._value[j][k] -= A._value[i][k] * (A._value[j][i] / A._value[i][i]);
                if (A._value[j][k] < EPS && A._value[j][k] > -1 * EPS)
                    A._value[j][k] = 0;
            }
            b._value[j][0] -= b._value[i][0] * (A._value[j][i] / A._value[i][i]);
            if (A._value[j][0] < EPS && A._value[j][0] > -1 * EPS)
                A._value[j][0] = 0;
            A._value[j][i] = 0;
        }
    }

    // Back substitution
    Matrix2d x(b._row, 1);
    x._value[x._row - 1][0] = b._value[x._row - 1][0] / A._value[x._row - 1][x._row - 1];
    if (x._value[x._row - 1][0] < EPS && x._value[x._row - 1][0] > -1 * EPS)
        x._value[x._row - 1][0] = 0;
    for (int i = x._row - 2; i >= 0; --i) {
        double sum = 0;
        for (int j = i + 1; j < x._row; ++j) {
            sum += A._value[i][j] * x._value[j][0];
        }
        x._value[i][0] = (b._value[i][0] - sum) / A._value[i][i];
        if (x._value[i][0] < EPS && x._value[i][0] > -1 * EPS)
            x._value[i][0] = 0;
    }

    return x;
}

Matrix2d Matrix2d::bandSolve(Matrix2d A, Matrix2d b, int k) {
    // optimized Gaussian elimination
    int bandsBelow = (k - 1) / 2;
    for (int i = 0; i < A._row; ++i) {
        if (A._value[i][i] == 0) {
            // pivot 0 - throw exception
            throw domain_error("Error: the coefficient Matrix2d has 0 as a pivot. Please fix the input and try again.");
        }
        for (int j = i + 1; j < A._row && j <= i + bandsBelow; ++j) {
            int k = i + 1;
            while (k < A._column && A._value[j][k]) {
                A._value[j][k] -= A._value[i][k] * (A._value[j][i] / A._value[i][i]);
                k++;
            }
            b._value[j][0] -= b._value[i][0] * (A._value[j][i] / A._value[i][i]);
            A._value[j][i] = 0;
        }
    }

    // Back substitution
    Matrix2d x(b._row, 1);
    x._value[x._row - 1][0] = b._value[x._row - 1][0] / A._value[x._row - 1][x._row - 1];
    for (int i = x._row - 2; i >= 0; --i) {
        double sum = 0;
        for (int j = i + 1; j < x._row; ++j) {
            sum += A._value[i][j] * x._value[j][0];
        }
        x._value[i][0] = (b._value[i][0] - sum) / A._value[i][i];
    }

    return x;
}

// functions on VECTORS
double Matrix2d::dotProduct(Matrix2d a, Matrix2d b) {
    double sum = 0;
    for (int i = 0; i < a._row; ++i) {
        sum += (a(i, 0) * b(i, 0));
    }
    return sum;
}

// functions on AUGMENTED matrices
Matrix2d Matrix2d::augment(Matrix2d A, Matrix2d B) {
    Matrix2d AB(A._row, A._column + B._column);
    for (int i = 0; i < AB._row; ++i) {
        for (int j = 0; j < AB._column; ++j) {
            if (j < A._column)
                AB(i, j) = A(i, j);
            else
                AB(i, j) = B(i, j - B._column);
        }
    }
    return AB;
}

Matrix2d Matrix2d::gaussianEliminate() {
    Matrix2d Ab(*this);
    int rows = Ab._row;
    int cols = Ab._column;
    int Acols = cols - 1;

    int i = 0; // row tracker
    int j = 0; // column tracker

    // iterate through the rows
    while (i < rows) {
        // find a pivot for the row
        bool pivot_found = false;
        while (j < Acols && !pivot_found) {
            if (Ab(i, j) != 0) { // pivot not equal to 0
                pivot_found = true;
            } else { // check for a possible swap
                int max_row = i;
                double max_val = 0;
                for (int k = i + 1; k < rows; ++k) {
                    double cur_abs = Ab(k, j) >= 0 ? Ab(k, j) : -1 * Ab(k, j);
                    if (cur_abs > max_val) {
                        max_row = k;
                        max_val = cur_abs;
                    }
                }
                if (max_row != i) {
                    Ab.swapRows(max_row, i);
                    pivot_found = true;
                } else {
                    j++;
                }
            }
        }

        // perform elimination as normal if pivot was found
        if (pivot_found) {
            for (int t = i + 1; t < rows; ++t) {
                for (int s = j + 1; s < cols; ++s) {
                    Ab(t, s) = Ab(t, s) - Ab(i, s) * (Ab(t, j) / Ab(i, j));
                    if (Ab(t, s) < EPS && Ab(t, s) > -1 * EPS)
                        Ab(t, s) = 0;
                }
                Ab(t, j) = 0;
            }
        }

        i++;
        j++;
    }

    return Ab;
}

Matrix2d Matrix2d::rowReduceFromGaussian() {
    Matrix2d R(*this);
    int rows = R._row;
    int cols = R._column;

    int i = rows - 1; // row tracker
    int j = cols - 2; // column tracker

    // iterate through every row
    while (i >= 0) {
        // find the pivot column
        int k = j - 1;
        while (k >= 0) {
            if (R(i, k) != 0)
                j = k;
            k--;
        }

        // zero out elements above pivots if pivot not 0
        if (R(i, j) != 0) {

            for (int t = i - 1; t >= 0; --t) {
                for (int s = 0; s < cols; ++s) {
                    if (s != j) {
                        R(t, s) = R(t, s) - R(i, s) * (R(t, j) / R(i, j));
                        if (R(t, s) < EPS && R(t, s) > -1 * EPS)
                            R(t, s) = 0;
                    }
                }
                R(t, j) = 0;
            }

            // divide row by pivot
            for (int k = j + 1; k < cols; ++k) {
                R(i, k) = R(i, k) / R(i, j);
                if (R(i, k) < EPS && R(i, k) > -1 * EPS)
                    R(i, k) = 0;
            }
            R(i, j) = 1;

        }

        i--;
        j--;
    }

    return R;
}

void Matrix2d::readSolutionsFromRREF(ostream& os) {
    Matrix2d R(*this);

    // print number of solutions
    bool hasSolutions = true;
    bool doneSearching = false;
    int i = 0;
    while (!doneSearching && i < _row) {
        bool allZeros = true;
        for (int j = 0; j < _column - 1; ++j) {
            if (R(i, j) != 0)
                allZeros = false;
        }
        if (allZeros && R(i, _column - 1) != 0) {
            hasSolutions = false;
            os << "NO SOLUTIONS" << endl << endl;
            doneSearching = true;
        } else if (allZeros && R(i, _column - 1) == 0) {
            os << "INFINITE SOLUTIONS" << endl << endl;
            doneSearching = true;
        } else if (_row < _column - 1) {
            os << "INFINITE SOLUTIONS" << endl << endl;
            doneSearching = true;
        }
        i++;
    }
    if (!doneSearching)
        os << "UNIQUE SOLUTION" << endl << endl;

    // get solutions if they exist
    if (hasSolutions) {
        Matrix2d particular(_column - 1, 1);
        Matrix2d special(_column - 1, 1);

        for (int i = 0; i < _row; ++i) {
            bool pivotFound = false;
            bool specialCreated = false;
            for (int j = 0; j < _column - 1; ++j) {
                if (R(i, j) != 0) {
                    // if pivot variable, add b to particular
                    if (!pivotFound) {
                        pivotFound = true;
                        particular(j, 0) = R(i, _column - 1);
                    } else { // otherwise, add to special solution
                        if (!specialCreated) {
                            special = Matrix2d(_column - 1, 1);
                            specialCreated = true;
                        }
                        special(j, 0) = -1 * R(i, j);
                    }
                }
            }
            os << "Special solution:" << endl << special << endl;
        }
        os << "Particular solution:" << endl << particular << endl;
    }
}

Matrix2d Matrix2d::inverse() {
    Matrix2d I = Matrix2d::createIdentity(_row);
    Matrix2d AI = Matrix2d::augment(*this, I);
    Matrix2d U = AI.gaussianEliminate();
    Matrix2d IAInverse = U.rowReduceFromGaussian();
    Matrix2d AInverse(_row, _column);
    for (int i = 0; i < AInverse._row; ++i) {
        for (int j = 0; j < AInverse._column; ++j) {
            AInverse(i, j) = IAInverse(i, j + _column);
        }
    }
    return AInverse;
}


/* PRIVATE HELPER FUNCTIONS
 ********************************/

void Matrix2d::allocSpace() {
    _value = new double*[_row];
    for (int i = 0; i < _row; ++i) {
        _value[i] = new double[_column];
    }
}

Matrix2d Matrix2d::expHelper(const Matrix2d& m, int num) {
    if (num == 0) {
        return createIdentity(m._row);
    } else if (num == 1) {
        return m;
    } else if (num % 2 == 0) {  // num is even
        return expHelper(m * m, num / 2);
    } else {                    // num is odd
        return m * expHelper(m * m, (num - 1) / 2);
    }
}

/* NON-MEMBER FUNCTIONS
 ********************************/

Matrix2d operator+(const Matrix2d& m1, const Matrix2d& m2) {
    Matrix2d temp(m1);
    return (temp += m2);
}

Matrix2d operator-(const Matrix2d& m1, const Matrix2d& m2) {
    Matrix2d temp(m1);
    return (temp -= m2);
}

Matrix2d operator*(const Matrix2d& m1, const Matrix2d& m2) {
    Matrix2d temp(m1);
    return (temp *= m2);
}

Matrix2d operator*(const Matrix2d& m, double num) {
    Matrix2d temp(m);
    return (temp *= num);
}

Matrix2d operator*(double num, const Matrix2d& m) {
    return (m * num);
}

Matrix2d operator/(const Matrix2d& m, double num) {
    Matrix2d temp(m);
    return (temp /= num);
}

ostream& operator<<(ostream& os, const Matrix2d& m) {
    for (int i = 0; i < m._row; ++i) {
        os << m._value[i][0];
        for (int j = 1; j < m._column; ++j) {
            os << " " << m._value[i][j];
        }
        os << endl;
    }
    return os;
}

istream& operator>>(istream& is, Matrix2d& m) {
    for (int i = 0; i < m._row; ++i) {
        for (int j = 0; j < m._column; ++j) {
            is >> m._value[i][j];
        }
    }
    return is;
}
