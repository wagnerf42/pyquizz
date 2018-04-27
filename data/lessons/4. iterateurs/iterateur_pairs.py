#!/usr/bin/env python3
# a l'aide d'un generateur
# faire un iterateur iterant sur tous les elements pairs de l'iterable
def iterateur_pairs(iterable):
    for element in iterable:##    
        if not(element%2):##    
            yield element##    
###
for element in iterateur_pairs([1, 2, 5, 3, 4, 6, 3, 1]):
    print(element)
