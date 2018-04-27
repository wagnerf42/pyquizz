#!/usr/bin/env python3
# renvoyer le premier element de l'iterable divisible par le facteur donne
def premier_multiple(iterable, facteur):
    for e in iterable:##    
        if not(e % facteur):##    
            return e##    
###
print(premier_multiple(list(range(1, 10)), 4))
print(premier_multiple(list(range(1, 10)), 7))
print(premier_multiple(range(1, 10), 12))
