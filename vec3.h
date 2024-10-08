#pragma once

#include "utilities.h"

class vec3 {
  public:
    double e[3];

    vec3() : e{0,0,0} {}
    vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}
    vec3(double v) : e{v, v, v} {}; 

    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

    vec3 operator-() const {return vec3(-e[0], -e[1], -e[2]);}
    double operator[](int i) const {return e[i];}
    double& operator[](int i) { return e[i]; }

    vec3& operator+=(const vec3& v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }
    
    vec3& operator*=(double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    vec3& operator/=(double t) {
        return *this *= 1/t;
    }

    double length() const {
        return sqrt(length_squared());
    }

    double length_squared() const {
        return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
    }

    bool near_zero() const {
        double eps = 1e-8;
        return (fabs(e[0]) < eps) && (fabs(e[1]) < eps) && (fabs(e[2]) < eps);
    }

    static vec3 random() {
        return vec3(random_double(), random_double(), random_double());
    }

    static vec3 random(double min, double max) {
        return vec3(random_double(min, max), random_double(min, max),random_double(min, max));
    }

    static inline vec3 up() {
        return vec3(0, 1, 0);
    } 

    static inline vec3 forward() {
        return vec3(0, 0, 1);
    } 

    static inline vec3 right() {
        return vec3(1, 0, 0);
    }
};

using point3 = vec3;

inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3& u, const vec3& v) {
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator-(const vec3& u, const vec3& v) {
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3& u, const vec3& v) {
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator/(const vec3& u, const vec3& v) {
    return vec3(u[0]/v[0], u[1]/v[1], u[2]/v[2]);
}

inline vec3 operator*(double t, const vec3& v) {
    return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline vec3 operator*(const vec3& v, double t) {
    return t * v;
}

inline vec3 operator/(const vec3& v, double t) {
    return (1/t) * v;
}

inline double dot(const vec3& u, const vec3& v) {
    return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3& u, const vec3& v) {
    return vec3(u[1] * v[2] - u[2] * v[1],
                u[2] * v[0] - u[0] * v[2],
                u[0] * v[1] - u[1] * v[0]);
}

inline vec3 unit_vector(const vec3& v) {
    return v/v.length();
}

#include <cmath>
using std::min, std::max;
inline vec3 min(const vec3& u, const vec3& v)  {
    return vec3(min(u[0], v[0]), min(u[1], v[1]), min(u[2], v[2]));
}

inline vec3 max(const vec3& u, const vec3& v)  {
    return vec3(max(u[0], v[0]), max(u[1], v[1]), max(u[2], v[2]));
}

inline vec3 random_in_unit_sphere() {
    while(true) {
        vec3 p = vec3::random(-1, 1);
        if (p.length_squared() < 1)
            return p;
    }
}

inline vec3 random_in_unit_disk() {
    while(true) {
        vec3 p = vec3::random(-1, 1);
        p[2] = 0;
        if (p.length_squared()<1)
            return p;
    }
}

inline vec3 random_unit_vector() {
    return unit_vector(random_in_unit_sphere());
}

inline vec3 random_on_hemisphere(const vec3& normal) {
    vec3 ruv = random_unit_vector();

    if (dot(ruv, normal) > 0)
        return ruv;
    else 
        return -ruv;
}

inline vec3 reflect(const vec3& v, const vec3& normal) {
    return v - 2 * dot(v, normal) * normal;
}

//https://graphics.stanford.edu/courses/cs148-10-summer/docs/2006--degreve--reflection_refraction.pdf
inline vec3 refract(const vec3& v, const vec3& normal, double rel_eta) {
    double cosTheta = fmin(dot(-v, normal), 1.0);
    vec3 Rprep     = rel_eta * (normal * cosTheta + v);
    vec3 Rparallel = -normal * sqrt(fabs(1.0 - Rprep.length_squared())); 

    return Rprep + Rparallel;
}