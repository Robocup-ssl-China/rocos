import numpy as np
class Vec:
    def __init__(self, x, y):
        self.x = x
        self.y = y
    def __add__(self, other):
        return Vec(self.x + other.x, self.y + other.y)
    def __sub__(self, other):
        return Vec(self.x - other.x, self.y - other.y)
    def __mul__(self, other):
        return Vec(self.x * other, self.y * other)
    def __truediv__(self, other):
        return Vec(self.x / other, self.y / other)
    def __str__(self):
        return f"({self.x}, {self.y})"
    def __repr__(self):
        return f"({self.x}, {self.y})"
    def __iter__(self):
        return iter([self.x, self.y])
    def __getitem__(self, index):
        return [self.x, self.y][index]
    @property
    def mod(self):
        return np.sqrt(self.x**2 + self.y**2)
    @mod.setter
    def mod(self, value):
        self.x = self.x / self.mod * value
        self.y = self.y / self.mod * value
    @property
    def dir(self):
        return np.arctan2(self.y, self.x)
    @dir.setter
    def dir(self, value):
        _mod = self.mod
        self.x = np.cos(value) * _mod
        self.y = np.sin(value) * _mod
    def _rotate(self, radians):
        x, y = self.x, self.y
        self.x = x * np.cos(radians) - y * np.sin(radians)
        self.y = x * np.sin(radians) + y * np.cos(radians)
    def rotate(self, radians):
        newVec = Vec(self.x, self.y)
        newVec._rotate(radians)
        return newVec
    
if __name__ == "__main__":
    v = Vec(1,1)
    print(v)
    v._rotate(np.pi/2)
    print(v)
    v._rotate(-v.dir)
    print(v)