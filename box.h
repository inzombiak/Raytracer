#ifndef BOX_H
#define BOX_H

#include "hittable.h"
#include "rtweekend.h"
 
class box : public hittable {
    public:
        box(const point3& center, const vec3& half_extents, shared_ptr<material> mat) : center(center), half_extents(half_extents), mat(mat) {};

        bool contains(const point3& p) const {
            vec3 box_min = center - half_extents;
            vec3 box_max = center + half_extents;
            return (p.x() >= box_min.x() && p.x() <= box_max.x() &&
                    p.y() >= box_min.y() && p.y() <= box_max.y() &&
                    p.z() >= box_min.z() && p.z() <= box_max.z());
        }

        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            double tmin = numeric_limits<double>::lowest(), tmax = numeric_limits<double>::max();
            bool is_inside = contains(r.origin());
            vec3 ndir = (r.direction());           
            vec3 dirfrac;
            dirfrac[0] = 1.0f / ndir[0];
            dirfrac[1] = 1.0f / ndir[1];
            dirfrac[2] = 1.0f / ndir[2];

            // lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
            // r.org is origin of ray
            vec3 lb = center - half_extents;
            vec3 rt = center + half_extents; 
            double t1 = (lb[0] - r.origin()[0])*dirfrac[0];
            double t2 = (rt[0] - r.origin()[0])*dirfrac[0];
            double t3 = (lb[1] - r.origin()[1])*dirfrac[1];
            double t4 = (rt[1] - r.origin()[1])*dirfrac[1];
            double t5 = (lb[2] - r.origin()[2])*dirfrac[2];
            double t6 = (rt[2] - r.origin()[2])*dirfrac[2];

            tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
            tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

            // if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
            if (tmax < 0)
            {
                return false;
            }

            // if tmin > tmax, ray doesn't intersect AABB
            if (tmin > tmax)
            {
                return false;
            }
            
            double root = tmin;
            if(!ray_t.surrounds(t1)) {
                if (!ray_t.surrounds(t2)) 
                    return false;
                else
                    root = t2;
            } 
            rec.t = root;
            rec.p = r.at(rec.t);
            vec3 out_norm = (rec.p - center);
            if (abs(out_norm.x()) > abs(out_norm.y()) && abs(out_norm.x()) > abs(out_norm.z())) {
                out_norm[1] = 0;
                out_norm[2] = 0;
            } else if (abs(out_norm.y()) > abs(out_norm.x()) && abs(out_norm.y()) > abs(out_norm.z())) {
                out_norm[0] = 0;
                out_norm[2] = 0;
            } else {
                out_norm[0] = 0;
                out_norm[1] = 0;
            }
            out_norm = unit_vector(out_norm);
            rec.set_face_normal(r, out_norm);
            rec.mat = mat;

            return true;
        }

    private:
        point3 center;
        vec3 half_extents;
        shared_ptr<material> mat;
};

#endif