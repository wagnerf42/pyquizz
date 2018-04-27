#!/usr/bin/env python3
# renvoyer l'indice du max du vecteur d'entiers positifs
def indice_max(vecteur):
    meilleur = -1
    meilleur_i = 0
    for i, e in enumerate(vecteur):##    
        if e > meilleur:##    
            meilleur_i, meilleur = i, e##    
    return meilleur_i
###
print(indice_max([0, 1, 2, 3]))
print(indice_max([0, 5, 2, 3]))
