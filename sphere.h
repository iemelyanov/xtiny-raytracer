#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "hitable.h"
#include "material.h"

typedef struct Sphere Sphere;

struct Sphere {
    Hitable base;
    Vec3 center;
    float radius;
    Material *material;
};

Sphere *sphere_new(Vec3 center, float r, Material *m);

void sphere_destroy(Hitable *h);

void sphere_init(Sphere *s, Vec3 center, float r, Material *m);

bool sphere_hit(Hitable *h, const Ray *r, float t_min, float t_max, HitRecord *rec);

#endif
