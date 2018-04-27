#!/usr/bin/env python3
# simuler l'appel a la fonction f (dans votre tete)
# mettre dans 'a_aurait_ete' la valeur contenue dans 'a'
# a la fin de l'appel
def g(a):
    b = a - 1
    a = a + 1
    return b

def f():
    a = 2
    b = g(a)
    a = g(b)

a_aurait_ete = 0##a_aurait_ete = 
###90
print(a_aurait_ete)
