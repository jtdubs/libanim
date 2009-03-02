#include "anim.h"

#include <glib.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

/* time transformations - modify the relationship between external time and animation time by transforming numbers in the range [0,1] */

typedef float (*TimeTransformFunction)(TimeTransform*, float);

struct TimeTransformStruct {
    TimeTransformFunction f;
};

float apply_transform(TimeTransform* t, float f) {
    return t->f(t, f);
}

/* identity transform */

float identity_transform_function(TimeTransform* t, float f) {
    return f;
}

TimeTransform* identity_transform() {
    TimeTransform* t = malloc(sizeof(TimeTransform));
    t->f = identity_transform_function;
    return t;
}

/* sinusoid transform */

float sinusoid_transform_function(TimeTransform* t, float f) {
    return sin(f * G_PI_2);
}

TimeTransform* sinusoid_transform() {
    TimeTransform* t = malloc(sizeof(TimeTransform));
    t->f = sinusoid_transform_function;
    return t;
}

/* reverse transform */

float reverse_transform_function(TimeTransform* t, float f) {
    return 1.0 - f;
}

TimeTransform* reverse_transform() {
    TimeTransform* t = malloc(sizeof(TimeTransform));
    t->f = reverse_transform_function;
    return t;
}

/* exp transform */

typedef struct ExpTransformStruct {
    TimeTransform t;
    float exp;
} ExpTransform;

float exp_transform_function(TimeTransform* t, float f) {
    ExpTransform* e = (ExpTransform*)t;
    return pow(f, e->exp);
}

TimeTransform* exp_transform(float exp) {
    ExpTransform* t = malloc(sizeof(ExpTransform));
    t->t.f = exp_transform_function;
    t->exp = exp;
    return (TimeTransform*)t;
}

/* animations */

typedef void (*UpdateAnimationFunction)(Animation*, float f);
typedef float (*AnimationDurationFunction)(Animation*);
typedef void (*FreeAnimationFunction)(Animation*);

struct AnimationStruct {
    UpdateAnimationFunction update;
    AnimationDurationFunction duration;
    FreeAnimationFunction free;
};

void update_animation(Animation* a, float f) {
    a->update(a, f);
}

float animation_duration(Animation* a) {
    return a->duration(a);
}

void free_animation(Animation* a) {
    a->free(a);
}

float default_animation_duration(Animation* a) {
    return 1.0;
}

void default_free_animation(Animation* a) {
    free(a);
}

/* null animation */

typedef struct NullAnimationStruct {
    Animation a;
} NullAnimation;

void update_null_animation(Animation* a, float f) {
}

Animation* null_animation() {
    NullAnimation* a = malloc(sizeof(NullAnimation));
    a->a.update   = update_null_animation;
    a->a.duration = default_animation_duration;
    a->a.free     = default_free_animation;
    return (Animation*)a;
}

/* linear animation (float) */

typedef struct LinearAnimationFStruct {
    Animation a;
    float* v;
    int n;
    float* start;
    float* end;
} LinearAnimationF;

void update_linear_animationf(Animation* a, float f) {
    LinearAnimationF* la = (LinearAnimationF*)a;

    int i;
    for (i=0; i<la->n; i++)
        la->v[i] = la->start[i] + (la->end[i] - la->start[i]) * f;
}

void free_linear_animationf(Animation* a) {
    LinearAnimationF* la = (LinearAnimationF*)a;
    free(la->start);
    free(la->end);
    default_free_animation(a);
}

Animation* linearf(float* v, int n, float* start, float* end) {
    LinearAnimationF* a = malloc(sizeof(LinearAnimationF));
    a->a.update   = update_linear_animationf;
    a->a.duration = default_animation_duration;
    a->a.free     = free_linear_animationf;
    a->v     = v;
    a->n     = n;
    a->start = start;
    a->end   = end;
    return (Animation*)a;
}

