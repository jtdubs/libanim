#include "anim.h"
#include <stdio.h>
#include <math.h>

float x=0, y=0, z=0, s1=0, s2=0;

void show_animation(Animation* a, float step);

float fcos(float x) {
    return cos(x);
}

int main() {
    Animation* a = attach(parallelpn(sequencen(sinusoid(scale(linearf1(&x, 0, 4), 5)),
                                               sinusoid(scale(linearf1(&x, 4, 2), 3)),
                                               NULL),
                                     sequencen(delay(sinusoid(scale(linearf1(&y, 0, 4), 3)), 3),
                                               NULL),
                                     sequencen(scale(attachn(linearf1(&z, 2, 1),
                                                             derivef1(fcos, &z, &s1),
                                                             NULL),
                                                     3),
                                               scale(linearf1(&z, 1, 3), 3),
                                               scale(linearf1(&z, 3, 1), 2),
                                               NULL),
                                     NULL),
                          derivef1(fcos, &x, &s2));
    show_animation(a, 0.02);
    animation_free(a);
    return 0;
}

void show_animation(Animation* a, float step) {
    float t, d = animation_duration(a);
    for (t=0.0; t<d; t+=step) {
        animation_update(a, t);
        printf("%f %f %f %f %f %f\n", t, x, y, z, s1, s2);
    }

    animation_update(a, d);
    printf("%f %f %f %f %f %f\n", d, x, y, z, s1, s2);
}
