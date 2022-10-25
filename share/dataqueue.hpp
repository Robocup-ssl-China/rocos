#ifndef DATAQUEUE_H
#define DATAQUEUE_H

// use to simulate one round-robin queue for the data that calculate with cycles
// you can't change data with the access of brackets. in order to add data, use the function 'push()'

template <typename T>
class DataQueue{
public:
    DataQueue();
    DataQueue(int size);
    const T& operator[](int index);
    const T& getStart();
    int getstartIndex(){return _startIndex;}
    int getcurrentIndex(){return _currentIndex;}
    int size(){ return _size; }
    int validSize(){ return _validSize; }
    int cycle(){ return _cycle; }
    void push(const T& t);
    void clear();
    void renew();
private:
    long long _cycle;
    long _size;
    long _validSize;
    long  _currentIndex;
    long _startIndex;
    T* pointer;
};

template<typename T>
inline DataQueue<T>::DataQueue():_cycle(0),_size(100),_validSize(0),_currentIndex(99),_startIndex(1),pointer(nullptr){
    pointer = new T[_size];
}

template<typename T>
inline DataQueue<T>::DataQueue(int size):_size(size),_currentIndex(size-1),_startIndex(1),_validSize(0),pointer(nullptr),_cycle(0){
    pointer = new T[_size];
}

template<typename T>
const T& DataQueue<T>::operator[](int index){
    return pointer[(( index % _size ) + _size + _currentIndex ) % _size];
}

template<typename T>
void DataQueue<T>::push(const T& t){
    _currentIndex =(_currentIndex+1)%_size;
    if(_startIndex>-_size) _startIndex--;
    if(_validSize < _size) _validSize++;
    _cycle++;
    pointer[_currentIndex] = t;
}
template<typename T>
void DataQueue<T>::clear(){
    _validSize=0;
    _currentIndex=_size-1;
    _startIndex=1;
}
template<typename T>
void DataQueue<T>::renew(){
    _currentIndex=10;
    _startIndex=-10;
}
template<typename T>
const T& DataQueue<T>::getStart(){
    return pointer[(( _startIndex % _size ) + _size + _currentIndex ) % _size];
}
#endif
