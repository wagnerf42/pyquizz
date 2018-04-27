#include<SDL.h>
#include<SDL_ttf.h>
//some global vars to avoid propagating these values everywhere
//initialized early, once and then read-only
extern int width;
extern int height;
extern SDL_Color color;
extern TTF_Font *bigfont;
extern TTF_Font *smallfont;
extern SDL_Renderer *renderer;
