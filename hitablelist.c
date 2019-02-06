#include "hitablelist.h"


void hitablelist_init(HitableList *h, Hitable **l, size_t size) {
    h->list = l;
    h->size = size;
}

bool hitablelist_hit(HitableList *h, const Ray *r, float t_min, float t_max, HitRecord *rec) {
    HitRecord tmp_rec = { 0 };
    bool hit_anything = false;
    float closest_so_far = t_max;

    for (size_t i = 0; i < h->size; i++) {
        Hitable *obj = h->list[i];
        if (obj->hit(obj, r, t_min, closest_so_far, &tmp_rec)) {
            hit_anything = true;
            closest_so_far = tmp_rec.t;
            *rec = tmp_rec;
        }
    }
    
    return hit_anything;
}
