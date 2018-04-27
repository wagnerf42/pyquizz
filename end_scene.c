#include<assert.h>
#include"scene.h"
#include"globals.h"

char *messages[6] = {
    "courage, il reste encore du boulot",
    "ca commence a donner quelque chose",
    "pas mal",
    "pas mal du tout",
    "c'est bon pour ce chapitre",
    "parfait"
};

typedef struct _end_scene {
    unsigned int start_ticks;
    float current_factor;

    char big_string[5];
    SDL_Texture *big_message;
    SDL_Rect big_position;

    char *small_string;
    SDL_Texture *small_message;
    SDL_Rect small_position;
} end_scene;

int end_scene_handle_event(void *data, SDL_Event event) {
    return 1;
}

void end_scene_destroy(void *data) {
    end_scene *end = (end_scene*) data;
    SDL_DestroyTexture(end->big_message);
    SDL_DestroyTexture(end->small_message);
}

void end_scene_render_messages(end_scene *end) {
    SDL_Color faded_color;
    faded_color.r = (int)(color.r * end->current_factor);
    faded_color.g = (int)(color.g * end->current_factor);
    faded_color.b = (int)(color.b * end->current_factor);
    SDL_Surface *surface = TTF_RenderText_Blended(bigfont, end->big_string,
            faded_color);
    end->big_message = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    surface = TTF_RenderText_Blended(smallfont,
            end->small_string,
            faded_color);
    end->small_message = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
}

void end_scene_display(void *data) {
    // display messages with a small fade in
    end_scene *end = (end_scene*) data;
    unsigned int ticks = SDL_GetTicks();
    float factor;
    if ((ticks - end->start_ticks) > 2000) {
        factor = 1.0;
    } else {
        factor = (ticks - end->start_ticks) / 2000.0;
    }
    if (factor != end->current_factor) {
        end->current_factor = factor;
        SDL_DestroyTexture(end->big_message);
        SDL_DestroyTexture(end->small_message);
        end_scene_render_messages(end);
    }

    SDL_RenderCopy(renderer, end->big_message, NULL, &end->big_position);
    SDL_RenderCopy(renderer, end->small_message, NULL, &end->small_position);
}

scene *end_scene_new(unsigned int success_rate) {

    scene *new_scene = (scene*) malloc(sizeof(scene));
    end_scene *end = (end_scene*) malloc(sizeof(end_scene));
    new_scene->data = (void*) end;

    end->start_ticks = SDL_GetTicks();
    assert(success_rate <= 100);
    sprintf(end->big_string, "%d%%", success_rate);
    end->small_string = messages[success_rate/20];

    end->current_factor = 0.0;
    end_scene_render_messages(end);
    int w, h;
    SDL_QueryTexture(end->big_message, NULL, NULL, &w, &h);
    end->big_position = (SDL_Rect) { width/2 - w/2, height/2 - h/2, w, h};
    SDL_QueryTexture(end->small_message, NULL, NULL, &w, &h);
    end->small_position =
        (SDL_Rect) { width/2 - w/2, height/2 + h/2 + 30, w, h};
    

    // store callbacks
    new_scene->display = end_scene_display;
    new_scene->handle_event = end_scene_handle_event;
    new_scene->destroy = end_scene_destroy;
    return new_scene;
}
