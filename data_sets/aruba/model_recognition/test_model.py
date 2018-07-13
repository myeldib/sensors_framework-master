#!/usr/bin/python

import numpy as np

def multiply(data1,data2):
    print data1
    print "=============="
    print data2
    x = np.array([2, 3, 1, 0]) 
    print x
    return x.astype(np.ubyte, copy=False)


def multiply2():
    x = np.array([2, 3, 1, 0])
    return x.astype(np.ubyte, copy=False)


print multiply2