#include "anim.h"
#include <stdio.h>
#include <math.h>

float theta=0, x=0, y=0;

void show_animation(Animation* a, float step);

int main() {
    Animation* a = attachn(sequencen(sinusoid(scale(linearf1(&theta, 0, 4), 5)),
                                     delay(sinusoid(scale(linearf1(&theta, 4, 2), 3)), 1),
                                     NULL),
                           deriveff(cosf, &theta, &x),
                           deriveff(sinf, &theta, &y),
                           NULL);
    show_animation(a, 0.02);
    animation_free(a);
    return 0;
}

void show_animation(Animation* a, float step) {
    float t, d = animation_duration(a);
    for (t=0.0; t<d; t+=step) {
        animation_update(a, t);
        printf("%f %f %f %f\n", t, theta, x, y);
    }

    animation_update(a, d);
    printf("%f %f %f %f\n", t, theta, x, y);
}
