#include <stdio.h>
#include <pthread.h>
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
        float q = rec.material->scatter(rec.material, r, &rec, &attenuation, &scattered);
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

typedef struct {
    uint8_t r, g, b;
} rgb;

typedef struct {
    uint32_t ny_a, ny_b, ny, nx;
    uint32_t ns;
    camera *cam;
    hitableList *world;
    rgb *result_bucket;
} task;

void *render(void *v) {
    task *t = (task*)v;

    int k = 0;
    for (uint32_t j = t->ny_a; j > t->ny_b; j--) {
        for (uint32_t i = 0; i < t->nx; i++) {
            vec3 col = {0, 0, 0};
            for (uint32_t s = 0; s < t->ns; s++) {
                float u = (float)(i + drand48()) / (float)t->nx;
                float v = (float)(j + drand48()) / (float)t->ny;
                ray r = cameraGetRay(t->cam, u, v);
                col = vec3Add(col, color(&r, t->world, 0));
            }

            col = vec3DivScalar(col, (float)t->ns);
            col = (vec3){sqrtf(col.x), sqrtf(col.y), sqrtf(col.z)};

            t->result_bucket[k++] = (rgb){
                .r = (uint32_t)255.99 * col.x,
                .g = (uint32_t)255.99 * col.y,
                .b = (uint32_t)255.99 * col.z,
            };
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    const uint32_t nx = 800;
    const uint32_t ny = 600;
    const uint32_t ns = 200;

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

    const int num_threads = 4;
    const int bucket_size = ny / num_threads * nx;
    rgb pixels[num_threads][bucket_size];
    task tasks[num_threads] = { 0 };

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    pthread_t threads[num_threads];
    uint32_t ny_diff = ny / num_threads;
    uint32_t ny_a = ny, ny_b = ny - ny_diff;
    for (int i = 0; i < num_threads; i++) {
        tasks[i] = (task){
            .ny_a = ny_a,
            .ny_b = ny_b,
            .nx = nx,
            .ny = ny,
            .ns = ns,
            .cam = &cam,
            .world = &world,
            .result_bucket = pixels[i],
        };
        int rc = pthread_create(&threads[i], NULL, render, (void *)(&tasks[i]));
        if (rc) {
            fprintf(stderr, "ERROR: return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
        ny_a = ny_b;
        ny_b -= ny_diff;
    }

    void *status;
    pthread_attr_destroy(&attr);
    for (int i = 0; i < num_threads; i++) {
        int rc = pthread_join(threads[i], &status);
        if (rc) {
            fprintf(stderr, "ERROR: return code from pthread_join() is %d\n", rc);
            exit(-1);
        }
    }

    for (size_t i = 0; i < n; i++)
        list[i]->destroy(list[i]);
    xfree(list);

    printf("P3\n%d %d\n255\n", nx, ny);
    for (int i = 0; i < num_threads; i++) {
        for (int j = 0; j < bucket_size; j++) {
            rgb c = pixels[i][j];
            printf("%d %d %d\n", c.r, c.g, c.b);
        }
    }

    #ifdef DEBUG
    fprintf(stderr, "alloc: %d dealloc: %d\n", total_alloc, total_dealloc);
    #endif

    pthread_exit(NULL);
}
