#ifndef HITTABLE_H
#define HITTABLE_H

#include "utilities.h"
#include "aabb.h"

class Material;

class Hit_Record {
    public:
        point3 p;
        vec3 normal;
        shared_ptr<Material> mat;
        double t;
        double u;
        double v;
        bool front_face = false;
    
        void set_face_normal(const Ray& r, const vec3& outward_normal) {
            front_face = dot(r.direction(), outward_normal) < 0;
            normal = front_face ? outward_normal : -outward_normal;
        }
};

class Hittable {
  public:
    virtual ~Hittable() = default;

    virtual bool hit(const Ray& r, const Interval& ray_t, Hit_Record& rec) const = 0;

    virtual AABB getBoundingBox() const = 0;
};

class Translate : public Hittable {
    public:
        Translate(shared_ptr<Hittable> object, const vec3& offset) : m_object(object), m_offset(offset) {
            m_bbox = m_object->getBoundingBox() + offset;
        }

        bool hit(const Ray& r, const Interval& ray_t, Hit_Record& rec) const {
            Ray offset_ray(r.origin() - m_offset, r.direction(), r.time());

            if(!m_object->hit(offset_ray, ray_t, rec))
                return false;

            rec.p += m_offset;

            return true;
        }

        AABB getBoundingBox() const {
            return m_bbox;
        }

    private:
        shared_ptr<Hittable> m_object;
        vec3 m_offset;    
        AABB m_bbox;    

};

class Rotate_Y : public Hittable {
    public:
        Rotate_Y(shared_ptr<Hittable> object, double angle) : m_object(object) {
            auto rad = degrees_to_radians(angle);
            m_sinTheta = std::sin(rad);
            m_cosTheta = std::cos(rad);
            m_bbox = m_object->getBoundingBox();

            point3 min = point3(infinity, infinity, infinity);
            point3 max = point3(-infinity, -infinity, -infinity);
            
            for(int i = 0; i < 2; ++i) {
                for(int j = 0; j < 2; ++j) {
                    for(int k = 0; k < 2; ++k) {
                        auto x = i * m_bbox.m_boxMax.x() + (i-1)*m_bbox.m_boxMin.x();
                        auto y = j * m_bbox.m_boxMax.y() + (j-1)*m_bbox.m_boxMin.y();
                        auto z = k * m_bbox.m_boxMax.z() + (k-1)*m_bbox.m_boxMin.z();
                        
                        auto newX = m_cosTheta*x + m_sinTheta*z;
                        auto newZ = -m_sinTheta*x + m_cosTheta*z;
                        
                        vec3 tester(newX, y, newZ);
                        for(int c = 0; c < 3; ++c) {
                            min[c] = std::fmin(min[c], tester[c]);
                            max[c] = std::fmax(max[c], tester[c]);
                        }
                    }
                }
            }

            m_bbox = AABB(min, max);
        }

        bool hit(const Ray& r, const Interval& ray_t, Hit_Record& rec) const {
             // Change the ray from world space to object space
            auto origin = r.origin();
            auto direction = r.direction();

            origin[0] = m_cosTheta*r.origin()[0] - m_sinTheta*r.origin()[2];
            origin[2] = m_sinTheta*r.origin()[0] + m_cosTheta*r.origin()[2];

            direction[0] = m_cosTheta*r.direction()[0] - m_sinTheta*r.direction()[2];
            direction[2] = m_sinTheta*r.direction()[0] + m_cosTheta*r.direction()[2];

            Ray rotated_r(origin, direction, r.time());

            // Determine whether an intersection exists in object space (and if so, where)
            if (!m_object->hit(rotated_r, ray_t, rec))
                return false;

            // Change the intersection point from object space to world space
            auto p = rec.p;
            p[0] =  m_cosTheta*rec.p[0] + m_sinTheta*rec.p[2];
            p[2] = -m_sinTheta*rec.p[0] + m_cosTheta*rec.p[2];

            // Change the normal from object space to world space
            auto normal = rec.normal;
            normal[0] =  m_cosTheta*rec.normal[0] + m_sinTheta*rec.normal[2];
            normal[2] = -m_sinTheta*rec.normal[0] + m_cosTheta*rec.normal[2];

            rec.p = p;
            rec.normal = normal;

            return true;
        }

        AABB getBoundingBox() const override {
            return m_bbox;
        } 

    private:
        shared_ptr<Hittable> m_object;
        double m_sinTheta;
        double m_cosTheta;
        AABB m_bbox;
};

#endif