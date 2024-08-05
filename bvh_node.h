#ifndef BVH_NODE_H
#define BVH_NODE_H

#include "utilities.h"
#include "hittable_list.h"

#include <algorithm>

class BVH_Node : public Hittable {
    public:
        BVH_Node(Hittable_List hitlist) : BVH_Node(hitlist.objects, 0, hitlist.objects.size()) {};
        BVH_Node(vector<shared_ptr<Hittable>>& objects, size_t start, size_t end) {
            m_aabb = AABB::empty;
            for(size_t idx = start; idx < end; ++idx)
                m_aabb = AABB(m_aabb, objects[idx]->getBoundingBox());

            size_t objCount = end - start;
            if(objCount == 1) {
                m_left = m_right = objects[start];
            } else if (objCount == 2) {
                m_left  = objects[start];
                m_right = objects[start + 1];
            } else {
                int splitAxis = m_aabb.longestAxis();

                auto comperator = (splitAxis == 0) ? compareAABB_x :
                                  (splitAxis == 1) ? compareAABB_y :
                                                     compareAABB_z;
                                                     

                std::sort(objects.begin() + start, objects.begin() + end, comperator);
                m_left =  make_shared<BVH_Node>(objects, start, start + objCount / 2);
                m_right = make_shared<BVH_Node>(objects, start + objCount/2, end);
            }
        }

        bool hit(const Ray& r, const Interval& ray_t, Hit_Record& rec) const override {
            if(!m_aabb.hit(r, ray_t))
                return false;

            bool hit_left = m_left->hit(r, ray_t, rec);
            bool hit_right = m_right->hit(r, Interval(ray_t.min, hit_left ? rec.t : ray_t.max), rec);

            return hit_left || hit_right;
        }

        AABB getBoundingBox() const override {
            return m_aabb;
        }
    private:
        static bool compareAABB(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b, int axis) {
            return a->getBoundingBox().axisBounds(axis).min < b->getBoundingBox().axisBounds(axis).min;
        }
        
        static bool compareAABB_x(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b) {
            return compareAABB(a, b, 0);
        }
        static bool compareAABB_y(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b) {
            return compareAABB(a, b, 1);
        }
        static bool compareAABB_z(const shared_ptr<Hittable> a, const shared_ptr<Hittable> b) {
            return compareAABB(a, b, 2);
        }


        shared_ptr<Hittable> m_left;
        shared_ptr<Hittable> m_right;
        AABB m_aabb;
};

#endif