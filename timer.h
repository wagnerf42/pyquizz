#include<SDL.h>
#include<SDL_ttf.h>

typedef struct _timer_widget {
    int remaining_seconds;
    SDL_Texture *texture;
    Uint32 deadline;
    SDL_Rect position;
} timer_widget;

extern timer_widget *timer_new(Uint32 deadline,
        Sint16 x, Sint16 y);

extern void timer_blit(timer_widget *timer);
extern void timer_destroy(timer_widget *timer);
// return if we still have time left
extern int timer_on_time(timer_widget *timer);
