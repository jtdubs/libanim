#include "anim.h"

#include <glib.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* utilities */

void assert_rangef(float x, float bottom, float top) {
    g_assert_cmpfloat(x, >=, bottom);
    g_assert_cmpfloat(x, <=, top);
}

void assert_rangei(int x, int bottom, int top) {
    g_assert_cmpint(x, >=, bottom);
    g_assert_cmpint(x, <=, top);
}

/* animation runners */

struct AnimationRunnerStruct {
    Animation* animation;
    float duration;
    GTimeVal start_time;
};

AnimationRunner* animation_runner(Animation* a) {
    AnimationRunner* r = malloc(sizeof(AnimationRunner));
    r->animation          = a;
    r->duration           = animation_duration(a);
    r->start_time.tv_sec  = 0;
    r->start_time.tv_usec = 0;
    return r;
}

void animation_runner_start(AnimationRunner* r) {
    g_get_current_time(&r->start_time);
}

gboolean animation_runner_update(AnimationRunner* r) {
    GTimeVal now;
    g_get_current_time(&now);

    float t = ((float)(now.tv_sec - r->start_time.tv_sec)) + (((float)(now.tv_usec - r->start_time.tv_usec)) / (float)G_USEC_PER_SEC);

    if (t < r->duration) {
        animation_update(r->animation, t);
        return TRUE;
    } else {
        animation_update(r->animation, r->duration);
        return FALSE;
    }
}

void animation_runner_free(AnimationRunner* r) {
    animation_free(r->animation);
    free(r);
}

/* time transformations - modify the relationship between external time and animation time by transforming numbers in the range [0,1] */

typedef float (*TimeTransformFunction)(TimeTransform*, float);

struct TimeTransformStruct {
    TimeTransformFunction f;
};