Animation* linearf1(float* v, float start, float end) {
    float* s = malloc(sizeof(float));
    float* e = malloc(sizeof(float));

    *s = start;
    *e = end;

    return linearf(v, 1, s, e);
}

/* linear animation (int) */

typedef struct LinearAnimationIStruct {
    Animation a;
    int* v;
    int n;
    int* start;
    int* end;
} LinearAnimationI;

void update_linear_animationi(Animation* a, float f) {
    LinearAnimationI* la = (LinearAnimationI*)a;

    int i;
    for (i=0; i<la->n; i++)
        la->v[i] = la->start[i] + (la->end[i] - la->start[i]) * f;
}

void free_linear_animationi(Animation* a) {
    LinearAnimationI* la = (LinearAnimationI*)a;
    free(la->start);
    free(la->end);
    default_free_animation(a);
}

Animation* lineari(int* v, int n, int* start, int* end) {
    LinearAnimationI* a = malloc(sizeof(LinearAnimationI));
    a->a.update   = update_linear_animationi;
    a->a.duration = default_animation_duration;
    a->a.free     = free_linear_animationi;
    a->v     = v;
    a->n     = n;
    a->start = start;
    a->end   = end;
    return (Animation*)a;
}

Animation* lineari1(int* v, int start, int end) {
    int* s = malloc(sizeof(int));
    int* e = malloc(sizeof(int));

    *s = start;
    *e = end;

    return lineari(v, 1, s, e);
}

/* bezier animation (float) */

typedef struct BezierAnimationFStruct {
    Animation a;
    float* v;
    int n;
    int m;
    float** control_points;
    float** working_storage;
} BezierAnimationF;

void update_bezier_animationf(Animation* a, float f) {
    BezierAnimationF* s = (BezierAnimationF*)a;

    int i;
    for (i=0; i<s->m; i++)
        memcpy(s->working_storage[i], s->control_points[i], sizeof(float)*s->n);

    int j, k;
    for (i=s->m-1; i>=1; i--)
        for (j=0; j<i; j++)
            for (k=0; k<s->n; k++)
                s->working_storage[j][k] = s->working_storage[j][k] + (s->working_storage[j+1][k] - s->working_storage[j][k]) * f;

    memcpy(s->v, s->working_storage[0], sizeof(float)*s->n);
}

void free_bezier_animationf(Animation* a) {
    BezierAnimationF* s = (BezierAnimationF*)a;

    int i;
    for (i=0; i<s->m; i++) {
        free(s->control_points[i]);
        free(s->working_storage[i]);
    }

    free(s->control_points);
    free(s->working_storage);

    default_free_animation(a);
}

Animation* bezierf(float* v, int n, int m, float** control_points) {
    int i;

    BezierAnimationF* a = malloc(sizeof(BezierAnimationF));
    a->a.update   = update_bezier_animationf;
    a->a.duration = default_animation_duration;
    a->a.free     = free_bezier_animationf;
    a->v               = v;
    a->n               = n;
    a->m               = m;
    a->control_points  = control_points;
    a->working_storage = malloc(sizeof(float*) * m);
    for (i=0; i<m; i++)
        a->working_storage[i] = malloc(sizeof(float) * n);

    return (Animation*)a;
}

/* scaled animation */

typedef struct ScaledAnimationStruct {
    Animation a;
    Animation* child;
    float scale_factor;
} ScaledAnimation;

void update_scaled_animation(Animation* a, float f) {
    ScaledAnimation* sa = (ScaledAnimation*)a;
    update_animation(sa->child, f / sa->scale_factor);
}

void free_scaled_animation(Animation* a) {
    ScaledAnimation* sa = (ScaledAnimation*)a;
    free_animation(sa->child);
    default_free_animation(a);
}

float scaled_animation_duration(Animation* a) {
    ScaledAnimation* sa = (ScaledAnimation*)a;
    return animation_duration(sa->child) * sa->scale_factor;
}

