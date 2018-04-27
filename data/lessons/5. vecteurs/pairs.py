#!/usr/bin/env python3
# renvoyer un vecteur contenant tous les entiers pairs
# du vecteur d'entree
def filtre_pairs(vecteur):
    return [e for e in vecteur if not(e%2)]##    return 
###
print(filtre_pairs(list(range(10))))
