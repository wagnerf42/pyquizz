#include<assert.h>
#include<dirent.h>
#include<SDL_image.h>
#include"scene.h"
#include"globals.h"

typedef struct _menu_scene {
    struct dirent **entries;
    unsigned int size;
    SDL_Texture **selected_textures;
    SDL_Texture **unselected_textures;
    SDL_Rect *positions;
    unsigned int selected_entry;
    SDL_Texture *logo;
    SDL_Rect logo_position;
} menu_scene;

int menu_scene_handle_event(void *data, SDL_Event event) {
    menu_scene *menu = (menu_scene*) data;
    switch(event.key.keysym.sym) {
        case SDLK_ESCAPE:
            return -3; // exit
        case SDLK_RETURN:
            return 0;
            break;
        case SDLK_UP:
        case SDLK_k:
            if (menu->selected_entry == 0) {
                menu->selected_entry = menu->size - 1;
            } else {
                menu->selected_entry--;
            }
            break;
        case SDLK_DOWN:
        case SDLK_j:
            menu->selected_entry++;
            menu->selected_entry %= menu->size;
            break;
        default:
            return -1;
    }
    return -1;
}

void menu_scene_destroy(void *data) {
    menu_scene *menu = (menu_scene*) data;
    int i;
    for(i = 0 ; i < menu->size ; i++) {
        SDL_DestroyTexture(menu->selected_textures[i]);
        SDL_DestroyTexture(menu->unselected_textures[i]);
        free(menu->entries[i]);
    }
    free(menu->selected_textures);
    free(menu->unselected_textures);
    free(menu->positions);
    free(menu->entries);
}

void menu_scene_display(void *data) {
    menu_scene *menu = (menu_scene*) data;
    int i;
    int ticks = SDL_GetTicks();
    for(i = 0 ; i < menu->size ; i++) {
        if ((menu->selected_entry == i)&&((ticks/800)%2)) {
            SDL_RenderCopy(renderer, menu->selected_textures[i], NULL,
                    &menu->positions[i]);
        } else {
            SDL_RenderCopy(renderer, menu->unselected_textures[i], NULL,
                    &menu->positions[i]);
        }
    }
    SDL_RenderCopy(renderer, menu->logo, NULL, &menu->logo_position);
}

// small filter to discard hidden files
int filter(const struct dirent* entry) {
    return entry->d_name[0] != '.';
}

extern scene *menu_scene_new(char *pattern) {

    scene *new_scene = (scene*) malloc(sizeof(scene));
    menu_scene *menu = (menu_scene*) malloc(sizeof(menu_scene));
    new_scene->data = (void*) menu;

    menu->selected_entry = 0;

    //find entries and create surfaces
    menu->size = scandir("data/lessons", &menu->entries, filter, alphasort);
    int i;
    menu->selected_textures = (SDL_Texture**)
        malloc(menu->size*sizeof(SDL_Texture*));
    menu->unselected_textures = (SDL_Texture**)
        malloc(menu->size*sizeof(SDL_Texture*));
    menu->positions = (SDL_Rect*)
        malloc(menu->size*sizeof(SDL_Rect));
    SDL_Color black = {0, 0, 0, 255};
    TTF_Font *menu_font = TTF_OpenFont("data/Inconsolata-Regular.ttf", 40);
    for(i = 0 ; i < menu->size; i++) {
        SDL_Surface *surface = TTF_RenderText_Shaded(
                menu_font,
                menu->entries[i]->d_name,
                black,
                color
                );
        menu->selected_textures[i] = SDL_CreateTextureFromSurface(renderer,
                surface);
        SDL_FreeSurface(surface);
        surface = TTF_RenderText_Blended(
                menu_font,
                menu->entries[i]->d_name,
                color
                );
        menu->unselected_textures[i] = SDL_CreateTextureFromSurface(renderer,
                surface);
        SDL_FreeSurface(surface);
    }
    TTF_CloseFont(menu_font);

    // compute all positions
    int entry_height;
    SDL_QueryTexture(menu->selected_textures[0],
            NULL, NULL, NULL, &entry_height);
    int used_height = menu->size * entry_height;
    int remaining_height = height - used_height;
    assert(remaining_height > 0);
    int skipped_pixels = remaining_height / (menu->size+1);
    int current_height = skipped_pixels;
    for(i = 0 ; i < menu->size ; i++) {
        SDL_QueryTexture(menu->selected_textures[i],
                NULL, NULL,
                &menu->positions[i].w,
                &menu->positions[i].h);
        menu->positions[i].x = width/2 - menu->positions[i].w/2;
        menu->positions[i].y = current_height;
        current_height += entry_height + skipped_pixels;
    }
    // load ensimag logo
    menu->logo = IMG_LoadTexture(renderer, "data/ensimag.png");
    menu->logo_position.x = 0;
    SDL_QueryTexture(menu->logo,
            NULL, NULL,
            &menu->logo_position.w,
            &menu->logo_position.h);
    menu->logo_position.y = height - menu->logo_position.h;

    // store callbacks
    new_scene->display = menu_scene_display;
    new_scene->handle_event = menu_scene_handle_event;
    new_scene->destroy = menu_scene_destroy;
    return new_scene;
}

char *menu_get_selection(void *menu_data) {
    menu_scene *menu = (menu_scene*) menu_data;
    return menu->entries[menu->selected_entry]->d_name;
}
