#!/usr/bin/env python3
# en numerotants les elements de l'iterable (sur des entiers) a partir de 0
# afficher tous les numeros pour lesquels l'element est pair
def affiche_positions_pairs(iterable):
    for i, e in enumerate(iterable):##    
        if not(e % 2):##    
            print(i)##    
###
affiche_positions_pairs(range(10))
affiche_positions_pairs([1,1,2,2,2,5])
