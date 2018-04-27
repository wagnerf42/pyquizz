#!/usr/bin/env python3
# on dispose de 2 vecteurs de notes
# (chaque position contient une note d'une epreuve)
# afficher la note maximale obtenue a chaque epreuve
def afficher_notes_maximales(notes1, notes2):
    for note1, note2 in zip(notes1, notes2):##    for 
            print(max(note1, note2))
###40
afficher_notes_maximales([1,2,3], [4,5,6])
afficher_notes_maximales([3,2,3], [1,5,2])
