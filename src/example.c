#include "anim.h"
#include <stdio.h>
#include <math.h>

float theta=0, v[2], r;

void show_animation(Animation* a, float step);

void pt_from_angle(int n, float *theta, float *v) {
    v[0] = cos(*theta);
    v[1] = sin(*theta);
}

int main() {
    Animation* a = attachn(sequencen(sinusoid(scale(linearf1(&theta, 0, 4), 5)),
                                     delay(sinusoid(scale(linearf1(&theta, 4, 2), 3)), 1),
                                     NULL),
                           derive((TransformN)pt_from_angle, 1, &theta, &v),
                           deriveff(roundf, &theta, &r),
                           NULL);
    show_animation(a, 0.02);
    animation_free(a);
    return 0;
}

void show_animation(Animation* a, float step) {
    float t, d = animation_duration(a);
    for (t=0.0; t<d; t+=step) {
        animation_update(a, t);
        printf("%f %f %f %f %f\n", t, theta, v[0], v[1], r);
    }

    animation_update(a, d);
    printf("%f %f %f %f %f\n", t, theta, v[0], v[1], r);
}
