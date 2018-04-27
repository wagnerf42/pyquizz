#include<SDL.h>
#include<SDL_ttf.h>
#include"line.h"

typedef struct _editor_widget {
    line_widget **lines;
    unsigned int lines_number;
    unsigned int cursor_x;
    unsigned int cursor_y;
    SDL_Rect position;
    unsigned int font_height;
    unsigned int font_width;
    char *hidden_lines;
} editor_widget;

extern editor_widget *editor_new(char *filename,
        unsigned int x, unsigned int y);

extern void editor_blit(editor_widget *editor);
extern void editor_handle_event(editor_widget *editor, SDL_Event event);
extern char *editor_get_content(editor_widget *editor);
extern void editor_destroy(editor_widget *editor);
// return number of seconds alloted for answering current file
extern unsigned int editor_get_alloted_time(editor_widget *editor);
