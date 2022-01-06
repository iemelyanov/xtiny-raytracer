#include "xtiny_raytracer.h"

static inline vec3 randomInUnitDisk() {
    vec3 p;
    do {
        p = vec3Sub(
            vec3MulScalar((vec3){drand48(), drand48(), 0}, 2.0),
            (vec3){1, 1, 0}
        );
    } while (vec3Dot(p, p) >= 1.0);
    return p;
}

void cameraInit(camera *c, vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect, float aperture, float focus_dist) {
    float theta = vfov * M_PI / 180;
    float half_height = tan(theta / 2);
    float half_width = aspect * half_height;

    c->origin = lookfrom;
    c->lens_radius = aperture / 2.0;
    
    c->w = vec3UnitVector(vec3Sub(lookfrom, lookat));
    c->u = vec3UnitVector(vec3Cross(vup, c->w));
    c->v = vec3Cross(c->w, c->u);
    
    vec3 a = vec3MulScalar(c->u, half_width * focus_dist);
    vec3 s = vec3MulScalar(c->v, half_height * focus_dist);
    vec3 d = vec3MulScalar(c->w, focus_dist);

    c->lower_left_corner = vec3Sub(vec3Sub(vec3Sub(c->origin, a), s), d);
    c->horizontal = vec3MulScalar(c->u, 2.0 * half_width * focus_dist);
    c->vertical = vec3MulScalar(c->v, 2.0 * half_height * focus_dist);
}

ray cameraGetRay(const camera *c, float s, float t) {
    vec3 rd = vec3MulScalar(randomInUnitDisk(), c->lens_radius);
    vec3 offset = vec3Add(vec3MulScalar(c->u, rd.x), vec3MulScalar(c->v, rd.y));
    vec3 q = vec3Add(c->lower_left_corner, vec3MulScalar(c->horizontal, s));
    vec3 w = vec3MulScalar(c->vertical, t);
    vec3 e = vec3Sub(vec3Add(q, w), c->origin);
    return (ray){vec3Add(c->origin, offset), vec3Sub(e, offset)};
}
