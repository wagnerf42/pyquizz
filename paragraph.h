#include<SDL.h>
#include<SDL_ttf.h>

typedef struct _paragraph_widget {
    unsigned int height_limit;
    unsigned int lines_number;
    SDL_Texture **lines_textures;
    SDL_Rect top_left;
    char *given_text; // carefull, do not use directly (split in lines)
} paragraph_widget;

paragraph_widget *paragraph_new(char *text,
        unsigned int width, unsigned int height,
        Sint16 x, Sint16 y);
void paragraph_blit(paragraph_widget *paragraph);
void paragraph_destroy(paragraph_widget *paragraph);
