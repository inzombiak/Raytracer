#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "utilities.h"
 
class Sphere : public Hittable {
    public:
        Sphere(const point3& center, double radius, shared_ptr<Material> mat) : m_center0(center), m_radius(fmax(0, radius)), m_mat(mat), m_isMoving(false) {};
        Sphere(const point3& center0, const point3& center1, double radius, shared_ptr<Material> mat) : 
        m_center0(center0), m_radius(fmax(0, radius)), m_mat(mat) {
            m_centerVec = center1 - center0;
            m_isMoving = m_centerVec.length_squared() > 0;
        };

        bool hit(const Ray& r, const Interval& ray_t, Hit_Record& rec) const override {
            vec3 center = getSphereCenter(r.time());
            vec3 oc = center - r.origin();
            double a = r.direction().length_squared();
            double h = dot(r.direction(), oc);
            double c = oc.length_squared() - m_radius * m_radius;

            double det = h*h - a * c;
            if (det < 0.0)
                return false;
           
            double t1 = (h - sqrt(det)) / a;
            double t2 = (h + sqrt(det)) / a;
            
            double root = t1;

            if(!ray_t.surrounds(t1)) {
                if (!ray_t.surrounds(t2)) 
                    return false;
                else
                    root = t2;
            } 

            rec.t = root;
            rec.p = r.at(rec.t);
            vec3 out_norm = (rec.p - center)/m_radius;
            rec.set_face_normal(r, out_norm);
            rec.mat = m_mat;

            return true;
        }

    private:
        point3 getSphereCenter(double time) const {
            if (!m_isMoving)
                return m_center0;
            
            return m_center0 + m_centerVec * time;
        }


        point3 m_center0;
        bool m_isMoving;
        vec3 m_centerVec;
        double m_radius;
        shared_ptr<Material> m_mat;
};

#endif