#ifndef _HITABLELIST_H_
#define _HITABLELIST_H_

#include <stddef.h>
#include "hitable.h"

typedef struct HitableList HitableList;

struct HitableList {
    Hitable **list;
    size_t size;
};

void hitablelist_init(HitableList *h, Hitable **l, size_t size);

bool hitablelist_hit(HitableList *h, const Ray *r, float t_min, float t_max, HitRecord *rec);

#endif
