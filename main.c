#include <stdio.h>
#include <stdatomic.h>
#include "xtiny_raytracer.h"

#define N 500

#ifdef DEBUG
static int total_alloc = 0;
static int total_dealloc = 0;
#endif

void *xalloc(size_t size) {
    #ifdef DEBUG
    total_alloc++;
    #endif
    return malloc(size);
}

void xfree(void *p) {
    #ifdef DEBUG
    total_dealloc++;
    #endif
    free(p);
}

static vec3 color(const ray *r, const hitableList *world, int32_t depth) {
    hitRecord rec = { 0 };
    if (hitableListHit(world, r, 0.001, FLT_MAX, &rec)) {
        ray scattered = {(vec3){0, 0, 0}, (vec3){0, 0, 0}};
        vec3 attenuation = {0, 0, 0};
        float q = rec.material->scatter(
            rec.material, r, &rec, &attenuation, &scattered);
        if (depth < 50 && q)
           return vec3Mul(attenuation, color(&scattered, world, depth + 1));
        return (vec3){0, 0, 0};
    }

    vec3 unit_direction = vec3UnitVector(rayDirection(r));
    float t = 0.5 * (unit_direction.y + 1);

    return vec3Add(vec3MulScalar((vec3){1, 1, 1}, 1 - t), vec3MulScalar((vec3){0.5, 0.7, 1}, t));
}

size_t randomScene(hitable ***list_) {
    *list_ = (hitable**)xalloc(sizeof(hitable*) * N);
    hitable **list = *list_;
    
    list[0] = (hitable*)sphereNew((vec3){0, -1000, 0}, 1000, (material*)lambertianNew((vec3){0.5, 0.5, 0.5}));

    size_t i = 1;
    for (int32_t a = -11; a < 11; a++) {
        for (int32_t b = -11; b < 11; b++) {
            float choose_mat = drand48();

            vec3 center = {a + 0.9 * drand48(), 0.2, b + 0.9 * drand48()};
            vec3 q = vec3Sub(center, (vec3){4, 0.2, 0});

            if (vec3Len(q) <= 0.9)
                continue;

            if (choose_mat < 0.8) {
                material *m = (material*)lambertianNew(
                    (vec3){
                        drand48() * drand48(),
                        drand48() * drand48(),
                        drand48() * drand48()
                    }
                );
                list[i++] = (hitable*)sphereNew(center, 0.2, m);
            } else if (choose_mat < 0.95) {
                material *m = (material*)metalNew(
                    (vec3){
                        0.5 * (1 + drand48()),
                        0.5 * (1 + drand48()),
                        0.5 * (1 + drand48())
                    },
                    0.5 * drand48()
                );
                list[i++] = (hitable*)sphereNew(center, 0.2, m);
            } else {
                material *m = (material*)dielectricNew(1.5);
                list[i++] = (hitable*)sphereNew(center, 0.2, m);
            }
        }
    }

    list[i++] = (hitable*)sphereNew((vec3){0, 1, 0}, 1, (material*)dielectricNew(1.5));
    list[i++] = (hitable*)sphereNew((vec3){-4, 1, 0}, 1, (material*)lambertianNew((vec3){0.4, 0.2, 0.1}));
    list[i++] = (hitable*)sphereNew((vec3){4, 1, 0}, 1, (material*)metalNew((vec3){0.7, 0.6, 0.5}, 0));

    return i;
}

uint32_t numDigits(uint32_t n) {
    uint32_t l = 0;
    while (n) {
        n = n / 10;
        l++;
    }
    return l;
}

int main(int argc, char **argv) {
    const uint32_t nx = 320;
    const uint32_t ny = 240;
    const uint32_t ns = 100;

    hitable **list = NULL;
    size_t n = randomScene(&list);

    hitableList world;
    hitableListInit(&world, list, n);

    vec3 lookfrom = {10, 2, 2.5};
    vec3 lookat = {-1.5, 0, -1.1};

    vec3 q = vec3Sub(lookfrom, lookat);
    float dist_to_focus = vec3Len(q);
    float aperture = 0;

    camera cam;
    cameraInit(&cam, lookfrom, lookat, (vec3){0, 1, 0}, 24, (float)nx / (float)ny, aperture, dist_to_focus);


    printf("P3\n%d %d\n255\n", nx, ny);
    for (uint32_t j = ny; j > 0; j--) {
        for (uint32_t i = 0; i < nx; i++) {
            vec3 col = {0, 0, 0};
            for (uint32_t s = 0; s < ns; s++) {
                float u = (float)(i + drand48()) / (float)nx;
                float v = (float)(j + drand48()) / (float)ny;
                ray r = cameraGetRay(&cam, u, v);
                col = vec3Add(col, color(&r, &world, 0));
            }

            col = vec3DivScalar(col, (float)ns);
            col = (vec3){sqrtf(col.x), sqrtf(col.y), sqrtf(col.z)};

            int ir = (uint32_t)255.99 * col.x;
            int ig = (uint32_t)255.99 * col.y;
            int ib = (uint32_t)255.99 * col.z;

            printf("%d %d %d\n", ir, ig, ib);
        }
    }

    for (size_t i = 0; i < n; i++)
        list[i]->destroy(list[i]);
    xfree(list);

    #ifdef DEBUG
    fprintf(stderr, "alloc: %d dealloc: %d\n", total_alloc, total_dealloc);
    #endif

    return 0;
}
