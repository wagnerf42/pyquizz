#!/usr/bin/env python3
# completer le code pour afficher
# touts les elements pairs
def positions_pairs(vecteur):
    for position in filter(lambda e: e%2 == 0, vecteur):##    for position in filter(
        print(position)
###
print(positions_pairs([2,4,7]))
print(positions_pairs(list(range(3))))
