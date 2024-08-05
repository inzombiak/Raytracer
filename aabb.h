#ifndef AABB_H
#define AABB_H

#include "utilities.h"

class AABB {

public:
    AABB() {};
    AABB(const point3& minima, const point3& maxima) : m_boxMin(minima), m_boxMax(maxima) {}
    AABB(const Interval& x, const Interval& y, const Interval& z) {
        m_boxMin = point3(x.min, y.min, z.min);
        m_boxMax = point3(x.max, y.max, z.max);
    };
    AABB(const AABB& a, const AABB& b) {
        m_boxMin = min(a.m_boxMin, b.m_boxMin);
        m_boxMax = max(a.m_boxMax, b.m_boxMax);
    }

    int longestAxis() const {
        vec3 span = m_boxMax - m_boxMin;
        if (span[0] > span[1]) 
            return span[0] > span[2] ? 0 : 2;
        else
            return span[1] > span[2] ? 1 : 2;
    }

    Interval axisBounds(int n) {
        if (n == 1)
            return Interval(m_boxMin.y(), m_boxMax.y());
        else if (n == 2)
            return Interval(m_boxMin.z(), m_boxMax.z());
        return Interval(m_boxMin.x(), m_boxMax.x());
    }

    bool hit(const Ray& r, Interval ray_t) const {
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

        return true;
    }

    static const AABB empty, universe;
protected:
    point3 m_boxMin, m_boxMax;

};

const AABB AABB::empty    = AABB(Interval::empty, Interval::empty, Interval::empty);
const AABB AABB::universe = AABB(Interval::universe, Interval::universe, Interval::universe);

#endif