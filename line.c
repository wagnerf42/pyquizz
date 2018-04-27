#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include"line.h"
#include"globals.h"

// recompute texture
static void line_render_text(line_widget *line) {
    if (line->text_texture != NULL) {
        SDL_DestroyTexture(line->text_texture);
    }
    SDL_Surface *surface = TTF_RenderText_Blended(smallfont,
            line->text, color);
    line->text_texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
}

line_widget *line_new(char *text, unsigned int line_number,
        unsigned int readonly) {

    line_widget *line = (line_widget*) malloc(sizeof(line_widget));
    line->text = (char*) malloc(sizeof(char)*81);
    if (text == NULL) {
        line->text[0] = 0;
    } else {
        strncpy(line->text, text, 80);
    }
    line->readonly = readonly;
    line->text_texture = NULL;

    // generate line number surface
    SDL_Color dim_color;
    dim_color.r = color.r / 2;
    dim_color.g = color.g / 2;
    dim_color.b = color.b / 2;
    char number[4];
    snprintf(number, 4, "%2d", line_number+1);
    if (readonly) {
        SDL_Surface *surface = TTF_RenderText_Blended(
                smallfont,
                number,
                dim_color
                );
        line->number_texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    } else {
        SDL_Color black = {0, 0, 0, 255};
        SDL_Surface *surface = TTF_RenderText_Shaded(
                smallfont,
                number,
                black,
                dim_color
                );
        line->number_texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
    }

    // generate line surface
    if (line->text[0] != 0) {
        line->text_texture = NULL;
        line_render_text(line);
    }
    return line;
}

void line_blit(line_widget *line,
        unsigned int x, unsigned int y,
        unsigned int width) {

    SDL_Rect number_position = {x, y, 0, 0};
    SDL_QueryTexture(line->number_texture, NULL, NULL,
            &number_position.w, &number_position.h);
    SDL_RenderCopy(renderer, line->number_texture, NULL, &number_position);

    SDL_Rect text_position = {x + 3*width, y, 0, 0};
    SDL_QueryTexture(line->text_texture, NULL, NULL,
            &text_position.w, &text_position.h);
    if (line->text_texture != NULL) {
        SDL_RenderCopy(renderer, line->text_texture, NULL, &text_position);
    }
}

unsigned int line_insert(line_widget *line, char character,
        unsigned int index) {

    if (strlen(line->text) == 80) {
        return 0;
    }
    //change string
    char displaced = character;
    while (line->text[index] != 0) {
        char tmp;
        tmp = line->text[index];
        line->text[index] = displaced;
        displaced = tmp;
        index++;
    }
    line->text[index] = displaced;
    line->text[index+1] = 0;
    line_render_text(line);
    return 1;
}

void line_remove(line_widget *line, unsigned int index) {
    while(line->text[index] != 0) {
        line->text[index] = line->text[index+1];
        index++;
    }
    line_render_text(line);
}

void line_destroy(line_widget *line) {
    free((void*) line->text);
    SDL_DestroyTexture(line->text_texture);
    SDL_DestroyTexture(line->number_texture);
    free((void*) line);
}
