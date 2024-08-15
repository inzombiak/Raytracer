#ifndef QUAD_H
#define QUAD_H

#include "utilities.h"

class Quad : public Hittable {
    public:
        Quad(const point3& Q, const vec3& u, const vec3& v, shared_ptr<Material> mat)
        : m_origin(Q), m_u(u), m_v(v), m_mat(mat)
        {
            vec3 n = cross(m_u, m_v);
            m_normal = unit_vector(n);
            m_D = dot(m_normal, m_origin);
            m_w = n / dot(n, n);
            setBoundingBox();
        }

        virtual void setBoundingBox() {
            // Compute the bounding box of all four vertices.
            auto bbox_diagonal1 = AABB(m_origin, m_origin + m_u + m_v);
            auto bbox_diagonal2 = AABB(m_origin + m_u, m_origin + m_v);
            m_aabb = AABB(bbox_diagonal1, bbox_diagonal2);
        }

        AABB getBoundingBox() const override { return m_aabb; }

        bool hit(const Ray& r, const Interval& ray_t, Hit_Record& rec) const override {
            vec3 dir = r.direction();
            double nDotDir = dot(m_normal, dir);
            if(fabs(nDotDir) < 1e-8)
                return false;

            
            double t = (m_D - dot(m_normal, r.origin()))/nDotDir;
            if(!ray_t.contains(t))
                return false;

            rec.p = r.at(t);

            vec3 p = rec.p - m_origin;
            double alpha = dot(m_w, cross(p, m_v));
            double beta  = dot(m_w, cross(m_u, p));
            if(alpha < 0 || beta < 0 ||
               alpha > 1 || beta > 1)
                return false;

            rec.t = t;
            rec.mat = m_mat;
            rec.set_face_normal(r, m_normal);
            rec.u = alpha;
            rec.v = beta;

            return true;
        }

    private:
        point3 m_origin;
        vec3 m_u, m_v;
        vec3 m_normal;
        vec3 m_w;
        //dot(normal, m_origin) = D
        double m_D;
        shared_ptr<Material> m_mat;
        AABB m_aabb;
};

#endif