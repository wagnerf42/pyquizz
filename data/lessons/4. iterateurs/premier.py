#!/usr/bin/env python3
# si le premier element de l'iterable est different de 0
# renvoyer la somme de tout le reste
# sinon renvoyer None
def presque_somme(iterable_entiers):
    i = iter(iterable_entiers)##    
    if next(i):##    
        return sum(i)##    
###
print(presque_somme(range(10)))
print(presque_somme(range(3, 10)))
