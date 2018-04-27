#include<glob.h>
#include<SDL.h>
#include<SDL_ttf.h>
#include"scene.h"

//different states we can be in
#ifdef __EMSCRIPTEN__
enum states {IN_MENU, IN_QUIZZ, WORKER_RUNNING, AT_END};
#else
enum states {IN_MENU, IN_QUIZZ, AT_END};
#endif

typedef struct _engine {
    // scene contains all widget currently displayed
    scene *current_scene;
    // files asked to the users
    glob_t lessons_files;
    unsigned int current_file;
    // some stats to give user feedback
    unsigned int successes; // #lessons answered on time
    // what is current state ?
    enum states state;
} engine;

extern engine * engine_new();
extern void engine_display(engine *quizz);
extern void engine_destroy(engine *quizz);
extern void engine_handle_events(engine *quizz);
