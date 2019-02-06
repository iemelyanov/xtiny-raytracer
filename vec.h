#ifndef _VEC_H_
#define _VEC_H_

#include <math.h>

typedef struct Vec3 Vec3;

struct Vec3 {
    float x;
    float y;
    float z;
};

static inline Vec3 vec3(float x, float y, float z) {
    Vec3 v = {x, y, z};
    return v;
}

static inline Vec3 vec3_mul(Vec3 v1, Vec3 v2) {
    return vec3(
        v1.x * v2.x,
        v1.y * v2.y,
        v1.z * v2.z
    );
}

static inline Vec3 vec3_add(Vec3 v1, Vec3 v2) {
    return vec3(
        v1.x + v2.x,
        v1.y + v2.y,
        v1.z + v2.z
    );
}

static inline Vec3 vec3_sub(Vec3 v1, Vec3 v2) {
    return vec3(
        v1.x - v2.x,
        v1.y - v2.y,
        v1.z - v2.z
    );
}

static inline Vec3 vec3_mul_scalar(Vec3 v, float k) {
    return vec3(
        v.x * k,
        v.y * k,
        v.z * k
    );
}

static inline Vec3 vec3_div_scalar(Vec3 v, float k) {
    return vec3(
        v.x / k,
        v.y / k,
        v.z / k
    );
}

static inline Vec3 vec3_neg(Vec3 v) {
    return vec3_mul_scalar(v, -1.0);
}

static inline Vec3 vec3_cross(Vec3 v1, Vec3 v2) {
    return vec3(
        v1.y * v2.z - v1.z * v2.y,
        -(v1.x * v2.z - v1.z * v2.x),
        v1.x * v2.y - v1.y * v2.x
    );
}

static inline float vec3_squared_len(Vec3 v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

static inline float vec3_len(Vec3 v) {
    return sqrtf(vec3_squared_len(v));
}

static inline Vec3 vec3_unit_vector(Vec3 v) {
    return vec3_div_scalar(v, vec3_len(v));
}

static inline float vec3_dot(Vec3 v1, Vec3 v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

#endif
