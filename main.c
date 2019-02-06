#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <float.h>

#include "ray.h"
#include "hitablelist.h"
#include "sphere.h"
#include "camera.h"

#define N 500

static Vec3 color(const Ray *r, HitableList *world, int32_t depth) {
    HitRecord rec = { 0 };
    if (hitablelist_hit(world, r, 0.001, FLT_MAX, &rec)) {
        Ray scattered = ray(vec3(0, 0, 0), vec3(0, 0, 0));
        Vec3 attenuation = vec3(0, 0, 0);
        float q = rec.material->scatter(
            rec.material, r, &rec, &attenuation, &scattered);
        if (depth < 50 && q) {
           return vec3_mul(attenuation, color(&scattered, world, depth + 1)); 
        }
        return vec3(0, 0, 0);
    }
    
    Vec3 unit_direction = vec3_unit_vector(ray_direcation(r));
    float t = 0.5 * (unit_direction.y + 1);

    return vec3_add(
        vec3_mul_scalar(vec3(1, 1, 1), 1 - t),
        vec3_mul_scalar(vec3(0.5, 0.7, 1), t)
    );
}

size_t random_scene(Hitable ***list_) {
    *list_ = (Hitable**)malloc(sizeof(Hitable*) * N);

    Hitable **list = *list_;
    
    list[0] = (Hitable*)sphere_new(
        vec3(0, -1000, 0), 1000,
        (Material*)lambertian_new(vec3(0.5, 0.5, 0.5))
    );

    size_t i = 1;
    
    for (int32_t a = -11; a < 11; a++) {
        for (int32_t b = -11; b < 11; b++) {
            float choose_mat = drand48();
            
            Vec3 center = vec3(a + 0.9 * drand48(), 0.2, b + 0.9 * drand48());
            Vec3 q = vec3_sub(center, vec3(4, 0.2, 0));
            
            if (vec3_len(q) <= 0.9) {
                continue;
            }

            if (choose_mat < 0.8) {
                Material *m = (Material*)lambertian_new(
                    vec3(
                        drand48() * drand48(),
                        drand48() * drand48(),
                        drand48() * drand48()
                    )
                );
                list[i++] = (Hitable*)sphere_new(center, 0.2, m);
            } else if (choose_mat < 0.95) {
                Material *m = (Material*)metal_new(
                    vec3(
                        0.5 * (1 + drand48()),
                        0.5 * (1 + drand48()),
                        0.5 * (1 + drand48())
                    ),
                    0.5 * drand48()
                );
                list[i++] = (Hitable*)sphere_new(center, 0.2, m);
            } else {
                Material *m = (Material*)dielectrict_new(1.5);
                list[i++] = (Hitable*)sphere_new(center, 0.2, m);
            }
        }
    }

    list[i++] = (Hitable*)sphere_new(
        vec3(0, 1, 0), 1,
        (Material*)dielectrict_new(1.5)
    );
    list[i++] = (Hitable*)sphere_new(
        vec3(-4, 1, 0), 1,
        (Material*)lambertian_new(vec3(0.4, 0.2, 0.1))
    );
    list[i++] = (Hitable*)sphere_new(
        vec3(4, 1, 0), 1,
        (Material*)metal_new(vec3(0.7, 0.6, 0.5), 0)
    );

    return i;
}

int main() {
    const uint32_t nx = 600;
    const uint32_t ny = 300;
    const uint32_t ns = 100;

    printf("P3\n%d %d\n255\n", nx, ny);

    Hitable **list = NULL;
    size_t n = random_scene(&list);

    HitableList world;
    hitablelist_init(&world, list, n);

    Vec3 lookfrom = vec3(10, 2, 2.5);
    Vec3 lookat = vec3(-1.5, 0, -1.1);

    Vec3 q = vec3_sub(lookfrom, lookat);
    float dist_to_focus = vec3_len(q);
    float aperture = 0;

    Camera cam;
    camera_init(
        &cam,
        lookfrom,
        lookat,
        vec3(0, 1, 0),
        24,
        (float)nx / (float)ny,
        aperture,
        dist_to_focus
    );

    for (uint32_t j = ny; j > 0; j--) {
        for (uint32_t i = 0; i < nx; i++) {
            Vec3 col = vec3(0, 0, 0);
            for (uint32_t s = 0; s < ns; s++) {
                float u = (float)(i + drand48()) / (float)nx;
                float v = (float)(j + drand48()) / (float)ny;
                Ray r = camera_get_ray(&cam, u, v);
                col = vec3_add(col, color(&r, &world, 0));
            }

            col = vec3_div_scalar(col, (float)ns);
            col = vec3(sqrtf(col.x), sqrtf(col.y), sqrtf(col.z));
        
            int ir = (uint32_t)255.99 * col.x;
            int ig = (uint32_t)255.99 * col.y;
            int ib = (uint32_t)255.99 * col.z;

            printf("%d %d %d\n", ir, ig, ib);
        }
    }

    // TODO:
    // for (size_t i = 0; i < N; i++) {
    //     printf("OK: %d\n", i);
    //     list[i]->destroy(list[i]);
    // }

    return 0;
}
