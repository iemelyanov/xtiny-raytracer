#ifndef _RAY_H_
#define _RAY_H_

#include "vec.h"

typedef struct Ray Ray;

struct Ray {
    Vec3 a;
    Vec3 b;
};

static inline Ray ray(Vec3 a, Vec3 b) {
    Ray r = {
        .a = a,
        .b = b
    };
    return r;
}

static inline Vec3 ray_origin(const Ray *r) {
    return r->a;
}

static inline Vec3 ray_direcation(const Ray *r) {
    return r->b;
}

static inline Vec3 ray_point_at_parameter(const Ray *r, float t) {
    return vec3_add(r->a, vec3_mul_scalar(r->b, t));
}

#endif
