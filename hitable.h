#ifndef _HITABLE_H_
#define _HITABLE_H_

#include <stdbool.h>
#include "ray.h"

typedef struct HitRecord HitRecord;
typedef struct Hitable Hitable;
typedef struct HitableVtbl HitableVtbl;

// Forward declaration
typedef struct Material Material;

struct HitRecord {
    float t;
    Vec3 p;
    Vec3 normal;
    Material *material;
};

struct Hitable {
    bool (*hit)(Hitable *h, const Ray *r, float t_min, float t_max, HitRecord *rec);
    void (*destroy)(Hitable *h);
};

#endif
