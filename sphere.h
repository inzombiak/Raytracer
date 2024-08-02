#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "utilities.h"
 
class Sphere : public Hittable {
    public:
        Sphere(const point3& center, double radius, shared_ptr<Material> mat) : m_center(center), m_radius(fmax(0, radius)), m_mat(mat) {};

        bool hit(const Ray& r, const Interval& ray_t, Hit_Record& rec) const override {

            vec3 oc = m_center - r.origin();
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
            vec3 out_norm = (rec.p - m_center)/m_radius;
            rec.set_face_normal(r, out_norm);
            rec.mat = m_mat;

            return true;
        }

    private:
        point3 m_center;
        double m_radius;
        shared_ptr<Material> m_mat;
};

#endif