#include "anim.h" 

#include <glib.h>
#include <math.h>

void assert_float_equal(float f1, float f2) {
	g_assert_cmpfloat(fabs(f1-f2), <, 0.00001);
}

void test_null() {
    Animation* a = null_animation();
    update_animation(a, 0.0);
    update_animation(a, 1.0);
    g_assert_cmpfloat(animation_duration(a), ==, 1.0);
}

void test_linearf_one_dimension_unit() {
    float f, start = 0.0, end = 1.0;
    Animation* a = linear_animationf(&f, 1, &start, &end);
    g_assert(animation_duration(a) == 1.0);

    update_animation(a, 0.0); assert_float_equal(f, 0.0);
    update_animation(a, 0.5); assert_float_equal(f, 0.5);
    update_animation(a, 1.0); assert_float_equal(f, 1.0);
}

void test_linearf_one_dimension_range() {
    float f, start = 10.0, end = 20.0;
    Animation* a = linear_animationf(&f, 1, &start, &end);
    g_assert(animation_duration(a) == 1.0);

    update_animation(a, 0.0); assert_float_equal(f, 10.0);
    update_animation(a, 0.5); assert_float_equal(f, 15.0);
    update_animation(a, 1.0); assert_float_equal(f, 20.0);
}

void test_linearf_three_dimensions_unit() {
    float f[3], start[3] = { 0.0, 0.0, 0.0 }, end[3] = { 1.0, 1.0, 1.0 };
    Animation* a = linear_animationf(f, 3, start, end);
    g_assert(animation_duration(a) == 1.0);

    update_animation(a, 0.0); assert_float_equal(f[0], 0.0); assert_float_equal(f[1], 0.0); assert_float_equal(f[2], 0.0);
    update_animation(a, 0.5); assert_float_equal(f[0], 0.5); assert_float_equal(f[1], 0.5); assert_float_equal(f[2], 0.5);
    update_animation(a, 1.0); assert_float_equal(f[0], 1.0); assert_float_equal(f[1], 1.0); assert_float_equal(f[2], 1.0);
}

void test_linearf_three_dimensions_range() {
    float f[3], start[3] = { 10.0, 11.0, 12.0 }, end[3] = { 20.0, 21.0, 22.0 };
    Animation* a = linear_animationf(f, 3, start, end);
    g_assert(animation_duration(a) == 1.0);

    update_animation(a, 0.0); assert_float_equal(f[0], 10.0); assert_float_equal(f[1], 11.0); assert_float_equal(f[2], 12.0);
    update_animation(a, 0.5); assert_float_equal(f[0], 15.0); assert_float_equal(f[1], 16.0); assert_float_equal(f[2], 17.0);
    update_animation(a, 1.0); assert_float_equal(f[0], 20.0); assert_float_equal(f[1], 21.0); assert_float_equal(f[2], 22.0);
}

void test_sequence() {
    float f, start1 = 0.0, end1 = 1.0, start2 = 1.0, end2 = 0.0;
    Animation* a = sequence(linear_animationf(&f, 1, &start1, &end1), linear_animationf(&f, 1, &start2, &end2));
    g_assert(animation_duration(a) == 2.0);

    update_animation(a, 0.0); assert_float_equal(f, 0.0);
    update_animation(a, 0.5); assert_float_equal(f, 0.5);
    update_animation(a, 1.0); assert_float_equal(f, 1.0);
    update_animation(a, 1.5); assert_float_equal(f, 0.5);
    update_animation(a, 2.0); assert_float_equal(f, 0.0);
}

void test_parallel() {
    float f1, f2, start1 = 0.0, end1 = 1.0, start2 = 1.0, end2 = 0.0;
    Animation* a = parallel(linear_animationf(&f1, 1, &start1, &end1), linear_animationf(&f2, 1, &start2, &end2));
    g_assert(animation_duration(a) == 1.0);

    update_animation(a, 0.0); assert_float_equal(f1, 0.0); assert_float_equal(f2, 1.0);
    update_animation(a, 0.5); assert_float_equal(f1, 0.5); assert_float_equal(f2, 0.5);
    update_animation(a, 1.0); assert_float_equal(f1, 1.0); assert_float_equal(f2, 0.0);
}

void test_scale_up() {
    float f, start = 0.0, end = 1.0;
    Animation* a = scale(linear_animationf(&f, 1, &start, &end), 5.0);
    g_assert(animation_duration(a) == 5.0);

    update_animation(a, 0.0); assert_float_equal(f, 0.0);
    update_animation(a, 1.0); assert_float_equal(f, 0.2);
    update_animation(a, 2.0); assert_float_equal(f, 0.4);
    update_animation(a, 3.0); assert_float_equal(f, 0.6);
    update_animation(a, 4.0); assert_float_equal(f, 0.8);
    update_animation(a, 5.0); assert_float_equal(f, 1.0);
}

