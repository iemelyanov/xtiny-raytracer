#include <stdlib.h>
#include "sphere.h"

Sphere *sphere_new(Vec3 center, float r, Material *m) {
    Sphere *s = (Sphere*)malloc(sizeof(Sphere));
    sphere_init(s, center, r, m);
    return s;
}

void sphere_destroy(Hitable *h) {
    Sphere *s = (Sphere*)h;
    free(s->material);
    free(s);
}

void sphere_init(Sphere *s, Vec3 center, float r, Material *m) {
    Hitable hitable = {
        .hit = sphere_hit,
        .destroy = sphere_destroy
    };
    s->base = hitable;
    s->center = center;
    s->radius = r;
    s->material = m;
}

bool sphere_hit(Hitable *h, const Ray *r, float t_min, float t_max, HitRecord *rec) {
    Sphere *s = (Sphere*)h;

    Vec3 oc = vec3_sub(ray_origin(r), s->center);
    Vec3 direction = ray_direcation(r);
    
    float a = vec3_dot(direction, direction);
    float b = vec3_dot(oc, direction);
    float c = vec3_dot(oc, oc) - s->radius * s->radius;

    float discriminant = b * b - a * c;
    if (discriminant > 0.0) {
        float tmp = (-b - sqrt(discriminant)) / a;
        if (tmp < t_max && tmp > t_min) {
            rec->t = tmp;
            rec->p = ray_point_at_parameter(r, rec->t);
            rec->normal = vec3_div_scalar(vec3_sub(rec->p, s->center), s->radius);
            rec->material = s->material;
            return true;
        }
        tmp = (-b + sqrt(discriminant)) / a;
        if (tmp < t_max && tmp > t_min) {
            rec->t = tmp;
            rec->p = ray_point_at_parameter(r, rec->t);
            rec->normal = vec3_div_scalar(vec3_sub(rec->p, s->center), s->radius);
            rec->material = s->material;
            return true;
        }
    }

    return false;
}
