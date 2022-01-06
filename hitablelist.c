#include "xtiny_raytracer.h"

void hitableListInit(hitableList *h, hitable **l, size_t size) {
    h->list = l;
    h->size = size;
}

bool hitableListHit(const hitableList *h, const ray *r, float t_min, float t_max, hitRecord *rec) {
    hitRecord tmp_rec = { 0 };
    bool hit_anything = false;
    float closest_so_far = t_max;

    for (size_t i = 0; i < h->size; i++) {
        hitable *obj = h->list[i];
        if (obj->hit(obj, r, t_min, closest_so_far, &tmp_rec)) {
            hit_anything = true;
            closest_so_far = tmp_rec.t;
            *rec = tmp_rec;
        }
    }
    
    return hit_anything;
}
