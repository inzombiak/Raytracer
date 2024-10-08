#ifndef BOX_H
#define BOX_H

#include "hittable.h"
#include "utilities.h"
 
class Box : public Hittable, public AABB {
    public:
        Box(const point3& center, const vec3& half_extents, shared_ptr<Material> mat) : m_mat(mat) {
            m_boxMin = center - half_extents;
            m_boxMax = center + half_extents;
        };

        bool hit(const Ray& r, const Interval& ray_t, Hit_Record& rec) const override {
            double tmin = numeric_limits<double>::lowest(), tmax = numeric_limits<double>::max();
            vec3 ndir = (r.direction());           
            vec3 dirfrac;
            dirfrac[0] = 1.0f / ndir[0];
            dirfrac[1] = 1.0f / ndir[1];
            dirfrac[2] = 1.0f / ndir[2];

            double t1 = (m_boxMin[0] - r.origin()[0])*dirfrac[0];
            double t2 = (m_boxMax[0] - r.origin()[0])*dirfrac[0];
            double t3 = (m_boxMin[1] - r.origin()[1])*dirfrac[1];
            double t4 = (m_boxMax[1] - r.origin()[1])*dirfrac[1];
            double t5 = (m_boxMin[2] - r.origin()[2])*dirfrac[2];
            double t6 = (m_boxMax[2] - r.origin()[2])*dirfrac[2];

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
            if(!ray_t.surrounds(tmin)) {
                if (!ray_t.surrounds(tmax)) 
                    return false;
                else
                    root = tmax;
            } 
            rec.t = root;
            rec.p = r.at(rec.t);
            vec3 out_norm = (rec.p - (m_boxMin + m_boxMax)*0.5);
            double dotX = abs(dot(vec3::right(), out_norm));
            double dotY = abs(dot(vec3::up(), out_norm));
            double dotZ = abs(dot(vec3::forward(), out_norm));
            
            if(root == t1)
                out_norm = -vec3::right();
            else if (root == t2)
                out_norm = vec3::right();
            else if(root == t3)
                out_norm = -vec3::up();
            else if (root == t4)
                out_norm = vec3::up();
            else if(root == t5)
                out_norm = -vec3::forward();
            else if (root == t6)
                out_norm = vec3::forward();

            out_norm = unit_vector(out_norm);
            rec.set_face_normal(r, out_norm);
            rec.mat = m_mat;
            rec.u = 1;
            rec.v = 1;
            return true;
        }

        AABB getBoundingBox() const override {
            return AABB(m_boxMin, m_boxMax);
        }

    private:
        shared_ptr<Material> m_mat;
};

#endif