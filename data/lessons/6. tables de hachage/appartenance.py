#!/usr/bin/env python3
def contient_2(table_de_hachage):
    #renvoyer True is la table de hachage contient une clef valant 2 et False sinon
    return 2 in table_de_hachage##    return 

###30
d = dict()
d[2] = "foo"
d[1] = "foo"
d[3] = "foo"
print(contient_2(d))
d = dict()
d[1] = "foo"
print(contient_2(d))
