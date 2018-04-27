#include<sys/time.h>
#include<stdio.h>
#include<stdlib.h>
#include"timer.h"
#include"globals.h"

timer_widget *timer_new(Uint32 deadline,
        Sint16 x, Sint16 y) {

    timer_widget *timer = (timer_widget*) malloc(sizeof(timer_widget));
    timer->deadline = SDL_GetTicks() + deadline;
    timer->position = (SDL_Rect) {x, y, 0, 0};
    timer->texture = NULL;

    return timer;
}

void timer_blit(timer_widget *timer) {
    // Attention ici, les temps sont non signés
    int remaining_seconds =
      (timer->deadline <= SDL_GetTicks()) ? 0 : (timer->deadline - SDL_GetTicks())/1000;

    if (timer->remaining_seconds != remaining_seconds) {
        SDL_Color selected_color = color;
        SDL_Color red = {255, 0, 0, 255};
        if (remaining_seconds == 0) {
            selected_color = red;
        }

        char text[7];
        snprintf(text, 7, "%2d", remaining_seconds);

        SDL_Surface *surface = TTF_RenderText_Blended(
                bigfont,
                text,
                selected_color);
        if (timer->texture != NULL) {
            SDL_DestroyTexture(timer->texture);
        }
        timer->texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_QueryTexture(timer->texture, NULL, NULL,
                &timer->position.w, &timer->position.h);
        SDL_FreeSurface(surface);
    }

    SDL_RenderCopy(renderer, timer->texture, NULL, &timer->position);
}

void timer_destroy(timer_widget *timer) {
    free((void*) timer);
}

int timer_on_time(timer_widget *timer) {
  return (timer->deadline >= SDL_GetTicks());
}
