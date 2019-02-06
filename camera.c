#include <stdlib.h>
#include "camera.h"

#define M_PI 3.14159265358979323846

static inline Vec3 random_in_unit_disk() {
    Vec3 p;
    do {
        p = vec3_sub(
            vec3_mul_scalar(
                vec3(drand48(), drand48(), 0),
                2.0
            ),
            vec3(1, 1, 0)
        );
    } while (vec3_dot(p, p) >= 1.0);
    return p;
}

void camera_init(
    Camera *c,
    Vec3 lookfrom,
    Vec3 lookat,
    Vec3 vup,
    float vfov,
    float aspect,
    float aperture,
    float focus_dist
)  {
    float theta = vfov * M_PI / 180;
    float half_height = tan(theta / 2);
    float half_width = aspect * half_height;

    c->origin = lookfrom;
    c->lens_radius = aperture / 2.0;
    
    c->w = vec3_unit_vector(vec3_sub(lookfrom, lookat));
    c->u = vec3_unit_vector(vec3_cross(vup, c->w));
    c->v = vec3_cross(c->w, c->u);
    
    Vec3 a = vec3_mul_scalar(c->u, half_width * focus_dist);
    Vec3 s = vec3_mul_scalar(c->v, half_height * focus_dist);
    Vec3 d = vec3_mul_scalar(c->w, focus_dist);

    c->lower_left_corner = vec3_sub(vec3_sub(vec3_sub(c->origin, a), s), d);
    c->horizontal = vec3_mul_scalar(c->u, 2.0 * half_width * focus_dist);
    c->vertical = vec3_mul_scalar(c->v, 2.0 * half_height * focus_dist);
}

Ray camera_get_ray(Camera *c, float s, float t) {
    Vec3 rd = vec3_mul_scalar(random_in_unit_disk(), c->lens_radius);
    Vec3 offset = vec3_add(
        vec3_mul_scalar(c->u, rd.x),
        vec3_mul_scalar(c->v, rd.y)
    );
    Vec3 q = vec3_add(
        c->lower_left_corner,
        vec3_mul_scalar(c->horizontal, s)
    );
    Vec3 w = vec3_mul_scalar(c->vertical, t);
    Vec3 e = vec3_sub(vec3_add(q, w), c->origin);

    return ray(vec3_add(c->origin, offset), vec3_sub(e, offset));
}
