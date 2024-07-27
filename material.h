#ifndef MATERIAL_H
#define MATERIAL_H

#include "rtweekend.h"

class hit_record;

class material {
  public:
    virtual ~material() = default;

    virtual bool scatter( const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const {
        return false;
    }
};

class lambertian : public material {
    public:
        lambertian(const color& albedo) : albedo(albedo) {}

        bool scatter( const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const {
            auto scatter_direction = rec.normal + random_unit_vector();
            if (scatter_direction.near_zero())
                scatter_direction = rec.normal;
            scattered = ray(rec.p, scatter_direction);
            attenuation = albedo;

            return true;
        }

    private:
        color albedo;
};

class metal : public material {
    public:
        metal(const color& albedo, double fuzz = 0) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

        bool scatter( const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const {
            auto scatter_direction = reflect(r_in.direction(), rec.normal);
            vec3 fuzz_vec = random_unit_vector() * fuzz;
            scatter_direction = unit_vector(scatter_direction) + fuzz_vec;
            if (scatter_direction.near_zero())
                scatter_direction = rec.normal;
            scattered = ray(rec.p, scatter_direction);
            attenuation = albedo;

            return (dot(scattered.direction(), rec.normal) > 0);
        }

    private:
        color albedo;
        double fuzz;
};

class dielectric : public material {
    public:
        dielectric(double refraction_index) : refraction_index(refraction_index) {}

        bool scatter( const ray& r_in, const hit_record& rec, color& attenuation, ray& scattered) const {
            attenuation = color(1.0, 1.0, 1.0);

            double ri = rec.front_face ? (1.0/refraction_index) : refraction_index;

            vec3 unit_direction = unit_vector(r_in.direction());
            double cosTheta = fmin(dot(-unit_direction, rec.normal), 1.0);
            double sinTheta = sqrt(1 - cosTheta * cosTheta);
            
            bool can_refract = sinTheta * ri <= 1.0;
            vec3 direction;
            if (can_refract)
                direction = refract(unit_direction, rec.normal, ri);
            else
                direction = reflect(unit_direction, rec.normal);

            scattered = ray(rec.p, direction);
            return true;
        }

    private:
        double refraction_index;
};

#endif