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


    def _mul_vector(self, v):
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
        xdot = x[0] # pygame.Vector2(1,0).dot(x) or dot product between identity matrix X axis and this
        ydot = y[1] # pygame.Vector2(0,1).dot(y) or angle between identiy matrix Y axis and this matrix
        avg  = (xdot+ydot)/(x.length()+y.length())  # weighted average normalization
        return math.acos(avg)
    
    def __mul__(self, o):
        if type(o) is Matrix2:
            return self._mul_matrix(o)
        if type(o) is pygame.Vector2:
            return self._mul_vector(o)
    
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
        else:
            # TODO: what would a 3d scale be on 2d. THe third one is a perspective uniform scale in 2,2 ? 
            raise TypeError("Scale must be a scalar or a 2d scale")

    @staticmethod
    def rotate(a):
        return Matrix2(math.cos(a), -math.sin(a),    0,
                      math.sin(a),  math.cos(a),    0,
                      0,            0,              1)
    