#ifndef TEXTURE_H
#define TEXTURE_H

#include "utilities.h"

class Texture {
    public:
        virtual ~Texture() = default;

        virtual color value(double u, double v, const point3& p) const = 0;
};

class BasicTexture : public Texture {
    public:
        BasicTexture(const color& albedo) : m_albedo(albedo) {};
        BasicTexture(double r, double g, double b) : m_albedo(r, g, b) {};

        color value(double u, double v, const point3& p) const override {
            return m_albedo;
        }

    private:
        color m_albedo;
};

class CheckerTexture : public Texture {
    public:
        CheckerTexture(double scale, shared_ptr<Texture> evenTex, shared_ptr<Texture> m_oddTex) : 
                    m_invScale(1.0/scale), m_evenTex(evenTex), m_oddTex(m_oddTex) {}
        
        CheckerTexture(double scale, const color& even, const color& odd) :
            CheckerTexture(scale, make_shared<BasicTexture>(even), make_shared<BasicTexture>(odd)) {}

        color value(double u, double v, const point3& p) const override {
            int scaledX = std::floor(m_invScale * p.x());
            int scaledY = std::floor(m_invScale * p.y());
            int scaledZ = std::floor(m_invScale * p.z());

            bool isEven = (scaledX + scaledY + scaledZ) % 2 == 0;

            if(isEven)
                return m_evenTex->value(u, v, p);
            else
                return m_oddTex->value(u, v, p);
        }


    private:
        double m_invScale = 0;
        shared_ptr<Texture> m_evenTex;
        shared_ptr<Texture> m_oddTex;

};

#include "rtw_stb_image.h"

class ImageTexture : public Texture {
    public:
        ImageTexture(const char* filename) : m_image(filename) {}
    
        color value(double u, double v, const point3& p) const override {
            if (m_image.height() <= 0)
                return color(0, 1, 1);


            u = Interval(0,1).clamp(u);
            v = 1.0 - Interval(0,1).clamp(v);  // Flip V to image coordinates

            auto i = int(u * m_image.width());
            auto j = int(v * m_image.height());
            auto pixel = m_image.pixel_data(i,j);

            auto color_scale = 1.0 / 255.0;
            return color(color_scale*pixel[0], color_scale*pixel[1], color_scale*pixel[2]);
       }


    private:
        rtw_image m_image;

};

#include "perlin.h"

class NoiseTexture : public Texture {
    public:
        NoiseTexture(double scale) : m_scale(scale) {}

        color value(double u, double v, const point3& p) const override {        
            return color(.5, .5, .5) * (1 + std::sin(m_scale * p.z() + 10 * m_perlinNoise.turb(p, 7)));
        }

    private:
        double m_scale;
        Perlin m_perlinNoise;

};

class EmissiveNoiseTexture : public Texture {
    public:
        EmissiveNoiseTexture(double scale, double intensity) : m_scale(scale), m_intensity(intensity) {}

        color value(double u, double v, const point3& p) const override {        
            return m_intensity * color(.5, .5, .5) * (1 + std::sin(m_scale * p.z() + 10 * m_perlinNoise.turb(p, 7)));
        }

    private:
        double m_scale;
        double m_intensity;
        Perlin m_perlinNoise;

};



#endif