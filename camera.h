#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "ray.h"

typedef struct Camera Camera;

struct Camera {
    Vec3 lower_left_corner;
    Vec3 horizontal;
    Vec3 vertical;
    Vec3 origin;
    Vec3 u;
    Vec3 v;
    Vec3 w;
    float lens_radius;
};

void camera_init(Camera *c, Vec3 lookfrom, Vec3 lookat, Vec3 vup, float vfov,
    float aspect, float aperture, float focus_dist);

Ray camera_get_ray(Camera *c, float s, float t);

#endif
