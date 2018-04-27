#!/usr/bin/env python3
from math import sqrt
class Point:
    def __init__(self, x, y):
        self.x = x
        self.y = y
    # implementer une methode "distance_vers" calculant la distance
    # entre deux points
    def distance_vers(self, autre):##
        x_diff = self.x-autre.x##
        y_diff = self.y-autre.y##
        return sqrt(x_diff*x_diff + y_diff*y_diff)##
p1 = Point(0, 5)
p2 = Point(2, 3)
print(p1.distance_vers(p2))
###90
p3 = Point(2, 5)
p4 = Point(1, 3)
print(p3.distance_vers(p4))
