#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <random>

#define MT_RENDER 1

using std::make_shared;
using std::shared_ptr;
using std::sqrt;
using std::fabs;

thread_local unsigned int seed;

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

#if MT_RENDER
    inline double random_double() {
        // Returns a random real in [0,1).
        return rand_r(&seed) / (RAND_MAX + 1.0);
    }
#else
inline double random_double() {
    // Returns a random real in [0,1).
    return rand() / (RAND_MAX + 1.0);
}
#endif

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max-min)*random_double();
}

#include "vec3.h"
#include "color.h"
#include "ray.h"
#include "interval.h"
#include "hittable.h"

#endif