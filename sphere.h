#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "rtweekend.h"
 
class sphere : public hittable {
    public:
        sphere(const point3& center, double radius) : center(center), radius(fmax(0, radius)) {};

        bool hit(const ray& r, const interval& ray_t, hit_record& rec) const override {

            vec3 oc = center - r.origin();
            double a = r.direction().length_squared();
            double h = dot(r.direction(), oc);
            double c = oc.length_squared() - radius * radius;

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
            vec3 out_norm = (rec.p - center)/radius;
            rec.set_face_normal(r, out_norm);

            return true;
        }

    private:
        point3 center;
        double radius;
};

#endif