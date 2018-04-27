#include<sys/time.h>
#include<stdlib.h>
#include<stdio.h>
#include<assert.h>
#include<string.h>
#include"globals.h"
#include"engine.h"

//declare all global variables here
int width;
int height;
SDL_Color color;
TTF_Font *bigfont;
TTF_Font *smallfont;
SDL_Renderer *renderer;

static void randomize_lessons(engine *quizz) {
    int i;
    int n = quizz->lessons_files.gl_pathc;
    for(i = 0 ; i < n-1 ; i++) {
        int j = rand() % (n - i -1);
        char *tmp = quizz->lessons_files.gl_pathv[i];
        quizz->lessons_files.gl_pathv[i] =
            quizz->lessons_files.gl_pathv[i+1+j];
        quizz->lessons_files.gl_pathv[i+1+j] = tmp;
    }
}

void engine_start_chapter(engine *quizz, char *chapter_name) {
    char *lessons_pattern = malloc(
            strlen("data/lessons/")+strlen(chapter_name)+strlen("/*.py")+1);
    strcpy(lessons_pattern, "data/lessons/");
    strcat(lessons_pattern, chapter_name);
    strcat(lessons_pattern, "/*.py");

    //read lessons names
    glob(lessons_pattern, GLOB_MARK, NULL, &quizz->lessons_files);
    assert(quizz->lessons_files.gl_pathc >= 0);
    randomize_lessons(quizz);
    quizz->successes = 0;

    quizz->current_scene = editor_scene_new(
            quizz->lessons_files.gl_pathv[quizz->current_file]);
#ifdef __EMSCRIPTEN__ // in emscripten, a worker is started at the beginning
    quizz->state = WORKER_RUNNING;
#else
    quizz->state = IN_QUIZZ;
#endif
}

void engine_next_lesson(engine *quizz) {
    quizz->current_file++;
    if (quizz->current_file == quizz->lessons_files.gl_pathc) {
        quizz->state = AT_END;
        float rate = quizz->successes /
            (float) quizz->lessons_files.gl_pathc;
        quizz->current_scene = end_scene_new((int)(rate*100));
    } else {
        quizz->current_scene = editor_scene_new(
                quizz->lessons_files.gl_pathv[quizz->current_file]);
    }
}


engine * engine_new() {

    //init random numbers generator
    struct timeval tv;
    gettimeofday(&tv, NULL);
    srand(tv.tv_usec);

    engine *quizz = (engine*) malloc(sizeof(engine));
    quizz->current_file = 0;
    quizz->state = IN_MENU;
    width = 1024;
    height = 768;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Unable to initialize SDL: ");
        fprintf(stderr, "%s", SDL_GetError());
        exit(1);
    }


    SDL_Window *window = SDL_CreateWindow("le quizz",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            width, height, 0);
    //SDL_WINDOW_FULLSCREEN);

    renderer = SDL_CreateRenderer(window, -1, 0);

    //SDL_EnableKeyRepeat(200, 100);

    TTF_Init();
    int small_font_size = width / 42; // because it will just
    // magically be 80 columns, whatever the screen width
    small_font_size -= (small_font_size % 2);
    smallfont = TTF_OpenFont("data/Inconsolata-Regular.ttf", small_font_size);
    bigfont = TTF_OpenFont("data/Inconsolata-Regular.ttf", 4*small_font_size);
    color = (SDL_Color) {0, 255, 0, 255};

    quizz->current_scene = menu_scene_new("data/lessons/*");

    return quizz;
}

void engine_display(engine *quizz) {
    // blank screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    // display content
    quizz->current_scene->display(quizz->current_scene->data);
    SDL_RenderPresent(renderer);
}

void engine_destroy(engine *quizz) {
    scene_destroy(quizz->current_scene);
    TTF_Quit();
    SDL_Quit();
}

void engine_advance_scene(engine *quizz) {
    scene *old_scene = quizz->current_scene;

    switch(quizz->state) {
        case AT_END:
            quizz->state = IN_MENU;
            quizz->current_file = 0;
            quizz->current_scene = menu_scene_new("data/lessons/*");
            break;
        case IN_QUIZZ:
            engine_next_lesson(quizz);
            break;
        case IN_MENU:
            engine_start_chapter(quizz,
                    menu_get_selection(quizz->current_scene->data));
            break;
    }
    scene_destroy(old_scene);
}

// pass event to current scene, get result
// and act consequently
void engine_scene_event(engine *quizz, SDL_Event event) {
    int result = quizz->current_scene->handle_event(
            quizz->current_scene->data, event);
    if (result == 1) quizz->successes++;
    if (result >= 0)
        engine_advance_scene(quizz);
#ifdef __EMSCRIPTEN__
    if (result == -2)
        quizz->state = WORKER_RUNNING;
#else
    if (result == -3) {
        engine_destroy(quizz);
        exit(0);
    }
#endif
}

void engine_handle_events(engine *quizz) {
    SDL_Event event;
#ifdef __EMSCRIPTEN__
    // in emscripten, postpone SDL events when a worker is running:
    // only handle worker events until it has terminated.
    if (quizz->state == WORKER_RUNNING) {
        int res = handle_worker_events(quizz->current_scene->data);
        if (res >= -1) quizz->state = IN_QUIZZ; // the worker has terminated
        if (res == 1) quizz->successes++; // the code is correct and on time
        if (res >= 0) engine_advance_scene(quizz); // the code is correct
    } // otherwise (-2) the worker is still running: do nothing, just wait

    // we leave the loop as soon as a worker is started and leave the SDL events for later
    // (otherwise we might launch several workers concurrently)
    while (quizz->state != WORKER_RUNNING && SDL_PollEvent(&event)) {
#else
        while (SDL_PollEvent(&event)) {
#endif
            SDL_Keycode sym;
            switch (event.common.type) {
                case SDL_KEYDOWN:
                    sym = event.key.keysym.sym;
                    if (
                            (sym == SDLK_UP)||
                            (sym == SDLK_DOWN)||
                            (sym == SDLK_LEFT)||
                            (sym == SDLK_RIGHT)||
                            (sym == SDLK_BACKSPACE)||
                            (sym == SDLK_DELETE)||
                            (sym == SDLK_j)||
                            (sym == SDLK_k)||
                            (sym == SDLK_ESCAPE)||
                            (sym == SDLK_RETURN)
                       ) {
                        engine_scene_event(quizz, event);
                    }
                    break;
                case SDL_TEXTINPUT:
                    engine_scene_event(quizz, event);
                    break;
                default:
                    break;
            }
        }
    }
