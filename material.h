#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <stdbool.h>
#include "hitable.h"
#include "ray.h"

typedef struct Material Material;
typedef struct Lambertian Lambertian;
typedef struct Metal Metal;
typedef struct Dielectrict Dielectrict;

struct Material {
    bool (*scatter)(Material *m, const Ray *r_in, const HitRecord *rec, Vec3 *attenuation, Ray *scattered);
};

struct Lambertian {
    Material base;
    Vec3 albedo;
};

struct Metal {
    Material base;
    Vec3 albedo;
    float fuzz;
};

struct Dielectrict {
    Material base;
    float ref_idx;
};

Lambertian *lambertian_new(Vec3 v);

bool lambertian_scatter(Material *m, const Ray *r_in, const HitRecord *rec, Vec3 *attenuation, Ray *scattered);

Metal *metal_new(Vec3 v, float f);

bool metal_scatter(Material *m, const Ray *r_in, const HitRecord *rec, Vec3 *attenuation, Ray *scattered);

Dielectrict *dielectrict_new(float ri);

bool dielectrict_scatter(Material *m, const Ray *r_in, const HitRecord *rec, Vec3 *attenuation, Ray *scattered);

#endif
