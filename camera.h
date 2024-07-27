#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"

#include "hittable.h"
#include "material.h"

class camera {
  public:
    double aspect_ratio = 16.0/ 9.0;
    int image_width = 400;
    int samples_per_pixel = 10;
    int max_depth = 10;

    void render(const hittable& world) {
        initialize();

        cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

        for (int j = 0; j < image_height; j++) {
            clog<<"\rScanline remaining: " << (image_height - j) << ' ' << flush;
            for (int i = 0; i < image_width; i++) {
                color pixel_color(0, 0, 0);
                for (int samp = 0; samp < samples_per_pixel; ++samp) {
                    ray r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth, world);
                }
                pixel_color *= pixel_sample_scale;
                write_color(std::cout, pixel_color);
            }
        }
        clog<<"\rDonezo!                    \n";
    }

  private:
    int image_height; 
    vec3 pixel_delta_u;
    vec3 pixel_delta_v;
    vec3 center;
    vec3 pixel00_loc;
    double pixel_sample_scale;

    void initialize() {
        // Image
        pixel_sample_scale = 1.0/double(samples_per_pixel);

        image_height = std::max(1, int(image_width/aspect_ratio));
        
        double focal_length = 1.0;
        float viewport_height = 2.0;
        float viewport_width  = viewport_height * ((double)image_width/image_height);

        center = vec3(0, 0, 0);
        vec3 viewport_u(viewport_width, 0, 0);
        vec3 viewport_v(0, -viewport_height, 0);

        pixel_delta_u = viewport_u/image_width;
        pixel_delta_v = viewport_v/image_height;

        auto viewport_upper_left = center
                                - vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + 0.5*(pixel_delta_u + pixel_delta_v);

    }

    ray get_ray(int i, int j) {
        vec3 offset = sample_square();
        vec3 sameple_pos = pixel00_loc + (i + offset.x()) * pixel_delta_u + (j + offset.y()) * pixel_delta_v;
        vec3 ray_direction = sameple_pos - center;

        return ray(center, ray_direction);
    }

    vec3 sample_square() {
        return vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    color ray_color(const ray& r,  int depth, const hittable& world) {
        if (depth <= 0) {
            return vec3(0, 0, 0);
        }

        hit_record record;
        if (world.hit(r, interval(0.001, infinity), record)){
            color att;
            ray scatter;
            if (record.mat->scatter(r, record, att, scatter)) {
                vec3 direction = record.normal + random_unit_vector();
                return att * ray_color(scatter, depth - 1, world);
            }
            return color(0, 0, 0);
        }
        vec3 unit = unit_vector(r.direction());

        double y = 0.5*(unit.y() + 1.0);
        return (1-y)*color(1, 1, 1) + y * color(0.5, 0.7, 1);
    }
};

#endif