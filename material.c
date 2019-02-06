#include <stdlib.h>
#include "material.h"

static Vec3 random_in_unit_sphere() {
    Vec3 t = vec3(1.0, 1.0, 1.0);
    Vec3 v = vec3_mul_scalar(vec3(drand48(), drand48(), drand48()), 2.0);
    Vec3 p = vec3_sub(v, t);
    while (vec3_squared_len(p) >= 1.0) {
        v = vec3_mul_scalar(vec3(drand48(), drand48(), drand48()), 2.0);
        p = vec3_sub(v, t);
    }
    return p;
}

static inline Vec3 reflect(Vec3 v1, Vec3 v2) {
    float a = vec3_dot(v1, v2);
    Vec3 t = vec3_mul_scalar(v2, a);
    t = vec3_mul_scalar(t, 2);
    return vec3_sub(v1, t);
}

static inline bool refaract(Vec3 v, Vec3 n, float ni_over_nt, Vec3 *refracted) {
    Vec3 uv = vec3_unit_vector(v);
    float dt = vec3_dot(uv, n);
    float discriminant = 1.0 - ni_over_nt * ni_over_nt * (1.0 - dt * dt);
    if (discriminant > 0.0) {
        Vec3 a = vec3_mul_scalar(n, dt);
        Vec3 b = vec3_sub(uv, a);
        Vec3 c = vec3_mul_scalar(b, ni_over_nt);
        Vec3 d = vec3_mul_scalar(n, sqrt(discriminant));
        *refracted = vec3_sub(c, d);
        return true;
    }
    return false;
}

static inline float schlick(float cosine, float ref_idx) {
    float r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}

Lambertian *lambertian_new(Vec3 v) {
    Material material = {
        .scatter = lambertian_scatter
    };
    Lambertian *l = (Lambertian*)malloc(sizeof(Lambertian));
    l->base = material;
    l->albedo = v;
    return l;
}

bool lambertian_scatter(
    Material *m,
    const Ray *r_in,
    const HitRecord *rec,
    Vec3 *attenuation,
    Ray *scattered
) {
    Lambertian *l = (Lambertian*)m;
    Vec3 target = vec3_add(vec3_add(rec->p, rec->normal), random_in_unit_sphere());
    *scattered = ray(rec->p, vec3_sub(target, rec->p));
    *attenuation = l->albedo;
    return true;
}

Metal *metal_new(Vec3 v, float f) {
    Material material = {
        .scatter = metal_scatter
    };
    Metal *m = (Metal*)malloc(sizeof(Metal));
    m->base = material;
    m->albedo = v;
    if (f < 1) {
        m->fuzz = f;
    } else {
        m->fuzz = 1;
    }
    return m;
}

bool metal_scatter(
    Material *_m,
    const Ray *r_in,
    const HitRecord *rec,
    Vec3 *attenuation,
    Ray *scattered
) {
    Metal *m = (Metal*)_m;
    Vec3 reflected = reflect(vec3_unit_vector(ray_direcation(r_in)), rec->normal);
    Vec3 t = vec3_mul_scalar(random_in_unit_sphere(), m->fuzz);
    *scattered = ray(rec->p, vec3_add(reflected, t));
    *attenuation = m->albedo;
    return vec3_dot(ray_direcation(scattered), rec->normal) > 0.0;
}

Dielectrict *dielectrict_new(float ri) {
    Material material = {
        .scatter = dielectrict_scatter
    };
    Dielectrict *d = (Dielectrict*)malloc(sizeof(Dielectrict));
    d->base = material;
    d->ref_idx = ri;
    return d;
}

bool dielectrict_scatter(
    Material *m,
    const Ray *r_in,
    const HitRecord *rec,
    Vec3 *attenuation,
    Ray *scattered
) {
    Dielectrict *d = (Dielectrict*)m;
    
    Vec3 outward_normal = { 0 };
    Vec3 refracted = { 0 };
    Vec3 reflected = reflect(ray_direcation(r_in), rec->normal);
    
    float reflect_prob = 0;
    float cosine = 0;
    float ni_over_nt = 0;
    
    *attenuation = vec3(1.0, 1.0, 1.0);
    
    if (vec3_dot(ray_direcation(r_in), rec->normal) > 0) {
        outward_normal = vec3_mul_scalar(rec->normal, -1);
        ni_over_nt = d->ref_idx;
        cosine = d->ref_idx * vec3_dot(ray_direcation(r_in), rec->normal) / vec3_len(ray_direcation(r_in));
    } else {
        outward_normal = rec->normal;
        ni_over_nt = 1.0 / d->ref_idx;
        cosine = -vec3_dot(ray_direcation(r_in), rec->normal) / vec3_len(ray_direcation(r_in));
    }

    if (refaract(ray_direcation(r_in), outward_normal, ni_over_nt, &refracted)) {
        *scattered = ray(rec->p, refracted);
        reflect_prob = schlick(cosine, d->ref_idx);
    } else {
        *scattered = ray(rec->p, reflected);
        reflect_prob = 1.0;
    }

    if (drand48() < reflect_prob) {
        *scattered = ray(rec->p, reflected);
    } else {
        *scattered = ray(rec->p, refracted);
    }

    return true;
}
