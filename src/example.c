#include "anim.h"
#include <stdio.h>

float x=0, y=0, z=0;

void show_animation(Animation* a, float step);

int main() {
    Animation* a = sinusoid(scale(linearf1(&x, 0, 1), 5));
    show_animation(a, 0.1);
    return 0;
}

void show_animation(Animation* a, float step) {
    float t, d = animation_duration(a);
    for (t=0.0; t<d; t+=step) {
        update_animation(a, t);
        printf("%f %f %f %f\n", t, x, y, z);
    }

    update_animation(a, d);
    printf("%f %f %f %f\n", t, x, y, z);
}
