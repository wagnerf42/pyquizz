#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<assert.h>
#include"editor.h"
#include"globals.h"

//return number of lines before '###' (non hidden lines)
unsigned int count_lines(char *filename) {
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        perror("unable to read code file");
        exit(1);
    }
    unsigned int count = 0;
    char *buffer = NULL;
    size_t size;
    while(getline(&buffer, &size, f) > 0) {
        if ((buffer[0] == '#')&&(buffer[1] == '#')&&(buffer[2] == '#')) break;
        count++;
    }
    fclose(f);
    return count;
}

// return everything after '##' or NULL if not found
char *parse_line(char *buffer) {
    unsigned int index = 0;
    unsigned int previous_ok = 0;
    while(buffer[index] != 0) {
        if (buffer[index] == '#') {
            if (previous_ok) {
                return buffer + index + 1;
            } else {
                previous_ok = 1;
            }
        } else {
            previous_ok = 0;
        }
        index++;
    }
    return NULL;
}

int read_lines(editor_widget *editor, char *filename) {
    // there is a race condition here
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        perror("unable to read code file");
        exit(1);
    }
    char *buffer = NULL;
    size_t size;
    int line_number;
    int first_modifiable_line = -1;
    for(line_number = 0 ; line_number < editor->lines_number ;
            line_number++) {

        getline(&buffer, &size, f);
        // remove \n
        buffer[strlen(buffer)-1] = 0;

        char *replaced_line = parse_line(buffer);
        if (replaced_line != NULL) {
            if (first_modifiable_line == -1)
                first_modifiable_line = line_number;
            editor->lines[line_number] = line_new(
                    replaced_line, line_number, 0);
        } else {
            editor->lines[line_number] = line_new(
                    buffer, line_number, 1);
        }
    }
    long hidden_part_start = ftell(f);
    fseek(f, 0, SEEK_END);
    long end = ftell(f);
    unsigned int hidden_size = end - hidden_part_start;
    editor->hidden_lines = (char*) malloc(hidden_size);
    fseek(f, hidden_part_start, SEEK_SET);
    fread(editor->hidden_lines, 1, hidden_size, f);
    editor->hidden_lines[hidden_size-1] = 0;
    fclose(f);
    return first_modifiable_line;
}

editor_widget *editor_new(char *filename,
        unsigned int x, unsigned int y) {


    editor_widget *editor = (editor_widget*)
        malloc(sizeof(editor_widget));
    editor->position = (SDL_Rect) {x, y, 0, 0};
    editor->lines_number = count_lines(filename);
    editor->font_height = TTF_FontHeight(smallfont);
    SDL_Surface *letter = TTF_RenderText_Blended(smallfont, "a",
            color);
    editor->font_width = letter->w;
    SDL_FreeSurface(letter);

    editor->lines = (line_widget**)
        malloc(sizeof(line_widget*) * editor->lines_number);

    editor->cursor_y = read_lines(editor, filename);
    editor->cursor_x = strlen(editor->lines[editor->cursor_y]->text);

    return editor;
}

// figure where cursor is on current line
unsigned int editor_visible_cursor_x(editor_widget *editor) {
    unsigned int y = editor->cursor_y;
    line_widget *line = editor->lines[y];
    unsigned int seen_x = strlen(line->text);
    if (seen_x > editor->cursor_x) {
        seen_x = editor->cursor_x;
    }
    return seen_x;
}

void editor_blit(editor_widget *editor) {
    int i;
    unsigned int x = editor->position.x;
    unsigned int y = editor->position.y;
    for(i = 0 ; i < editor->lines_number ; i++) {
        line_blit(editor->lines[i], x, y + editor->font_height *i,
                editor->font_width);
    }

    // blit cursor
    unsigned int ticks = SDL_GetTicks();
    SDL_Rect cursor_position;
    unsigned int seen_x = editor_visible_cursor_x(editor);
    cursor_position.x = editor->position.x +
        (seen_x + 3) * editor->font_width;
    cursor_position.y = editor->position.y +
        editor->cursor_y * editor->font_height;
    cursor_position.w = editor->font_width;
    cursor_position.h = editor->font_height;
    if ((ticks/1000) % 2) {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
        SDL_RenderFillRect(renderer, &cursor_position);
    }
}

