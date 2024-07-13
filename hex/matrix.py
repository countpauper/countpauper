import pygame 
import math

class Matrix2:
    def __init__(self, *coefficients):
        self.coef = coefficients if coefficients else (1,0,0, 0,1,0, 0,0,1)

    def __getitem__(self, idx):
        return self.coef[idx[0] + idx[1]*3]
    
    def row(self, row_idx):
        return pygame.Vector3(self[0, row_idx], self[1, row_idx], self[2,row_idx])
    
    def column(self, col_idx):
        return pygame.Vector3(self[col_idx, 0], self[col_idx, 1], self[col_idx, 2])
    
    def _mul_matrix(self, o):
        return Matrix2( self.row(0).dot(o.column(0)),
                        self.row(0).dot(o.column(1)),
                        self.row(0).dot(o.column(2)),

                        self.row(1).dot(o.column(0)),
                        self.row(1).dot(o.column(1)),
                        self.row(1).dot(o.column(2)),

                        self.row(2).dot(o.column(0)),
                        self.row(2).dot(o.column(1)),
                        self.row(2).dot(o.column(2)))


    def _mul_vector2(self, v):
        w = self[0,2]*v[0]+self[0,2]*v[1]+self[2,2]
        return pygame.Vector2(self[0,0]*v[0] + self[1,0]*v[1] + self[2,0],
                              self[0,1]*v[0] + self[1,1]*v[1] + self[2,1]) / w

    def translation(self):
        return pygame.Vector2(self[2, 0]/self[2,2], self[2, 1]/self[2,2])
    
    def scaling(self):
        return pygame.Vector2(math.sqrt(self[0,0]*self[0,0] + self[0,1]*self[0,1]),
                              math.sqrt(self[0,1]*self[0,1] + self[1,1]*self[1,1])) / self[2,2]
    
    def rotation(self):
        x = pygame.Vector2(self[0, 0], self[0, 1])
        y = pygame.Vector2(self[1, 0], self[1, 1])
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
        return pygame.Vector2(math.atan2(self[1,0],self[0,0]),
                              math.atan2(self[0,1],self[1,1]))

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
        return self.coef == o.coef
        
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
        