#!/usr/bin/env python3
# a l'aide d'un set
# renvoyer un vecteur contenant un seul de chacun des elements du vecteur d'entree
# par exemple [1, 2, 2, 1, 3] pourrait renvoyer [1, 2, 3]
def uniques(vecteur):
    return list(set(vecteur))##    return 
###20
print(uniques([1, 3, 5, 3, 1, 2]))
