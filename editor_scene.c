#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include"timer.h"
#include"scene.h"
#include"editor.h"
#include"paragraph.h"
#include"interpreter.h"
#include"globals.h"

typedef struct _editor_scene {
    timer_widget *timer;
    editor_widget *editor;
    paragraph_widget *messages;
    char *correct_stdout; // expected stdout when running correct code
#ifdef __EMSCRIPTEN__
    char *actual_stdout; // actual stdout when running student code
    char *actual_stderr; // stderr when running student code
    signed char status; // 0 : the code finished normally, 1 : the code timed out,
    // -1 : the code is still running
#endif
} editor_scene;

char *slurp(char *filename) {
    //read whole file
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        perror("unable to open file");
        exit(1);
    }
    fseek(f, 0, SEEK_END);
    long size = ftell(f) - 1;
    char *content = (char*) malloc(size+1);
    fseek(f, 0, SEEK_SET);
    fread(content, 1, size, f);
    content[size] = 0; // end the string
    fclose(f);
    return content;
}

#ifndef __EMSCRIPTEN__
//run code, return 1 if success ; 0 else
int run_code(editor_scene *editor) {
    char *code = editor_get_content(editor->editor);
    char *output = NULL;
    char *error_output = NULL;
    int status = launch(code, &output, &error_output);
    int success = 0;
    free((void*) code);
    paragraph_destroy(editor->messages);
    char *message;
    if (status != 0) {
        message = "le programme prend trop de temps. Boucle infinie ?";
    } else if (error_output != NULL) {
        message = error_output;
    } else {
        if (strcmp(output, editor->correct_stdout) == 0) {
            message = "OK.";
            success = 1;
        } else {
            //printf("output: '%s' expected: '%s'\n", output,
            //        editor->correct_stdout);
            message = "le programme ne donne pas le resultat voulu.";
        }
    }
    editor->messages = paragraph_new(message, 60, 8, 0.02*width, 0.74*height);

    if (output != NULL)
        free((void*) output);
    if (error_output != NULL)
        free((void*) error_output);
    return success;
}
#else //emscripten
// with emscripten, two functions: one launches, the other handles the result
void start_code(editor_scene *editor) {
    char *code = editor_get_content(editor->editor);
    editor->actual_stdout = NULL;
    editor->actual_stderr = NULL;
    launch_emscripten(code, &(editor->actual_stdout), &(editor->actual_stderr), &(editor->status));
    free(code);
}

int result_of_code(editor_scene *editor) {
    int success = 0;
    paragraph_destroy(editor->messages);
    char *message;
    if (editor->status == 1) {
        message = "le programme prend trop de temps. Boucle infinie ?";
    } else if (editor->actual_stderr[0] != 0) { // i.e. if stderr is not empty
        message = editor->actual_stderr;
    } else {
        if (strcmp(editor->actual_stdout, editor->correct_stdout) == 0) {
            message = "OK.";
            success = 1;
        } else {
            printf("output: '%s' expected: '%s'\n", editor->actual_stdout,
                    editor->correct_stdout);
            message = "le programme ne donne pas le resultat voulu.";
        }
    }
    editor->messages = paragraph_new(message, 60, 8, 0.02*width, 0.74*height);
    printf("%s\n", message);
    free(editor->actual_stdout);
    free(editor->actual_stderr);
    return success;
}
#endif //emscripten

// free all widgets
void editor_scene_destroy(void *data) {
    editor_scene *editor = (editor_scene*) data;
    paragraph_destroy(editor->messages);
    editor_destroy(editor->editor);
    timer_destroy(editor->timer);
}

void editor_scene_display(void *data) {
    editor_scene *editor = (editor_scene*) data;
    // nice separator between code and compiler messages
    SDL_Rect separator = {0, 0.72*height, width, 0.01*height};
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    SDL_RenderFillRect(renderer, &separator);
    // remaining time
    timer_blit(editor->timer);
    // compiler messages
    paragraph_blit(editor->messages);
    // text editor
    editor_blit(editor->editor);
}

// when using emscripten: this function must NOT be called while the worker is running
int editor_scene_handle_event(void *data, SDL_Event event) {
    editor_scene *editor = (editor_scene*) data;
    if ((event.key.keysym.sym == SDLK_RETURN) && (event.key.keysym.mod & KMOD_SHIFT)) {
#ifndef __EMSCRIPTEN__
        if (run_code(editor)) {
            if (timer_on_time(editor->timer)) {
                return 1;
            } else {
                return 0;
            }
        } else {
            return -1;
        }
#else // in emscripten, launch code and wait for another event to handle the result
        start_code(editor);
        return -2;
#endif // emscripten
    } else if ((event.key.keysym.sym == SDLK_ESCAPE) && (event.key.keysym.mod & KMOD_SHIFT)) {
        return 0; // skip this one
    } else {
        editor_handle_event(editor->editor, event);
        return -1;
    }
}

scene *editor_scene_new(char *code_file) {

    // store data
    scene *new_scene =  (scene*) malloc(sizeof(scene));
    editor_scene *new_editor_scene =
        (editor_scene*) malloc(sizeof(editor_scene));
    new_scene->data = (void*) new_editor_scene;

    //create widgets
    // TODO : remove static sizes
    char *welcome = "Bienvenue sur le quizz.\n\
    Tapez Shift-Entree pour lancer le code.\n\
    Les fleches pour se deplacer.\n\
    Shift-espace pour sauter le quizz en cours.";
    new_editor_scene->messages = paragraph_new(welcome, 60, 8,
            0.02*width, 0.74*height);

    new_editor_scene->editor = editor_new(code_file, 20, 20);

    unsigned int seconds = editor_get_alloted_time(new_editor_scene->editor);
    new_editor_scene->timer = timer_new(seconds*1000, 0.8*width, 0.8*height);

    char *code = slurp(code_file);
    new_editor_scene->correct_stdout = NULL;
#ifdef __EMSCRIPTEN__
    new_editor_scene->actual_stderr = NULL;
    new_editor_scene->actual_stdout = NULL;
    launch_emscripten(code, &new_editor_scene->correct_stdout,
            &new_editor_scene->actual_stderr,
            &new_editor_scene->status);
#else
    launch(code, &new_editor_scene->correct_stdout, NULL);
#endif
    free((void*) code);

    // store callbacks
    new_scene->display = editor_scene_display;
    new_scene->handle_event = editor_scene_handle_event;
    new_scene->destroy = editor_scene_destroy;

    return new_scene;
}

#ifdef __EMSCRIPTEN__
// returns: -2 if the worker is still running, -1 if the quizz is not solved yet
// 0 if it is solved but the time is out, 1 if it is solved in time
int handle_worker_events(void *data) {
    editor_scene *editor = (editor_scene*) data;
    if (editor->status < 0) // worker is still running, nothing to do
        return -2;
    if (editor->actual_stdout == NULL && editor->status == 0) {//we were just initializing correct_stdout
        if (editor->actual_stderr) { // discard the stderr from the sample run
            free(editor->actual_stderr);
            editor->actual_stderr = NULL;
        }
        return -1;
    }
    // last case, the worker has finished and was running the student code
    if (result_of_code(editor)) { // correct output
        if (timer_on_time(editor->timer)) {
            return 1;
        } else {
            return 0;
        }
    } else {
        return -1;
    }
}
#endif
