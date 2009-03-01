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

Animation* null_animation();
Animation* linear_animationf(float* v, int n, float* start, float* end);
Animation* linear_animationi(float* v, int n, float* start, float* end);

Animation* scale(Animation* a, float scale_factor);
Animation* transform(Animation* a, TimeTransform* t);

Animation* sequence(Animation* a1, Animation* a2);
Animation* parallel(Animation* a1, Animation* a2);

/* higher-level operations */

Animation* delay(Animation* a, float d);

#endif