Animation* scale(Animation* a, float scale_factor) {
    ScaledAnimation* s = malloc(sizeof(ScaledAnimation));
    s->a.update    = update_scaled_animation;
    s->a.duration  = scaled_animation_duration;
    s->a.free      = free_scaled_animation;
    s->child        = a;
    s->scale_factor = scale_factor;
    return (Animation*)s;
}

/* transformed animation */

typedef struct TransformedAnimationStruct {
    Animation a;
    Animation* child;
    TimeTransform* t;
} TransformedAnimation;

void update_transformed_animation(Animation* a, float f) {
    TransformedAnimation* ta = (TransformedAnimation*)a;

    float d = animation_duration(ta->child);
    update_animation(ta->child, apply_transform(ta->t, f / d) * d);
}

void free_transformed_animation(Animation* a) {
    TransformedAnimation* ta = (TransformedAnimation*)a;
    free_animation(ta->child);
    free(ta->t);
    default_free_animation(a);
}

float transformed_animation_duration(Animation* a) {
    TransformedAnimation* ta = (TransformedAnimation*)a;
    return animation_duration(ta->child);
}

Animation* transform(Animation* a, TimeTransform* t) {
    TransformedAnimation* ta = malloc(sizeof(TransformedAnimation));
    ta->a.update   = update_transformed_animation;
    ta->a.duration = transformed_animation_duration;
    ta->a.free     = free_transformed_animation;
    ta->child = a;
    ta->t     = t;
    return (Animation*)ta;
}

/* sequence animation */

typedef struct SequenceAnimationStruct {
    Animation a;
    Animation* a1;
    Animation* a2;
} SequenceAnimation;

void update_sequence_animation(Animation* a, float f) {
    SequenceAnimation* as = (SequenceAnimation*)a;

    float d = animation_duration(as->a1);

    if (f <= d)
        update_animation(as->a1, f);
    else
        update_animation(as->a2, f-d);
}

void free_sequence_animation(Animation* a) {
    SequenceAnimation* as = (SequenceAnimation*)a;
    free_animation(as->a1);
    free_animation(as->a2);
    default_free_animation(a);
}

float sequence_animation_duration(Animation* a) {
    SequenceAnimation* as = (SequenceAnimation*)a;
    return animation_duration(as->a1) + animation_duration(as->a2);
}

Animation* sequence(Animation* a1, Animation* a2) {
    SequenceAnimation* a = malloc(sizeof(SequenceAnimation));
    a->a.update   = update_sequence_animation;
    a->a.duration = sequence_animation_duration;
    a->a.free     = free_sequence_animation;
    a->a1 = a1;
    a->a2 = a2;
    return (Animation*)a;
}

/* parallel animation */

typedef struct ParallelAnimationStruct {
    Animation a;
    Animation* a1;
    Animation* a2;
} ParallelAnimation;

void update_parallel_animation(Animation* a, float f) {
    ParallelAnimation* as = (ParallelAnimation*)a;
    update_animation(as->a1, f);
    update_animation(as->a2, f);
}

void free_parallel_animation(Animation* a) {
    ParallelAnimation* as = (ParallelAnimation*)a;
    free_animation(as->a1);
    free_animation(as->a2);
    default_free_animation(a);
}

float parallel_animation_duration(Animation* a) {
    ParallelAnimation* as = (ParallelAnimation*)a;
    return animation_duration(as->a1);
}

Animation* parallel(Animation* a1, Animation* a2) {
    ParallelAnimation* a = malloc(sizeof(ParallelAnimation));
    a->a.update   = update_parallel_animation;
    a->a.duration = parallel_animation_duration;
    a->a.free     = free_parallel_animation;
    a->a1 = a1;
    a->a2 = a2;
    return (Animation*)a;
}

/* higher-level operations */

Animation* delay(Animation* a, float d) {
    return sequence(scale(null_animation(), d), a);
}

Animation* sinusoid(Animation* a) {
    return transform(a, sinusoid_transform());
}

Animation* reverse(Animation* a) {
    return transform(a, reverse_transform());
}