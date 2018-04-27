#include<stdlib.h>
#include<string.h>
#include"paragraph.h"
#include"globals.h"

void split_lines(char *text, unsigned int limit) {
    // split text into lines at given width limit
    unsigned int previous_start = 0;
    int previous_space = -1;
    unsigned int i = 0;
    while(text[i] != 0) {
        if (text[i] == ' ') {
            previous_space = i;
        }
        if ((i - previous_start) >= limit) {
            text[previous_space] = '\n';
            previous_start = previous_space+1;
        }
        if (text[i] == '\n') {
            previous_start = i+1;
        }
        i++;
    }
}

void paragraph_append_line(paragraph_widget *paragraph, char *line,
        SDL_Renderer *renderer) {
    if (line[0] == 0) {
        paragraph->lines_textures[paragraph->lines_number] = NULL;
    } else {
        if (paragraph->lines_number < paragraph->height_limit) {
            SDL_Surface *surface =
                TTF_RenderText_Blended(smallfont, line, color);

            paragraph->lines_textures[paragraph->lines_number] =
                SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);
        }
    }
    if (paragraph->lines_number < paragraph->height_limit)
        paragraph->lines_number++;
}

void foreach_line(char *text,
        void (*callback)(paragraph_widget*, char*, SDL_Renderer*),
        paragraph_widget *paragraph, SDL_Renderer *renderer) {
    unsigned int i = 0;
    unsigned int start = 0;
    while(text[i] != 0) {
        if (text[i] == '\n') {
            text[i] = 0;
            callback(paragraph, text + start, renderer);
            start = i+1;
        }
        i++;
    }
    if (text[start] != 0) callback(paragraph, text + start, renderer);
}

paragraph_widget *paragraph_new(char *text,
        unsigned int width, unsigned int height,
        Sint16 x, Sint16 y) {
    paragraph_widget *paragraph = (paragraph_widget*)
        malloc(sizeof(paragraph_widget));
    paragraph->top_left.x = x;
    paragraph->top_left.y = y;
    paragraph->height_limit = height;

    paragraph->lines_number = 0;
    paragraph->lines_textures = (SDL_Texture **)
        malloc(sizeof(SDL_Texture*)*height);

    paragraph->given_text = (char*) malloc(1+strlen(text));
    strcpy(paragraph->given_text, text);
    split_lines(paragraph->given_text, width);
    foreach_line(paragraph->given_text, paragraph_append_line, paragraph,
            renderer);
    return paragraph;
}

void paragraph_blit(paragraph_widget *paragraph) {
    int line_index;
    int height = TTF_FontHeight(smallfont);
    SDL_Rect position = paragraph->top_left;
    for(line_index = 0 ; line_index < paragraph->lines_number ; line_index++) {
        SDL_QueryTexture(paragraph->lines_textures[line_index], NULL, NULL,
                &position.w, &position.h);
        if (paragraph->lines_textures[line_index] != NULL) {
            SDL_RenderCopy(renderer, paragraph->lines_textures[line_index],
                    NULL, &position);
        }
        position.y += height;
    }
}

void paragraph_destroy(paragraph_widget *paragraph) {
    free((void*) paragraph->given_text);
    int line;
    for(line = 0 ; line < paragraph->lines_number ; line++) {
        SDL_DestroyTexture(paragraph->lines_textures[line]);
    }
    free((void*) paragraph);
}
