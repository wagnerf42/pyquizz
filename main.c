#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include"engine.h"

engine *quizz;

void main_iteration() {
    engine_handle_events(quizz);
    engine_display(quizz);
}


int main(int argc, char **argv) {

    quizz = engine_new();

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_iteration, 0, 1);
#else
    while (1) {
        main_iteration();
        SDL_Delay(50);
    }
#endif
}

