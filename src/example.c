#include "anim.h"
#include <stdio.h>

float x=0, y=0, z=0;

void show_animation(Animation* a, float step);

int main() {
    Animation* a = parallelpn(sequencen(sinusoid(scale(linearf1(&x, 0, 4), 5)),
                                        sinusoid(scale(linearf1(&x, 4, 2), 3)),
                                        NULL),
                              sequencen(delay(sinusoid(scale(linearf1(&y, 0, 4), 3)), 3),
                                        NULL),
                              sequencen(scale(linearf1(&z, 2, 1), 3),
                                        scale(linearf1(&z, 1, 3), 3),
                                        scale(linearf1(&z, 3, 1), 2),
                                        NULL),
                              NULL);
    show_animation(a, 0.02);
    return 0;
}

void show_animation(Animation* a, float step) {
    float t, d = animation_duration(a);
    for (t=0.0; t<d; t+=step) {
        animation_update(a, t);
        printf("%f %f %f %f\n", t, x, y, z);
    }

    animation_update(a, d);
    printf("%f %f %f %f\n", d, x, y, z);
}
