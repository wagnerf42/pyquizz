#!/usr/bin/env python3
# renvoyer la somme des elements de l'iterable
# celui-ci contient au moins un element
def somme(iterable):
    i = iter(iterable)##    
    resultat = next(i)##    
    for e in i:##    
        resultat += e##    
    return resultat##    

assert somme(range(4)) == 6
assert somme(["bonjour", "le", "monde"]) == "bonjourlemonde"
###80
print(somme(range(10)))
