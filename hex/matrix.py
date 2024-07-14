import pygame 
import math
import numpy as np

class Matrix2:
    def __init__(self, *coefficients):
        if coefficients:
            if len(coefficients)==1:
                self.coef = np.array(coefficients[0])
            else:
                self.coef = np.asarray(coefficients).reshape(3,3) 
        else:
            self.coef = np.asarray(((1,0,0), (0,1,0), (0,0,1)))

    def __getitem__(self, idx):
        return self.coef[idx]
    
    def _mul_matrix(self, o):
        return Matrix2(np.matmul(self.coef, o.coef))

    def _mul_vector2(self, v):
        w = self[2,0]*v[0]+self[2,0]*v[1]+self[2,2]
        return pygame.Vector2(self[0,0]*v[0] + self[0,1]*v[1] + self[0,2],
                              self[1,0]*v[0] + self[1,1]*v[1] + self[1,2]) / w

    def translation(self):
        return pygame.Vector2(self[0, 2]/self[2,2], self[1, 2]/self[2,2])
    
    def scaling(self):
        return pygame.Vector2(math.sqrt(self[0,0]*self[0,0] + self[0,1]*self[0,1]),
                              math.sqrt(self[0,1]*self[0,1] + self[1,1]*self[1,1])) / self[2,2]
    
    def rotation(self):
        x = pygame.Vector2(self[0, 0], self[1, 0])
        y = pygame.Vector2(self[0, 1], self[1, 1])
        xcross = pygame.Vector2(1,0).cross(x) 
        ycross = pygame.Vector2(0,1).cross(y) 
        # compute separate angles between rotates x and y then average
        #xasin = xcross / x.length()
        #yasin = ycross / y.length()
        #xtheta = math.asin(xasin)
        #ytheta = math.asin(yasin)
        #return (xtheta+ytheta)*0.5
        # vs weighted average by length of the axis, before the asin
        asin = (xcross+ycross) / (x.length() + y.length())
        return math.asin(asin)
    
    def skewed(self):
        return pygame.Vector2(math.atan2(self[0,1],self[0,0]),
                              math.atan2(self[1,0],self[1,1]))

    def __mul__(self, o):
        if type(o) is Matrix2:
            return self._mul_matrix(o)
        if type(o) is pygame.Vector2:
            return self._mul_vector2(o)
        if type(o) is pygame.Vector3:
            raise NotImplemented("Matrix multiplication of 2D homogenous vectors is not yet implemented.")
        else:
            return self.__mul__(pygame.Vector2(o))
         
    def __eq__(self, o):
        return np.array_equal(self.coef, o.coef)
        
    @staticmethod
    def identity():
        return Matrix2(1,0,0, 0,1,0, 0,0,1)
    
    @staticmethod
    def translate(*v):
        if len(v)==2:
            return Matrix2(1,0,v[0], 0,1,v[1], 0,0,1)
        else:
            raise TypeError("Translation must be a 2d vector") 
                   
    @staticmethod
    def scale(*s):
        if len(s)==1:
            return Matrix2(1,0,0, 0,1,0, 0,0,1/s[0])
        elif len(s)==2:
            return Matrix2(s[0],0,0, 0,s[1],0, 0,0,1)
        elif len(s)==3:
            return Matrix2(s[0],0,0, s[1],0,0, 0,0,1.0/s[2])
        else:
            raise ValueError("Scale must be a 1D 2D or 3D vector")

    @staticmethod
    def rotate(a):
        return Matrix2(math.cos(a), -math.sin(a),    0,
                      math.sin(a),  math.cos(a),    0,
                      0,            0,              1)
    
    @staticmethod
    def clockwise():
        return Matrix2.rotate(math.pi * -0.5)

    @staticmethod
    def counter_clockwise():
        return Matrix2.rotate(math.pi * 0.5)

    @staticmethod
    def flip(horizontal=False, vertical=False):
        return Matrix2.scale(-1.0 if horizontal else 1.0, -1.0 if vertical else 1.0)

    @staticmethod
    def skew(ax, ay=0):
        return Matrix2(1,math.tan(ax),0, math.tan(ay),1,0, 0,0,1 )
        