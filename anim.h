#ifndef __ANIM_H__
#define __ANIM_H__

/* time transformations */

struct TimeTransformStruct;
typedef struct TimeTransformStruct TimeTransform;

float apply_transform(TimeTransform* t, float f);

TimeTransform* identity_transform();
TimeTransform* sinusoid_transform();
TimeTransform* reverse_transform();
TimeTransform* exp_transform(float exp);


/* animations */

struct AnimationStruct;
typedef struct AnimationStruct Animation;

void  update_animation(Animation* a, float f);
float animation_duration(Animation* a);
void  free_animation(Animation* a);

Animation* null_animation();

Animation* linearf(float* v, int n, float* start, float* end);
Animation* lineari(int*   v, int n, int*   start, int*   end);

Animation* linearf1(float* v, float start, float end);
Animation* lineari1(int*   v, int   start, int   end);

Animation* bezierf(float* v, int n, int m, float** control_points);


/* primitive modifiers */

Animation* scale(Animation* a, float scale_factor);
Animation* transform(Animation* a, TimeTransform* t);

Animation* sequence(Animation* a1, Animation* a2);
Animation* parallel(Animation* a1, Animation* a2);


/* high-level modifiers */

Animation* delay(Animation* a, float d);
Animation* sinusoid(Animation *a);
Animation* reverse(Animation *a);

#endif
