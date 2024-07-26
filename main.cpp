#include "color.h"
#include "vec3.h"

#include <iostream>

using namespace std;
int main() {

    // Image
    auto aspect_ratio = 16.0/ 9.0;
    int image_width = 400;
    int image_height = max(1, int(image_width/aspect_ratio));
    
    float viewport_height = 2.0;
    float viewport_width  = viewport_height * ((double)image_width/image_height);

    // Render

    cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

    for (int j = 0; j < image_height; j++) {
        clog<<"\rScanline remaining: " << (image_height - j) << ' ' << flush;
        for (int i = 0; i < image_width; i++) {
            color pixel_color = color(double(i) / (image_width-1),
                                      double(j) / (image_height-1),
                                      0.0);

            write_color(cout, pixel_color);
        }
    }
    clog<<"\rDone!                    \n";
}