float apply_transform(TimeTransform* t, float f) {
    g_assert(t != NULL);
    assert_rangef(f, 0.0, 1.0);
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
    return (1.0 + sin((f * G_PI) - G_PI_2)) / 2.0;
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

/* exponent transform */

typedef struct exponentonentTransformStruct {
    TimeTransform t;
    float exponent;
} exponentonentTransform;

float exponent_transform_function(TimeTransform* t, float f) {
    exponentonentTransform* e = (exponentonentTransform*)t;
    return pow(f, e->exponent);
}

TimeTransform* exponent_transform(float exponent) {
    exponentonentTransform* t = malloc(sizeof(exponentonentTransform));
    t->t.f = exponent_transform_function;
    t->exponent = exponent;
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

void animation_update(Animation* a, float f) {
    g_assert(a != NULL);
    assert_rangef(f, 0.0, animation_duration(a));

    a->update(a, f);
}

float animation_duration(Animation* a) {
    g_assert(a != NULL);
    return a->duration(a);
}

void animation_free(Animation* a) {
    g_assert(a != NULL);
    a->free(a);
}

float default_animation_duration(Animation* a) {
    return 1.0;
}

void default_animation_free(Animation* a) {
    free(a);
}

/* null animation */

typedef struct NullAnimationStruct {
    Animation a;
} NullAnimation;

void null_animation_update(Animation* a, float f) {
}

Animation* null_animation() {
    NullAnimation* a = malloc(sizeof(NullAnimation));
    a->a.update   = null_animation_update;
    a->a.duration = default_animation_duration;
    a->a.free     = default_animation_free;
    return (Animation*)a;
}

/* hold animation */

Animation* holdf(float* v, int n, float* c) {
    return linearf(v, 1, c, c);
}

Animation* holdi(int* v, int n, int* c) {
    return lineari(v, 1, c, c);
}

Animation* holdf1(float* v, float c) {
    return linearf1(v, c, c);
}

Animation* holdi1(int* v, int c) {
    return lineari1(v, c, c);
}

/* linear animation (float) */

typedef struct LinearAnimationFStruct {
    Animation a;
    float* v;
    int n;
    float* start;
    float* end;
} LinearAnimationF;

void linear_animationf_update(Animation* a, float f) {
    LinearAnimationF* la = (LinearAnimationF*)a;

    int i;
    for (i=0; i<la->n; i++)
        la->v[i] = la->start[i] + (la->end[i] - la->start[i]) * f;
}

void linear_animationf_free(Animation* a) {
    LinearAnimationF* la = (LinearAnimationF*)a;
    free(la->start);
    free(la->end);
    default_animation_free(a);
}

Animation* linearf(float* v, int n, float* start, float* end) {
    g_assert(v != NULL);
    g_assert(start != NULL);
    g_assert(end != NULL);
    g_assert_cmpint(n, >, 0);

    LinearAnimationF* a = malloc(sizeof(LinearAnimationF));
    a->a.update   = linear_animationf_update;
    a->a.duration = default_animation_duration;
    a->a.free     = linear_animationf_free;
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

void linear_animationi_update(Animation* a, float f) {
    LinearAnimationI* la = (LinearAnimationI*)a;

    int i;
    for (i=0; i<la->n; i++)
        la->v[i] = la->start[i] + (la->end[i] - la->start[i]) * f;
}

void linear_animationi_free(Animation* a) {
    LinearAnimationI* la = (LinearAnimationI*)a;
    free(la->start);
    free(la->end);
    default_animation_free(a);
}

Animation* lineari(int* v, int n, int* start, int* end) {
    g_assert(v != NULL);
    g_assert(start != NULL);
    g_assert(end != NULL);
    g_assert_cmpint(n, >, 0);

    LinearAnimationI* a = malloc(sizeof(LinearAnimationI));
    a->a.update   = linear_animationi_update;
    a->a.duration = default_animation_duration;
    a->a.free     = linear_animationi_free;
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

void bezier_animationf_update(Animation* a, float f) {
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

void bezier_animationf_free(Animation* a) {
    BezierAnimationF* s = (BezierAnimationF*)a;

    int i;
    for (i=0; i<s->m; i++) {
        free(s->control_points[i]);
        free(s->working_storage[i]);
    }

    free(s->control_points);
    free(s->working_storage);

    default_animation_free(a);
}

Animation* bezierf(float* v, int n, int m, float** control_points) {
    g_assert(v != NULL);
    g_assert(control_points != NULL);
    g_assert_cmpint(n, >, 0);
    g_assert_cmpint(m, >, 0);

    int i;

    BezierAnimationF* a = malloc(sizeof(BezierAnimationF));
    a->a.update         = bezier_animationf_update;
    a->a.duration       = default_animation_duration;
    a->a.free           = bezier_animationf_free;
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

void scaled_animation_update(Animation* a, float f) {
    ScaledAnimation* sa = (ScaledAnimation*)a;
    animation_update(sa->child, f / sa->scale_factor);
}

void scaled_animation_free(Animation* a) {
    ScaledAnimation* sa = (ScaledAnimation*)a;
    animation_free(sa->child);
    default_animation_free(a);
}

float scaled_animation_duration(Animation* a) {
    ScaledAnimation* sa = (ScaledAnimation*)a;
    return animation_duration(sa->child) * sa->scale_factor;
}

Animation* scale(Animation* a, float scale_factor) {
    g_assert(a != NULL);
    g_assert_cmpfloat(scale_factor, !=, 0);

    ScaledAnimation* s = malloc(sizeof(ScaledAnimation));
    s->a.update         = scaled_animation_update;
    s->a.duration       = scaled_animation_duration;
    s->a.free           = scaled_animation_free;
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

void transformed_animation_update(Animation* a, float f) {
    TransformedAnimation* ta = (TransformedAnimation*)a;

    float d = animation_duration(ta->child);
    animation_update(ta->child, apply_transform(ta->t, f / d) * d);
}

void transformed_animation_free(Animation* a) {
    TransformedAnimation* ta = (TransformedAnimation*)a;
    animation_free(ta->child);
    free(ta->t);
    default_animation_free(a);
}

float transformed_animation_duration(Animation* a) {
    TransformedAnimation* ta = (TransformedAnimation*)a;
    return animation_duration(ta->child);
}

Animation* transform(Animation* a, TimeTransform* t) {
    g_assert(a != NULL);
    g_assert(t != NULL);

    TransformedAnimation* ta = malloc(sizeof(TransformedAnimation));
    ta->a.update   = transformed_animation_update;
    ta->a.duration = transformed_animation_duration;
    ta->a.free     = transformed_animation_free;
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

void sequence_animation_update(Animation* a, float f) {
    SequenceAnimation* as = (SequenceAnimation*)a;

    float d = animation_duration(as->a1);

    if (f <= d)
        animation_update(as->a1, f);
    else
        animation_update(as->a2, f-d);
}

void sequence_animation_free(Animation* a) {
    SequenceAnimation* as = (SequenceAnimation*)a;
    animation_free(as->a1);
    animation_free(as->a2);
    default_animation_free(a);
}

float sequence_animation_duration(Animation* a) {
    SequenceAnimation* as = (SequenceAnimation*)a;
    return animation_duration(as->a1) + animation_duration(as->a2);
}

Animation* sequence(Animation* a1, Animation* a2) {
    g_assert(a1 != NULL);
    g_assert(a2 != NULL);

    SequenceAnimation* a = malloc(sizeof(SequenceAnimation));
    a->a.update   = sequence_animation_update;
    a->a.duration = sequence_animation_duration;
    a->a.free     = sequence_animation_free;
    a->a1 = a1;
    a->a2 = a2;
    return (Animation*)a;
}

Animation* sequencen(Animation *a1, ...) {
    Animation *result = a1;

    va_list args;
    va_start(args, a1);
    while (TRUE) {
        Animation* a = va_arg(args, Animation*);
        if (a == NULL)
            break;
        result = sequence(result, a);
    }

    return result;
}

/* parallel animation */

typedef struct ParallelAnimationStruct {
    Animation a;
    Animation* a1;
    Animation* a2;
} ParallelAnimation;

void parallel_animation_update(Animation* a, float f) {
    ParallelAnimation* as = (ParallelAnimation*)a;
    animation_update(as->a1, f);
    animation_update(as->a2, f);
}

void parallel_animation_free(Animation* a) {
    ParallelAnimation* as = (ParallelAnimation*)a;
    animation_free(as->a1);
    animation_free(as->a2);
    default_animation_free(a);
}

float parallel_animation_duration(Animation* a) {
    ParallelAnimation* as = (ParallelAnimation*)a;
    return animation_duration(as->a1);
}

Animation* parallel(Animation* a1, Animation* a2) {
    g_assert(a1 != NULL);
    g_assert(a2 != NULL);
    g_assert_cmpfloat(animation_duration(a1), ==, animation_duration(a2));

    ParallelAnimation* a = malloc(sizeof(ParallelAnimation));
    a->a.update   = parallel_animation_update;
    a->a.duration = parallel_animation_duration;
    a->a.free     = parallel_animation_free;
    a->a1 = a1;
    a->a2 = a2;
    return (Animation*)a;
}

Animation* paralleln(Animation *a1, ...) {
    Animation *result = a1;

    va_list args;
    va_start(args, a1);
    while (TRUE) {
        Animation* a = va_arg(args, Animation*);
        if (a == NULL)
            break;
        result = parallel(result, a);
    }

    return result;
}

Animation* parallelp(Animation *a1, Animation* a2) {
    float d1 = animation_duration(a1), d2 = animation_duration(a2);

    if (d1 < d2)
        return parallel(pad_to(a1, d2), a2);

    if (d1 > d2)
        return parallel(a1, pad_to(a2, d1));

    return parallel(a1, a2);
}

Animation* parallelpn(Animation *a1, ...) {
    Animation *result = a1;

    va_list args;
    va_start(args, a1);
    while (TRUE) {
        Animation* a = va_arg(args, Animation*);
        if (a == NULL)
            break;
        result = parallelp(result, a);
    }

    return result;
}

/* higher-level operations */

Animation* delay(Animation* a, float d) {
    g_assert(a != NULL);
    return sequence(scale(null_animation(), d), a);
}

Animation* pad_by(Animation* a, float d) {
    return sequence(a, scale(null_animation(), d));
}

Animation* pad_to(Animation* a, float d) {
    return pad_by(a, d - animation_duration(a));
}

Animation* identity(Animation* a) {
    g_assert(a != NULL);
    return transform(a, identity_transform());
}

Animation* sinusoid(Animation* a) {
    g_assert(a != NULL);
    return transform(a, sinusoid_transform());
}

Animation* reverse(Animation* a) {
    g_assert(a != NULL);
    return transform(a, reverse_transform());
}

Animation* exponent(Animation* a, float f) {
    g_assert(a != NULL);
    g_assert_cmpfloat(f, !=, 0.0);
    return transform(a, exponent_transform(f));
}

/* derived values */

typedef void (*UpdateDerivedValueFunction)(DerivedValue*);
typedef void (*FreeDerivedValueFunction)(DerivedValue*);

struct DerivedValueStruct {
    UpdateDerivedValueFunction update;
    FreeDerivedValueFunction free;
};

void derived_value_update(DerivedValue* dv) {
    dv->update(dv);
}

void derived_value_free(DerivedValue* dv) {
    dv->free(dv);
}

void default_derived_value_free(DerivedValue* dv) {
    free(dv);
}

/* concrete derived values */

typedef struct ConcreteDerivedValueStruct {
    DerivedValue dv;
    void* transform;
    int n;
    void* in;
    void* out;
} ConcreteDerivedValue;

void concrete_derived_value_update(DerivedValue* dv) {
    ConcreteDerivedValue *cdv = (ConcreteDerivedValue*)dv;
    ((TransformN)cdv->transform)(cdv->n, cdv->in, cdv->out);
}

void concrete_derived_value_update_ff(DerivedValue* dv) {
    ConcreteDerivedValue *cdv = (ConcreteDerivedValue*)dv;
    *((float*)cdv->out) = ((TransformFF)cdv->transform)(*((float*)cdv->in));
}

void concrete_derived_value_update_fi(DerivedValue* dv) {
    ConcreteDerivedValue *cdv = (ConcreteDerivedValue*)dv;
    *((int*)cdv->out) = ((TransformFI)cdv->transform)(*((float*)cdv->in));
}

void concrete_derived_value_update_if(DerivedValue* dv) {
    ConcreteDerivedValue *cdv = (ConcreteDerivedValue*)dv;
    *((float*)cdv->out) = ((TransformIF)cdv->transform)(*((int*)cdv->in));
}

void concrete_derived_value_update_ii(DerivedValue* dv) {
    ConcreteDerivedValue *cdv = (ConcreteDerivedValue*)dv;
    *((int*)cdv->out) = ((TransformII)cdv->transform)(*((int*)cdv->in));
}

void concrete_derived_value_update_ffn(DerivedValue* dv) {
    int i;
    ConcreteDerivedValue *cdv = (ConcreteDerivedValue*)dv;
    for (i = 0; i < cdv->n; i++)
        ((float*)cdv->out)[i] = ((TransformFF)cdv->transform)(((float*)cdv->in)[i]);
}

void concrete_derived_value_update_fin(DerivedValue* dv) {
    int i;
    ConcreteDerivedValue *cdv = (ConcreteDerivedValue*)dv;
    for (i = 0; i < cdv->n; i++)
        ((int*)cdv->out)[i] = ((TransformFI)cdv->transform)(((float*)cdv->in)[i]);
}

void concrete_derived_value_update_ifn(DerivedValue* dv) {
    int i;
    ConcreteDerivedValue *cdv = (ConcreteDerivedValue*)dv;
    for (i = 0; i < cdv->n; i++)
        ((float*)cdv->out)[i] = ((TransformIF)cdv->transform)(((int*)cdv->in)[i]);
}

void concrete_derived_value_update_iin(DerivedValue* dv) {
    int i;
    ConcreteDerivedValue *cdv = (ConcreteDerivedValue*)dv;
    for (i = 0; i < cdv->n; i++)
        ((int*)cdv->out)[i] = ((TransformII)cdv->transform)(((int*)cdv->in)[i]);
}

DerivedValue* mk_cdv(UpdateDerivedValueFunction update, void* f, int n, void* in, void* out) {
    ConcreteDerivedValue *cdv = (ConcreteDerivedValue*)malloc(sizeof(ConcreteDerivedValue));
    cdv->dv.update = update;
    cdv->dv.free   = default_derived_value_free;
    cdv->transform = f;
    cdv->n         = n;
    cdv->in        = in;
    cdv->out       = out;
    return (DerivedValue*)cdv;
}

DerivedValue* derive(TransformN f, int n, void* in, void* out) {
    return mk_cdv(concrete_derived_value_update, f, n, in, out);
}

DerivedValue* deriveff(TransformFF f, float* in, float* out) {
    return mk_cdv(concrete_derived_value_update_ff, f, 1, in, out);
}

DerivedValue* derivefi(TransformFI f, float* in, int* out) {
    return mk_cdv(concrete_derived_value_update_fi, f, 1, in, out);
}

DerivedValue* deriveif(TransformIF f, int* in, float* out) {
    return mk_cdv(concrete_derived_value_update_if, f, 1, in, out);
}

DerivedValue* deriveii(TransformII f, int* in, int* out) {
    return mk_cdv(concrete_derived_value_update_ii, f, 1, in, out);
}

DerivedValue* mapderiveff(TransformFF f, int n, float* in, float* out) {
    return mk_cdv(concrete_derived_value_update_ffn, f, n, in, out);
}

DerivedValue* mapderivefi(TransformFI f, int n, float* in, int* out) {
    return mk_cdv(concrete_derived_value_update_fin, f, n, in, out);
}

DerivedValue* mapderiveif(TransformIF f, int n, int* in, float* out) {
    return mk_cdv(concrete_derived_value_update_ifn, f, n, in, out);
}

DerivedValue* mapderiveii(TransformII f, int n, int* in, int* out) {
    return mk_cdv(concrete_derived_value_update_iin, f, n, in, out);
}

/* derived value animation */

typedef struct DerivedAnimationStruct {
    Animation a;
    Animation* child;
    DerivedValue* dv;
} DerivedAnimation;

void derived_animation_update(Animation* a, float f) {
    DerivedAnimation* da = (DerivedAnimation*)a;
    animation_update(da->child, f);
    derived_value_update(da->dv);
}

void derived_animation_free(Animation* a) {
    DerivedAnimation* da = (DerivedAnimation*)a;
    derived_value_free(da->dv);
    animation_free(da->child);
}

float derived_animation_duration(Animation* a) {
    DerivedAnimation* da = (DerivedAnimation*)a;
    return animation_duration(da->child);
}

Animation* attach(Animation* a, DerivedValue *dv) {
    g_assert(a != NULL);
    g_assert(dv != NULL);

    DerivedAnimation* da = malloc(sizeof(DerivedAnimation));
    da->a.update   = derived_animation_update;
    da->a.duration = derived_animation_duration;
    da->a.free     = derived_animation_free;
    da->child = a;
    da->dv    = dv;
    return (Animation*)da;
}

Animation* attachn(Animation *a, DerivedValue *dv1, ...) {
    Animation *result = attach(a, dv1);

    va_list args;
    va_start(args, dv1);
    while (TRUE) {
        DerivedValue* dv = va_arg(args, DerivedValue*);
        if (dv == NULL)
            break;
        result = attach(result, dv);
    }

    return result;
}
