#!/usr/bin/env python3
# renvoyer le premier element de l'iterable donne (suppose non vide)
def premier(iterable):
    return next(iter(iterable))##    
###30
print(premier(range(10)))
print(premier(range(4, 10)))
print(premier([4, 5]))
