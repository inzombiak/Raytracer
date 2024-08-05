#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittable.h"
#include "utilities.h"

#include <vector>

using std::shared_ptr;
using std::make_shared;

class Hittable_List : public Hittable {
    public:
        std::vector<shared_ptr<Hittable>> objects;

        Hittable_List() {};
        Hittable_List(shared_ptr<Hittable> obj) {
            add(obj);
        }

        void add(shared_ptr<Hittable> obj) {
            m_aabb = AABB(m_aabb, obj->getBoundingBox());
            objects.push_back(obj);
        }

        bool hit(const Ray& r, const Interval& ray_t, Hit_Record& rec) const override {
            Hit_Record record;
            bool hit_anything = false;
            double closest_so_far = ray_t.max;

            for(const shared_ptr<Hittable>& obj : objects) {
                if(obj->hit(r, Interval(ray_t.min, closest_so_far), record)) {
                    hit_anything = true;
                    rec = record;
                    closest_so_far = rec.t;
                }   
            }

            return hit_anything;
        }

        AABB getBoundingBox() const override {
            return m_aabb;
        }

    private:
        AABB m_aabb;
};

#endif