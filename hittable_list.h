#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"
#include "rtweekend.h"

#include <vector>

using std::shared_ptr;
using std::make_shared;

class hittable_list : public hittable {
    public:
        std::vector<shared_ptr<hittable>> objects;

        hittable_list() {};
        hittable_list(shared_ptr<hittable> obj) {
            add(obj);
        }

        void add(shared_ptr<hittable> obj) {
            objects.push_back(obj);
        }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            hit_record record;
            bool hit_anything = false;
            double closest_so_far = ray_t.max;

            for(const shared_ptr<hittable>& obj : objects) {
                if(obj->hit(r, interval(ray_t.min, closest_so_far), record)) {
                    hit_anything = true;
                    rec = record;
                    closest_so_far = rec.t;
                }   
            }

            return hit_anything;
        }
};

#endif