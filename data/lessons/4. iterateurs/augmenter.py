#!/usr/bin/env python3
# a l'aide d'un generateur iterer sur tous les entiers de l'iterable,
# chacun augmente de 1
def augmenter(iterable):
    for e in iterable:##    
        yield e+1##    

v = list(range(4))
# creer le vecteur v_plus contenant les elements de v augmentes de 1
v_plus = list(augmenter(v))##v_plus =
###
print(v_plus)
print(list(augmenter(range(5))))
print(type(v_plus))
