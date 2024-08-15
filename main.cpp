#include "utilities.h"

#include "camera.h"
#include "hittable_list.h"
#include "sphere.h"
#include "box.h"
#include "quad.h"
#include "material.h"
#include "bvh_node.h"

void bouncingSpheres() {
    Hittable_List world;

    auto ground_material = make_shared<Lambertian>(make_shared<CheckerTexture>(1.0, color(0, 0.7, 0.7), color(0, 0, 0)));
    world.add(make_shared<Sphere>(point3(0,-1000,0), 1000, ground_material));

    for (int a = -6; a < 6; a++) {
        for (int b = -6; b < 6; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<Material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<Lambertian>(albedo);
                    point3 center1 = center + vec3::up() * random_double(0, 0.5);
                    world.add(make_shared<Sphere>(center, center1, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<Metal>(albedo, fuzz);
                    world.add(make_shared<Sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<Dielectric>(1.5);
                    world.add(make_shared<Sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<Dielectric>(1.5);
    world.add(make_shared<Sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<Lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<Sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<Metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<Box>(point3(4, 1, 0), vec3(1), material3));

    world = Hittable_List(make_shared<BVH_Node>(world));

    Camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 1000;
    cam.samples_per_pixel = 150;
    cam.max_depth         = 50;

    cam.vfov     = 20;
    cam.lookfrom = point3(13,2,3);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0.6;
    cam.focus_dist    = 10.0;

    cam.render(world);
}

void checkeredSpheres() {
    Hittable_List world;

    auto checker = make_shared<CheckerTexture>(0.32, color(.2, .3, .1), color(.9, .9, .9));

    world.add(make_shared<Sphere>(point3(0,-10, 0), 10, make_shared<Lambertian>(checker)));
    world.add(make_shared<Sphere>(point3(0, 10, 0), 10, make_shared<Lambertian>(checker)));

    Camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.vfov     = 20;
    cam.lookfrom = point3(13,2,3);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world);
}

void earth() {
    auto earth_texture = make_shared<ImageTexture>("earthmap.jpg");
    auto earth_surface = make_shared<Lambertian>(earth_texture);
    auto globe = make_shared<Sphere>(point3(0,0,0), 2, earth_surface);

    Camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.vfov     = 20;
    cam.lookfrom = point3(0,0,12);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(Hittable_List(globe));
}

void perlinSphere() {
    Hittable_List world;

    auto perlinTex = make_shared<NoiseTexture>(4);
    world.add(make_shared<Sphere>(point3(0, -1000, 0), 1000, make_shared<Lambertian>(perlinTex)));
    world.add(make_shared<Sphere>(point3(0, 2, 0), 2, make_shared<Lambertian>(perlinTex)));

    Camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.vfov     = 20;
    cam.lookfrom = point3(13,2,3);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world);
}

void quads() {
    Hittable_List world;

    // Materials
    auto left_red     = make_shared<Lambertian>(color(1.0, 0.2, 0.2));
    auto back_green   = make_shared<Lambertian>(color(0.2, 1.0, 0.2));
    auto right_blue   = make_shared<Lambertian>(color(0.2, 0.2, 1.0));
    auto upper_orange = make_shared<Lambertian>(color(1.0, 0.5, 0.0));
    auto lower_teal   = make_shared<Lambertian>(color(0.2, 0.8, 0.8));

    // Quads
    world.add(make_shared<Quad>(point3(-3,-2, 5), vec3(0, 0,-4), vec3(0, 4, 0), left_red));
    world.add(make_shared<Quad>(point3(-2,-2, 0), vec3(4, 0, 0), vec3(0, 4, 0), back_green));
    world.add(make_shared<Quad>(point3( 3,-2, 1), vec3(0, 0, 4), vec3(0, 4, 0), right_blue));
    world.add(make_shared<Quad>(point3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4), upper_orange));
    world.add(make_shared<Quad>(point3(-2,-3, 5), vec3(4, 0, 0), vec3(0, 0,-4), lower_teal));

    Camera cam;

    cam.aspect_ratio      = 1.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.vfov     = 80;
    cam.lookfrom = point3(0,0,9);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world);
}

int main() {
    switch(5) {
        case 1: bouncingSpheres();  break;
        case 2: checkeredSpheres(); break;
        case 3: earth();            break;
        case 4: perlinSphere();     break;
        case 5: quads();            break;
    }
}