#!/usr/bin/env python3
# si division par zero afficher "impossible"
# sinon renvoyer la reponse
def division(numerateur, denominateur):
    if denominateur:##    
        return numerateur / denominateur##    
    print("impossible")##    
###60
print(division(3,2))
print(division(1,0))
print(division(0,5))
