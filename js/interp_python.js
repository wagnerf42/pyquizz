"use strict";
// on définit les objets dont brython a besoin (document et window)
var brython = {
    src : "brython.js",
    type : "text/javascript"
}
var document = {
    scripts : [brython],
    getElementsByTagName() { // la seule qui nous intéresse
	return this.scripts;
    },
    // par sécurité on implément aussi ces méthodes qui apparaissent dans brython.js
    getElementsByName() {return [];},
    getElementsByClassName() {return [];},
    getElementById() {return [];},
    querySelectorAll() {return [];},
    createTextNode() {console.log("create text node: impossible in a worker!");},
    createElement() {console.log("create element: impossible in a worker!");},
    createElementNS() {console.log("create element NS: impossible in a worker!");},
    createEvent() {console.log("create event: impossible in a worker!");},
    location : location,
    body : {scrollLeft : 0, scrollTop : 0},
    documentElement: {scrollLeft : 0, scrollTop : 0}
}
var window = this; // le worker (this) possède déjà pas mal d’attributs/méthodes de window
// normalement on n’appellera pas ces fonctions mais brython les cherche pour s’initialiser :
window.alert = function(message) {console.log(message);}
window.confirm = function(message) {console.log("confirmation :" + message); return true;}
window.prompt = function(message, def) {
    console.log("prompt :" + message);
    if (def !== undefined) return def;
    else return "";
}
// les variables pour stocker stdout et stderr
var sortie_python = "";
var erreur_python = "";
// maintenant on peut charger brython
importScripts("brython.js");
importScripts("brython_stdlib.js");
var en_tete = { // code python à exécuter avant le code fourni, pour rediriger stdout/stderr
    type : "text/python3",
    id : "header", // ceci donne le nom du module et permet que l’autre s’appelle __main__
    textContent :
"import sys\n\
from browser import window\n\
class Pyth_stdout:\n\
    def write(string):\n\
        window.sortie_python += string\n\
class Pyth_stderr:\n\
    def write(string):\n\
        window.erreur_python += string\n\
sys.stdout = Pyth_stdout\n\
sys.stderr = Pyth_stderr"
}
document.scripts.push(en_tete);
var code_a_executer = { // contiendra le code principal
    type : "text/python3" // textContent sera rempli au dernier moment
}
document.scripts.push(code_a_executer);
onmessage = function (e) {
    code_a_executer.textContent = e.data;
    try {
	brython(1); // 1 = afficher les exceptions non rattrapées
    } catch (e) {
	erreur_python += "\nL'interpreteur s'est interrompu : " + e;
    }
    postMessage([sortie_python, erreur_python]); // réponse au thread principal
    code_a_executer.textContent = ""; // on nettoie
    sortie_python = "";
    erreur_python = "";
}
