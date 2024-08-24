#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include "utilities.h"
#include "material.h"
#include "texture.h"

class Constant_Medium : public Hittable {
    
    public:
        Constant_Medium(shared_ptr<Hittable> boundary, double density, shared_ptr<Texture> tex) : 
            m_boundary(boundary), m_negInvDensity(-1/density), m_phaseFunction(make_shared<Isotropic>(tex)) {}
        
        Constant_Medium(shared_ptr<Hittable> boundary, double density,  const color& albedo) : 
            m_boundary(boundary), m_negInvDensity(-1/density), m_phaseFunction(make_shared<Isotropic>(albedo)) {}

        bool hit(const Ray& r, const Interval& ray_t, Hit_Record& rec) const override {
            Hit_Record rec1, rec2;

            //Does the ray hit the boundary
            if(!m_boundary->hit(r, Interval::universe, rec1))
                return false;

            //Is ray in the boundary? (???)
            //To confirm ray isnt on the border and exiting
            if(!m_boundary->hit(r, Interval(rec1.t+0.0001, infinity), rec2))
                return false;

            //Clamp bounds
            if(rec1.t < ray_t.min) rec1.t = ray_t.min;
            if(rec2.t > ray_t.max) rec2.t = ray_t.max;

            if (rec1.t >= rec2.t)
                return false;

            if(rec1.t < 0)
                rec1.t = 0;
            
            auto ray_length = r.direction().length();
            auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
            auto hit_distance = m_negInvDensity * std::log(random_double());
            
            if(hit_distance > distance_inside_boundary)
                return false;

            rec.t = rec1.t + hit_distance/ray_length;
            rec.p = r.at(rec.t);

            //Arbitrary
            rec.normal = vec3(1, 0, 0);
            rec.front_face = true;
            rec.mat = m_phaseFunction;

            return true;
        }

        AABB getBoundingBox() const override {
            return m_boundary->getBoundingBox();
        }

    private:
        shared_ptr<Hittable> m_boundary;
        double m_negInvDensity;
        shared_ptr<Material> m_phaseFunction;
};

#endif