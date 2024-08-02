#ifndef COLOR_H
#define COLOR_H

#include "utilities.h"

#include "interval.h"
#include "vec3.h"

using namespace std;

using color = vec3;

//ZGTODO What is gamut?
 inline double linear_to_gamma(double linear) {
    if(linear > 0)
        return sqrt(linear);
    
    return 0;
 }

void write_color(ostream& out, const color& pixel_color) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();
    
    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    static const Interval intensity(0.000, 0.999);
    int rbyte = int(256 * intensity.clamp(r));
    int gbyte = int(256 * intensity.clamp(g));
    int bbyte = int(256 * intensity.clamp(b));

    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

#endif