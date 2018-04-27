#!/usr/bin/env python3
# renvoyez l'indice du premier element pair
def indice_premier_pair(vecteur):
    return min((i, e) for i, e in enumerate(vecteur) if not(e%2))[0]##    return 
###90
print(indice_premier_pair([1, 3, 5, 7, 8]))
print(indice_premier_pair([1, 2, 5, 7, 8]))
