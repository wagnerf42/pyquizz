#include<sys/wait.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<assert.h>
#include<SDL.h>
#include"interpreter.h"

#define DATA_SIZE 10000

#ifdef __EMSCRIPTEN__
#include<emscripten.h>

void launch_emscripten(char *code,
                       char **standard_output, char **standard_error, signed char *result) {
    *result = -1;
    EM_ASM_({ //ici une fonction en javascript
        'use strict';
	if (Module.pythonWorker === undefined) { //only create the worker once
	    Module.pythonWorker = new Worker('js/interp_python.js');
	}
        function interrompre() {
            Module.pythonWorker.terminate();
	    Module.pythonWorker = undefined; // so that we recreate it next time
	    console.log('interruption du worker !');
            Module.setValue($3, 1, 'i8'); // $3 = result
        };
        var timer = setTimeout(interrompre, 2000); // timeout 2 secondes
        Module.pythonWorker.onmessage = function(e) {
	    clearTimeout(timer);
            // le message doit contenir un tableau à deux cases : stdout et stderr
            var reponse = e.data[0];
            var erreur = e.data[1];
	    var length_r = lengthBytesUTF8(reponse) + 1;
	    var length_e = lengthBytesUTF8(erreur) + 1;
	    var output = Module._malloc(length_r);
	    Module.setValue($1, output, 'i8*'); // $1 = standard_output
	    var error = Module._malloc(length_e);
	    Module.setValue($2, error, 'i8*'); // $2 = standard_error
	    stringToUTF8(reponse, output, length_r);
	    stringToUTF8(erreur, error, length_e);
	    Module.setValue($3, 0, 'i8'); // $3 = result
	    Module.pythonWorker.onmessage = null; // clean up
	};
	var le_code = Pointer_stringify($0); // $0 = code
	Module.pythonWorker.postMessage(le_code); // pythonWorker va exécuter le code
    }, code, standard_output, standard_error, result); // fin de la fonction javascript  
}

#else

int launch_native(char *code, char **standard_output, char **standard_error) {
    int in_pipe[2];
    int out_pipe[2];
    int err_pipe[2];
    pipe(in_pipe);
    pipe(out_pipe);
    pipe(err_pipe);
    pid_t pid = fork();
    if (pid) {
        close(in_pipe[0]);
        close(out_pipe[1]);
        close(err_pipe[1]);
        // father, send data, wait and kill
        size_t size = strlen(code);
        write(in_pipe[1], code, size+1);
        close(in_pipe[1]); // close pipe for interpreter to complete
        int status;
        SDL_Delay(200);
        int result = waitpid(pid, &status, WNOHANG);
        if (result == -1) {
            perror("error waiting for child");
            exit(1);
        } else if (result == 0) {
            printf("killing process\n");
            kill(pid, 9);
            return 1;
        }
        // get data
        if (status != 0) {
            // get stderr
            if (standard_output != NULL) {
                *standard_output = NULL;
            }
            if (standard_error == NULL) {
                fprintf(stderr, "code incorrect: %s\n", code);
                exit(1);
            }
            *standard_error = (char*) malloc(DATA_SIZE);
            ssize_t size = read(err_pipe[0], *standard_error, DATA_SIZE);
            assert(size > 0);
            (*standard_error)[size-1] = 0;
        } else {
            // get stdout
            if (standard_error != NULL) {
                *standard_error = NULL;
            }
            assert(standard_output != NULL);
            *standard_output = (char*) malloc(DATA_SIZE);
            ssize_t size = read(out_pipe[0], *standard_output, DATA_SIZE);
            assert(size > 0);
            (*standard_output)[size-1] = 0;
        }
        return 0;
    } else {
        // prepare pipes
        close(in_pipe[1]);
        dup2(in_pipe[0], 0);
        close(out_pipe[0]);
        dup2(out_pipe[1], 1);
        dup2(err_pipe[1], 2);
        // execute
        execlp("python3", "python3", NULL);
    }

    return 1;
}

int launch(char *code, char **standard_output, char **standard_error) {
    return launch_native(code, standard_output, standard_error);
}

#endif
