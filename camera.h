#ifndef CAMERA_H
#define CAMERA_H

#define MT_RENDER 1

#include "rtweekend.h"

#include "hittable.h"
#include "material.h"

#include "thread_pool.h"

class camera {
  public:
    double aspect_ratio = 16.0/ 9.0;
    int image_width = 400;
    int samples_per_pixel = 10;
    int max_depth = 10;
    double vfov = 90;
    point3 lookfrom = point3(0,0,0); 
    point3 lookat   = point3(0,0,-1);
    vec3   vup      = vec3(0,1,0); 
    double defocus_angle = 0;  // Variation angle of rays through each pixel
    double focus_dist = 10;    // Distance from camera lookfrom point to plane of perfect focus

    void render(const hittable& world) {
        initialize();

        cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
#if MT_RENDER
        ray_world = &world;

        threadPool.start();
        mt_tex = vector<vector<color>>(image_height, vector<color>(image_width, color(0, 0, 0)));
        for (int j = 0; j < image_height; j++) {
            threadPool.queue_job([this](int i){ this->mt_render_row(i);},j);
        }
        threadPool.waitForCompletion();
        for (int i = 0; i < image_height; ++i) {
            for (int j = 0; j < image_width; ++j) {
                clog << mt_tex[i][j] << endl;
                write_color(std::cout, mt_tex[i][j]);
            }
        }
        threadPool.stop();
#else
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
#endif
        clog<<"\rDonezo!                    \n";
    }

  private:
    int image_height; 
    vec3 pixel_delta_u;
    vec3 pixel_delta_v;
    vec3 center;
    vec3 pixel00_loc;
    double pixel_sample_scale;
    vec3   u, v, w;     
    vec3   defocus_disk_u;       // Defocus disk horizontal radius
    vec3   defocus_disk_v;       // Defocus disk vertical radius
    const hittable* ray_world;
    vector<vector<color>> mt_tex;
    ThreadPool threadPool;

    void mt_render_row(int i) {
        for (int j = 0; j < image_width; j++) {
            color pixel_color(0, 0, 0);
            for (int samp = 0; samp < samples_per_pixel; ++samp) {
                ray r = get_ray(j, i);
                pixel_color += ray_color(r, max_depth, *ray_world);
            }
            pixel_color *= pixel_sample_scale;
            mt_tex[i][j] = pixel_color;
        }
    }
    
    void initialize() {
        image_height = int(image_width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;

        pixel_sample_scale = 1.0 / samples_per_pixel;

        center = lookfrom;

        // Determine viewport dimensions.
        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta/2);
        auto viewport_height = 2 * h * focus_dist;
        auto viewport_width = viewport_height * (double(image_width)/image_height);

        // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        vec3 viewport_u = viewport_width * u;    // Vector across viewport horizontal edge
        vec3 viewport_v = viewport_height * -v;  // Vector down viewport vertical edge

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left = center - (focus_dist * w) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
        
        auto defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    ray get_ray(int i, int j) {
        vec3 offset = sample_square();
        vec3 sameple_pos = pixel00_loc + (i + offset.x()) * pixel_delta_u + (j + offset.y()) * pixel_delta_v;
        auto ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
        vec3 ray_direction = sameple_pos - ray_origin;

        return ray(ray_origin, ray_direction);
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


    point3 defocus_disk_sample() const {
        // Returns a random point in the camera defocus disk.
        auto p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }
};

#endif