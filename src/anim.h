#ifndef __ANIM_H__
#define __ANIM_H__

#include <glib.h>

/* Animations
 *
 * Animations change values over time.  All primitive animations take 1 unit of time.
 */

struct AnimationStruct;
typedef struct AnimationStruct Animation;

void  animation_update(Animation* a, float time);
float animation_duration(Animation* a);
void  animation_free(Animation* a);

Animation* null_animation(); /* the null animation does nothing */

Animation* holdf(float* v, int n, float* c); /* hold v at the constant n-dimensional value c */
Animation* holdi(int*   v, int n, int*   c); /* hold v at the constant n-dimensional value c */

Animation* holdf1(float* v, float c); /* hold v at the constant value c */
Animation* holdi1(int*   v, int   c); /* hold v at the constant value c */

Animation* linearf(float* v, int n, float* start, float* end); /* animate n-dimensional point v from start to end */
Animation* lineari(int*   v, int n, int*   start, int*   end); /* animate n-dimensional point v from start to end */

Animation* linearf1(float* v, float start, float end); /* animate value v from start to end */
Animation* lineari1(int*   v, int   start, int   end); /* animate value v from start to end */

Animation* bezierf(float* v, int n, int m, float** control_points); /* animate n-dimensional point v along bezier with m control points */




/* Time Transformations
 *
 * Time Transformations modify the internal rate of time.
 * The transformed animation's duration is unchanged, but time may be slower in parts and faster in others. 
 */

struct TimeTransformStruct;
typedef struct TimeTransformStruct TimeTransform;

TimeTransform* identity_transform();       /* the identity transformation does not affect the speed of time */
TimeTransform* sinusoid_transform();       /* time varies as the (sin(x)+1)/2 for x in [-PI/2, PI/2], leading to animations that speed up then slow down */
TimeTransform* reverse_transform();        /* reverses time within an animation */
TimeTransform* exponent_transform(float);  /* time varies as x^n leading to animations that accelerate (for n>1) or decellerate (for n<1) */


/* Primitive Modifiers
 *
 * These are the basic animation combinators upon which all others are built.
 */

Animation* scale(Animation* a, float scale_factor);   /* scales the duration of an animation by scale_factor */
Animation* transform(Animation* a, TimeTransform* t); /* applies an internal time transformation to an animation */

Animation* sequence(Animation* a1, Animation* a2); /* perform two animations in sequence */
Animation* parallel(Animation* a1, Animation* a2); /* perform two animations in parallel */

Animation* sequencen(Animation *a1, ...); /* sequence a null-terminated list of animations */
Animation* paralleln(Animation *a1, ...); /* parallel a null-terminated list of animations */

Animation* parallelp(Animation *a1, Animation* a2); /* parallel two animations, padding the shorter */
Animation* parallelpn(Animation *a1, ...);          /* parallel a null-terminated list of animations, padding the shorter ones */


/* High-Level Modifiers
 *
 * These modifiers are built upon the primitive modifiers.
 */

Animation* delay(Animation* a, float d);  /* delay the start of an animation by d units of time */
Animation* pad_by(Animation* a, float d); /* pad the end of an animation by d units of time */
Animation* pad_to(Animation* a, float d); /* pad the end of an animation to a total of d units of time */

Animation* identity(Animation *a);          /* apply the identity transformation to an animation */
Animation* sinusoid(Animation *a);          /* apply the sinusoid transformation to an animation */
Animation* reverse(Animation *a);           /* apply the reverse transformation to an animation */
Animation* exponent(Animation* a, float f); /* apply the exponent transformation to an animation */


/* Animation Runner
 *
 * Animation Runners keep track of the start time of an animation and keep it up to date.
 */

struct AnimationRunnerStruct;
typedef struct AnimationRunnerStruct AnimationRunner;

AnimationRunner* animation_runner(Animation*);              /* create a runner for an animation */
void             animation_runner_start(AnimationRunner*);  /* start the animation at the current time */
gboolean         animation_runner_update(AnimationRunner*); /* update based on the current time.  returns TRUE if more animation remains. */
void             animation_runner_free(AnimationRunner*);   /* free the runner and its animation */


/* Derived Values
 *
 * Derived values are attached to animations and are automatically updated as the animation progresses.
 */

struct DerivedValueStruct;
typedef struct DerivedValueStruct DerivedValue;

typedef void  (*TransformN)(int n, void* in, void* out);
typedef float (*TransformFF)(float);
typedef int   (*TransformFI)(float);
typedef float (*TransformIF)(int);
typedef int   (*TransformII)(int);

DerivedValue* derive(TransformN f, int n, void* in, void* out);
DerivedValue* deriveff(TransformFF f, float* in, float* out);
DerivedValue* derivefi(TransformFI f, float* in, int*   out);
DerivedValue* deriveif(TransformIF f, int*   in, float* out);
DerivedValue* deriveii(TransformII f, int*   in, int*   out);
DerivedValue* deriveffn(TransformFF f, int n, float* in, float* out);
DerivedValue* derivefin(TransformFI f, int n, float* in, int* out);
DerivedValue* deriveifn(TransformIF f, int n, int* in,   float* out);
DerivedValue* deriveiin(TransformII f, int n, int* in,   int* out);

Animation* attach(Animation*, DerivedValue*);       /* attach a derived value to an animation */
Animation* attachn(Animation*, DerivedValue*, ...); /* attach a null-terminated list of derived values to an animation */

#endif
