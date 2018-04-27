#!/usr/bin/env python3
# renvoyer un vecteur contenant le double de chaque element entier
# du vecteur d'entree
# par exemple [1, 2, 3] devient [2, 4, 6]
def doubles(vecteur):
    return [e*2 for e in vecteur]##    return [

assert doubles(list(range(4))) == list(range(0, 8, 2))
###40
print(doubles(list(range(10))))
