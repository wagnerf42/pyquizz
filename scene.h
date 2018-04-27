#include<SDL.h>
#include<SDL_ttf.h>

typedef struct _scene {
    void *data;
    // virtual methods for subclasses ;-)
    void (*display)(void *data);
    void (*destroy)(void *data);
    int (*handle_event)(void *data, SDL_Event event);
} scene;

// different scenes available
extern scene *editor_scene_new(char *code_file);
extern scene *end_scene_new(unsigned int success_rate);
extern scene *menu_scene_new(char *pattern);

#ifdef __EMSCRIPTEN__
int handle_worker_events(void *data);
#endif

// get selected entry in menu
extern char *menu_get_selection(void *menu_data);

// free memory
extern void scene_destroy(scene *s);
