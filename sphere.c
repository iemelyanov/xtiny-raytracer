#include "xtiny_raytracer.h"

sphere *sphereNew(vec3 center, float r, material *m) {
    sphere *s = xalloc(sizeof(sphere));
    sphereInit(s, center, r, m);
    return s;
}

void sphereDestroy(hitable *h) {
    sphere *s = (sphere*)h;
    xfree(s->material);
    xfree(s);
}

void sphereInit(sphere *s, vec3 center, float r, material *m) {
    hitable hitable = {
        .hit = sphereHit,
        .destroy = sphereDestroy
    };
    s->base = hitable;
    s->center = center;
    s->radius = r;
    s->material = m;
}

bool sphereHit(hitable *h, const ray *r, float t_min, float t_max, hitRecord *rec) {
    sphere *s = (sphere*)h;

    vec3 oc = vec3Sub(rayOrigin(r), s->center);
    vec3 direction = rayDirection(r);
    
    float a = vec3Dot(direction, direction);
    float b = vec3Dot(oc, direction);
    float c = vec3Dot(oc, oc) - s->radius * s->radius;

    float discriminant = b * b - a * c;
    if (discriminant > 0.0) {
        float tmp = (-b - sqrt(discriminant)) / a;
        if (tmp < t_max && tmp > t_min) {
            rec->t = tmp;
            rec->p = rayPointAtParameter(r, rec->t);
            rec->normal = vec3DivScalar(vec3Sub(rec->p, s->center), s->radius);
            rec->material = s->material;
            return true;
        }
        tmp = (-b + sqrt(discriminant)) / a;
        if (tmp < t_max && tmp > t_min) {
            rec->t = tmp;
            rec->p = rayPointAtParameter(r, rec->t);
            rec->normal = vec3DivScalar(vec3Sub(rec->p, s->center), s->radius);
            rec->material = s->material;
            return true;
        }
    }

    return false;
}
