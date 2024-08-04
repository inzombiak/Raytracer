#ifndef RAY_H
#define RAY_H

#include "vec3.h"

class Ray {
    public:
        Ray () {};
        Ray (const point3& origin, const vec3& direction, double time) : m_orig(origin), m_dir(direction), m_time(time) {};
        Ray (const point3& origin, const vec3& direction) : m_orig(origin), m_dir(direction), m_time(0) {};

        const point3& origin() const {return m_orig;}
        const vec3& direction() const {return m_dir;}
        double time() const {return m_time;}

        point3 at(double t) const {
            return m_orig + t * m_dir;
        }



    private:
        point3 m_orig;
        vec3 m_dir;
        double m_time;
};

#endif