void test_scale_down() {
    float f, start = 0.0, end = 1.0;
    Animation* a = scale(linear_animationf(&f, 1, &start, &end), 0.5);
    g_assert(animation_duration(a) == 0.5);

    update_animation(a, 0.0); assert_float_equal(f, 0.0);
    update_animation(a, 0.1); assert_float_equal(f, 0.2);
    update_animation(a, 0.2); assert_float_equal(f, 0.4);
    update_animation(a, 0.3); assert_float_equal(f, 0.6);
    update_animation(a, 0.4); assert_float_equal(f, 0.8);
    update_animation(a, 0.5); assert_float_equal(f, 1.0);
}

void test_identity() {
    float f, start = 0.0, end = 1.0;
    Animation* a = transform(linear_animationf(&f, 1, &start, &end), identity_transform());
    g_assert(animation_duration(a) == 1.0);

    update_animation(a, 0.0); assert_float_equal(f, 0.0);
    update_animation(a, 0.5); assert_float_equal(f, 0.5);
    update_animation(a, 1.0); assert_float_equal(f, 1.0);
}

void test_sinusoid() {
    float f, start = 0.0, end = 1.0;
    Animation* a = transform(linear_animationf(&f, 1, &start, &end), sinusoid_transform());
    g_assert(animation_duration(a) == 1.0);

    update_animation(a, 0.0); assert_float_equal(f, 0.0);
    update_animation(a, 0.5); assert_float_equal(f, sinf(G_PI_4));
    update_animation(a, 1.0); assert_float_equal(f, 1.0);
}

void test_reverse() {
    float f, start = 0.0, end = 1.0;
    Animation* a = transform(linear_animationf(&f, 1, &start, &end), reverse_transform());
    g_assert(animation_duration(a) == 1.0);

    update_animation(a, 0.0); assert_float_equal(f, 1.0);
    update_animation(a, 0.5); assert_float_equal(f, 0.5);
    update_animation(a, 1.0); assert_float_equal(f, 0.0);
}

void test_exp() {
    float f, start = 0.0, end = 1.0;
    Animation* a = transform(linear_animationf(&f, 1, &start, &end), exp_transform(2.0));
    g_assert(animation_duration(a) == 1.0);

    update_animation(a, 0.0); assert_float_equal(f, 0.0);
    update_animation(a, 0.5); assert_float_equal(f, powf(0.5, 2.0));
    update_animation(a, 1.0); assert_float_equal(f, 1.0);
}

void test_delay() {
    float f, start = 0.0, end = 1.0;
    Animation* a = delay(linear_animationf(&f, 1, &start, &end), 1.0);
    g_assert(animation_duration(a) == 2.0);

    update_animation(a, 0.0); assert_float_equal(f, 0.0);
    update_animation(a, 0.5); assert_float_equal(f, 0.0);
    update_animation(a, 1.0); assert_float_equal(f, 0.0);
    update_animation(a, 1.5); assert_float_equal(f, 0.5);
    update_animation(a, 2.0); assert_float_equal(f, 1.0);
}

int main(int argc, char** argv) {
    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/libanim/animation/null", test_null);
    g_test_add_func("/libanim/animation/linearf/one_dimension/1", test_linearf_one_dimension_unit);
    g_test_add_func("/libanim/animation/linearf/one_dimension/2", test_linearf_one_dimension_range);
    g_test_add_func("/libanim/animation/linearf/three_dimensions/1", test_linearf_three_dimensions_unit);
    g_test_add_func("/libanim/animation/linearf/three_dimensions/2", test_linearf_three_dimensions_range);
    g_test_add_func("/libanim/animation/sequence", test_sequence);
    g_test_add_func("/libanim/animation/parallel", test_parallel);
    g_test_add_func("/libanim/animation/delay", test_delay);
    g_test_add_func("/libanim/animation/scale/1", test_scale_up);
    g_test_add_func("/libanim/animation/scale/2", test_scale_down);
    g_test_add_func("/libanim/transform/identity", test_identity);
    g_test_add_func("/libanim/transform/sinusoid", test_sinusoid);
    g_test_add_func("/libanim/transform/reverse", test_reverse);
    g_test_add_func("/libanim/transform/exp", test_exp);
	g_test_run();
}
