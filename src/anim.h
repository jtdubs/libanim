#ifndef __ANIM_H__
#define __ANIM_H__

/* Animations
 *
 * Animations change values over time.  All prmitive animations take 1 unit of time.
 */

struct AnimationStruct;
typedef struct AnimationStruct Animation;

void  update_animation(Animation* a, float time);
float animation_duration(Animation* a);
void  free_animation(Animation* a);

Animation* null_animation(); /* the null animation does nothing */

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
TimeTransform* sinusoid_transform();       /* time varies as the sin(x) for x in [0, PI/2], leading to animations that speed up then slow down */
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


/* High-Level Modifiers
 *
 * These modifiers are built upon the primitive modifiers.
 */

Animation* delay(Animation* a, float d); /* delay the start of an animation by d units of time */

Animation* identity(Animation *a);          /* apply the identity transformation to an animation */
Animation* sinusoid(Animation *a);          /* apply the sinusoid transformation to an animation */
Animation* reverse(Animation *a);           /* apply the reverse transformation to an animation */
Animation* exponent(Animation* a, float f); /* apply the exponent transformation to an animation */

#endif
