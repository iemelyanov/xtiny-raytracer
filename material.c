#include "xtiny_raytracer.h"

static vec3 randomInUnitSphere() {
    vec3 t = (vec3){1.0, 1.0, 1.0};
    vec3 v = vec3MulScalar((vec3){drand48(), drand48(), drand48()}, 2.0);
    vec3 p = vec3Sub(v, t);
    while (vec3SquaredLen(p) >= 1.0) {
        v = vec3MulScalar((vec3){drand48(), drand48(), drand48()}, 2.0);
        p = vec3Sub(v, t);
    }
    return p;
}

static inline vec3 reflect(vec3 v1, vec3 v2) {
    float a = vec3Dot(v1, v2);
    vec3 t = vec3MulScalar(v2, a);
    t = vec3MulScalar(t, 2);
    return vec3Sub(v1, t);
}

static inline bool refract(vec3 v, vec3 n, float ni_over_nt, vec3 *refracted) {
    vec3 uv = vec3UnitVector(v);
    float dt = vec3Dot(uv, n);
    float discriminant = 1.0 - ni_over_nt * ni_over_nt * (1.0 - dt * dt);
    if (discriminant > 0.0) {
        vec3 a = vec3MulScalar(n, dt);
        vec3 b = vec3Sub(uv, a);
        vec3 c = vec3MulScalar(b, ni_over_nt);
        vec3 d = vec3MulScalar(n, sqrt(discriminant));
        *refracted = vec3Sub(c, d);
        return true;
    }
    return false;
}

static inline float schlick(float cosine, float ref_idx) {
    float r0 = (1 - ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1 - r0) * pow((1 - cosine), 5);
}

lambertian *lambertianNew(vec3 v) {
    material material = {.scatter = lambertianScatter};
    lambertian *l = xalloc(sizeof(lambertian));
    l->base = material;
    l->albedo = v;
    return l;
}

bool lambertianScatter(material *m, const ray *r_in, const hitRecord *rec, vec3 *attenuation, ray *scattered) {
    lambertian *l = (lambertian*)m;
    vec3 target = vec3Add(vec3Add(rec->p, rec->normal), randomInUnitSphere());
    *scattered = (ray){rec->p, vec3Sub(target, rec->p)};
    *attenuation = l->albedo;
    return true;
}

metal *metalNew(vec3 v, float f) {
    material material = {.scatter = metalScatter};
    metal *m = xalloc(sizeof(metal));
    m->base = material;
    m->albedo = v;
    m->fuzz = f < 1 ? f : 1;
    return m;
}

bool metalScatter(material *_m, const ray *r_in, const hitRecord *rec, vec3 *attenuation, ray *scattered) {
    metal *m = (metal*)_m;
    vec3 reflected = reflect(vec3UnitVector(rayDirection(r_in)), rec->normal);
    vec3 t = vec3MulScalar(randomInUnitSphere(), m->fuzz);
    *scattered = (ray){rec->p, vec3Add(reflected, t)};
    *attenuation = m->albedo;
    return vec3Dot(rayDirection(scattered), rec->normal) > 0.0;
}

dielectric *dielectricNew(float ri) {
    material material = {.scatter = dielectricScatter};
    dielectric *d = xalloc(sizeof(dielectric));
    d->base = material;
    d->ref_idx = ri;
    return d;
}

bool dielectricScatter(material *m, const ray *r_in, const hitRecord *rec, vec3 *attenuation, ray *scattered) {
    dielectric *d = (dielectric*)m;
    
    vec3 outward_normal = { 0 };
    vec3 refracted = { 0 };
    vec3 reflected = reflect(rayDirection(r_in), rec->normal);
    
    float reflect_prob = 0;
    float cosine = 0;
    float ni_over_nt = 0;
    
    *attenuation = (vec3){1.0, 1.0, 1.0};
    
    if (vec3Dot(rayDirection(r_in), rec->normal) > 0) {
        outward_normal = vec3MulScalar(rec->normal, -1);
        ni_over_nt = d->ref_idx;
        cosine = d->ref_idx * vec3Dot(rayDirection(r_in), rec->normal) / vec3Len(rayDirection(r_in));
    } else {
        outward_normal = rec->normal;
        ni_over_nt = 1.0 / d->ref_idx;
        cosine = -vec3Dot(rayDirection(r_in), rec->normal) / vec3Len(rayDirection(r_in));
    }

    if (refract(rayDirection(r_in), outward_normal, ni_over_nt, &refracted)) {
        *scattered = (ray){rec->p, refracted};
        reflect_prob = schlick(cosine, d->ref_idx);
    } else {
        *scattered = (ray){rec->p, reflected};
        reflect_prob = 1.0;
    }
    *scattered = drand48() < reflect_prob ? (ray){rec->p, reflected} : (ray){rec->p, refracted};

    return true;
}
