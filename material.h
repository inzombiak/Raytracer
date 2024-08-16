#ifndef MATERIAL_H
#define MATERIAL_H

#include "utilities.h"
#include "texture.h"

class Hit_record;

class Material {
  public:
    virtual ~Material() = default;

    virtual bool scatter( const Ray& r_in, const Hit_Record& rec, color& attenuation, Ray& scattered) const {
        return false;
    }

    virtual color emitted(double u, double v, const point3& p) const {
        return color(0, 0, 0);
    }
};

class Lambertian : public Material {
    public:
        Lambertian(const color& albedo) : m_texture(make_shared<BasicTexture>(albedo)) {}
        Lambertian(shared_ptr<Texture> tex): m_texture(tex) {}

        bool scatter( const Ray& r_in, const Hit_Record& rec, color& attenuation, Ray& scattered) const {
            auto scatter_direction = rec.normal + random_unit_vector();
            if (scatter_direction.near_zero())
                scatter_direction = rec.normal;
            scattered = Ray(rec.p, scatter_direction, r_in.time());
            attenuation = m_texture->value(rec.u, rec.v, rec.p);

            return true;
        }

    private:
        shared_ptr<Texture> m_texture;
};

class Metal : public Material {
    public:
        Metal(const color& albedo, double fuzz = 0) : m_albedo(albedo), m_fuzz(fuzz < 1 ? fuzz : 1) {}

        bool scatter( const Ray& r_in, const Hit_Record& rec, color& attenuation, Ray& scattered) const {
            auto scatter_direction = reflect(r_in.direction(), rec.normal);
            vec3 fuzz_vec = random_unit_vector() * m_fuzz;
            scatter_direction = unit_vector(scatter_direction) + fuzz_vec;
            if (scatter_direction.near_zero())
                scatter_direction = rec.normal;
            scattered = Ray(rec.p, scatter_direction, r_in.time());
            attenuation = m_albedo;

            return (dot(scattered.direction(), rec.normal) > 0);
        }

    private:
        color m_albedo;
        double m_fuzz;
};

class Dielectric : public Material {
    public:
        Dielectric(double refraction_index) : m_refractionIndex(refraction_index) {}

        bool scatter( const Ray& r_in, const Hit_Record& rec, color& attenuation, Ray& scattered) const {
            attenuation = color(1.0, 1.0, 1.0);

            double ri = rec.front_face ? (1.0/m_refractionIndex) : m_refractionIndex;

            vec3 unit_direction = unit_vector(r_in.direction());
            double cosTheta = fmin(dot(-unit_direction, rec.normal), 1.0);
            double sinTheta = sqrt(1 - cosTheta * cosTheta);
            
            bool cannot_refract = sinTheta * ri > 1.0;
            vec3 direction;
            if (cannot_refract || reflectance(cosTheta, ri) > random_double())
                direction = reflect(unit_direction, rec.normal);
            else
                direction = refract(unit_direction, rec.normal, ri);

            scattered = Ray(rec.p, direction, r_in.time());
            return true;
        }

        static double reflectance(double cosine, double refraction_index) {
            // Use Schlick's approximation for reflectance.
            auto r0 = (1 - refraction_index) / (1 + refraction_index);
            r0 = r0*r0;
            return r0 + (1-r0)*pow((1 - cosine),5);
        }

    private:
        double m_refractionIndex;
};

class Emissive : public Material {
    public:
        Emissive(shared_ptr<Texture> tex): m_tex(tex) {}
        Emissive(const color& emittion) : m_tex(make_shared<BasicTexture>(emittion)) {}

        color emitted(double u, double v, const point3& p) const override {
            return m_tex->value(u, v, p);
        }
    private:
        shared_ptr<Texture> m_tex;
};

#endif