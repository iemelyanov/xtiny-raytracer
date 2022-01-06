#ifndef XTINY_RAYTRACE_H
#define XTINY_RAYTRACE_H

#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <float.h>
#include <math.h>

void *xalloc(size_t size);
void xfree(void *p);

typedef struct vec3 vec3;

struct vec3 {
    float x;
    float y;
    float z;
};

static inline vec3 vec3Mul(vec3 v1, vec3 v2) {
    return (vec3){
        v1.x * v2.x,
        v1.y * v2.y,
        v1.z * v2.z
    };
}

static inline vec3 vec3Add(vec3 v1, vec3 v2) {
    return (vec3){
        v1.x + v2.x,
        v1.y + v2.y,
        v1.z + v2.z
    };
}

static inline vec3 vec3Sub(vec3 v1, vec3 v2) {
    return (vec3){
        v1.x - v2.x,
        v1.y - v2.y,
        v1.z - v2.z
    };
}

static inline vec3 vec3MulScalar(vec3 v, float k) {
    return (vec3){
        v.x * k,
        v.y * k,
        v.z * k
    };
}

static inline vec3 vec3DivScalar(vec3 v, float k) {
    return (vec3){
        v.x / k,
        v.y / k,
        v.z / k
    };
}

static inline vec3 vec3Neg(vec3 v) {
    return vec3MulScalar(v, -1.0);
}

static inline vec3 vec3Cross(vec3 v1, vec3 v2) {
    return (vec3){
        v1.y * v2.z - v1.z * v2.y,
        -(v1.x * v2.z - v1.z * v2.x),
        v1.x * v2.y - v1.y * v2.x
    };
}

static inline float vec3SquaredLen(vec3 v) {
    return v.x * v.x + v.y * v.y + v.z * v.z;
}

static inline float vec3Len(vec3 v) {
    return sqrtf(vec3SquaredLen(v));
}

static inline vec3 vec3UnitVector(vec3 v) {
    return vec3DivScalar(v, vec3Len(v));
}

static inline float vec3Dot(vec3 v1, vec3 v2) {
    return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

typedef struct ray ray;

struct ray {
    vec3 a;
    vec3 b;
};

static inline vec3 rayOrigin(const ray *r) {
    return r->a;
}

static inline vec3 rayDirection(const ray *r) {
    return r->b;
}

static inline vec3 rayPointAtParameter(const ray *r, float t) {
    return vec3Add(r->a, vec3MulScalar(r->b, t));
}

typedef struct hitRecord hitRecord;
typedef struct hitable hitable;
typedef struct hitableVtbl hitableVtbl;
typedef struct material material;

struct hitRecord {
    float t;
    vec3 p;
    vec3 normal;
    material *material;
};

struct hitable {
    bool (*hit)(hitable *h, const ray *r, float t_min, float t_max, hitRecord *rec);
    void (*destroy)(hitable *h);
};

typedef struct hitableList hitableList;

struct hitableList {
    hitable **list;
    size_t size;
};

void hitableListInit(hitableList *h, hitable **l, size_t size);
bool hitableListHit(const hitableList *h, const ray *r, float t_min, float t_max, hitRecord *rec);

typedef struct camera camera;

struct camera {
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 origin;
    vec3 u;
    vec3 v;
    vec3 w;
    float lens_radius;
};

void cameraInit(camera *c, vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect, float aperture, float focus_dist);
ray cameraGetRay(const camera *c, float s, float t);

typedef struct lambertian lambertian;
typedef struct metal metal;
typedef struct dielectric dielectric;

struct material {
    bool (*scatter)(material *m, const ray *r_in, const hitRecord *rec, vec3 *attenuation, ray *scattered);
};

struct lambertian {
    material base;
    vec3 albedo;
};

struct metal {
    material base;
    vec3 albedo;
    float fuzz;
};

struct dielectric {
    material base;
    float ref_idx;
};

lambertian *lambertianNew(vec3 v);
bool lambertianScatter(material *m, const ray *r_in, const hitRecord *rec, vec3 *attenuation, ray *scattered);

metal *metalNew(vec3 v, float f);
bool metalScatter(material *m, const ray *r_in, const hitRecord *rec, vec3 *attenuation, ray *scattered);

dielectric *dielectricNew(float ri);
bool dielectricScatter(material *m, const ray *r_in, const hitRecord *rec, vec3 *attenuation, ray *scattered);

typedef struct sphere sphere;

struct sphere {
    hitable base;
    vec3 center;
    float radius;
    material *material;
};

sphere *sphereNew(vec3 center, float r, material *m);
void sphereDestroy(hitable *h);
void sphereInit(sphere *s, vec3 center, float r, material *m);
bool sphereHit(hitable *h, const ray *r, float t_min, float t_max, hitRecord *rec);

#endif