// go to previous editable line if possible
int editor_move_up(editor_widget *editor) {
    int y;
    for(y = editor->cursor_y - 1 ; y >= 0; y--) {
        if (!editor->lines[y]->readonly) {
            editor->cursor_y = y;
            return 1;
        }
    }
    return 0;
}

// go to next editable line if possible
int editor_move_down(editor_widget *editor) {
    int y;
    for(y = editor->cursor_y + 1 ; y < editor->lines_number ; y++) {
        if (!editor->lines[y]->readonly) {
            editor->cursor_y = y;
            return 1;
        }
    }
    return 0;
}

int editor_move_right(editor_widget *editor) {
    line_widget *line = editor->lines[editor->cursor_y];
    unsigned int seen_x = editor_visible_cursor_x(editor);
    if (seen_x != strlen(line->text)) {
        editor->cursor_x = seen_x + 1;
        return 1;
    } else {
        int line_changed = editor_move_down(editor);
        if (line_changed) {
            editor->cursor_x = 0;
            return 1;
        }
        return 0;
    }
}

int editor_move_left(editor_widget *editor) {
    unsigned int seen_x = editor_visible_cursor_x(editor);
    if (seen_x == 0) {
        int line_changed = editor_move_up(editor);
        if (line_changed) {
            // we changed line, change x
            line_widget *line = editor->lines[editor->cursor_y];
            editor->cursor_x = strlen(line->text);
            return 1;
        }
        return 0;
    } else {
        editor->cursor_x = seen_x - 1;
        return 1;
    }
}

// delete char under cursor
void editor_delete(editor_widget *editor) {
    unsigned int seen_x = editor_visible_cursor_x(editor);
    line_remove(editor->lines[editor->cursor_y], seen_x);
}

void editor_handle_event(editor_widget *editor, SDL_Event event) {

    if (event.common.type == SDL_KEYDOWN) {
        SDL_Keycode key = event.key.keysym.sym;
        switch (key) {
            case SDLK_UP:
                editor_move_up(editor);
                break;
            case SDLK_DOWN:
                editor_move_down(editor);
                break;
            case SDLK_RIGHT:
                editor_move_right(editor);
                break;
            case SDLK_LEFT:
                editor_move_left(editor);
                break;
            case SDLK_BACKSPACE:
                if (editor_move_left(editor)) {
                    editor_delete(editor);
                }
                break;
            case SDLK_DELETE:
                editor_delete(editor);
                break;
            default:
                break;
        }
    } else {
        int seen_x = editor_visible_cursor_x(editor);
        editor->cursor_x = seen_x + line_insert(
                editor->lines[editor->cursor_y],
                event.text.text[0], seen_x);
    }
}

char *editor_get_content(editor_widget *editor) {
    unsigned int size = editor->lines_number * 81 +
        strlen(editor->hidden_lines) + 1;
    char *content = (char*) malloc(size);
    content[0] = 0;
    int line;
    for(line = 0 ; line < editor->lines_number ; line++) {
        strcat(content, editor->lines[line]->text);
        strcat(content, "\n");
    }
    strcat(content, editor->hidden_lines);
    return content;
}

void editor_destroy(editor_widget *editor) {
    int line = 0;
    for(line = 0 ; line < editor->lines_number ; line++) {
        line_destroy(editor->lines[line]);
    }
    free((void*) editor->hidden_lines);
    free((void*) editor);
}

unsigned int editor_get_alloted_time(editor_widget *editor) {
    // information is contained in first hidden line
    assert(editor->hidden_lines[0] == '#');
    assert(editor->hidden_lines[1] == '#');
    assert(editor->hidden_lines[2] == '#');
    unsigned int alloted_time;
    int parse_ok = sscanf(editor->hidden_lines, "###%d", &alloted_time);
    if (parse_ok > 0) {
        return alloted_time;
    } else {
        return 60;
    }
}
