#include<stdlib.h>
#include"scene.h"

void scene_destroy(scene *s) {
    s->destroy(s->data);
    free((void*) s);
};
