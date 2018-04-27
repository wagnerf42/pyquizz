#include<SDL.h>
#include<SDL_ttf.h>

typedef struct _line_widget {
    char *text;
    unsigned int readonly;
    SDL_Texture *text_texture;
    SDL_Texture *number_texture;
} line_widget;

extern line_widget *line_new(char *text, unsigned int line_number,
        unsigned int readonly);
extern void line_blit(line_widget *line,
        unsigned int x, unsigned int y,
        unsigned int width);
// add a character at given place
extern unsigned int line_insert(line_widget *line, char character,
        unsigned int index);
// remove a character at given place
extern void line_remove(line_widget *line, unsigned int index);
// free memory
extern void line_destroy(line_widget *line);
