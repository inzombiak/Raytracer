#ifndef PERLIN_H
#define PERLIN_H

#include "utilities.h"

class Perlin {

    public:
        Perlin() {
            for(int i = 0; i < POINT_COUNT; ++i) {
                m_randVec[i] = unit_vector(random_double(-1, 1));
            }

            perlinGeneratePerm(m_permX);
            perlinGeneratePerm(m_permY);
            perlinGeneratePerm(m_permZ);
        }

        double noise(const point3& p) const {
            //Get fracitonal parts of position
            auto u = p.x() - std::floor(p.x());
            auto v = p.y() - std::floor(p.y());
            auto w = p.z() - std::floor(p.z());

            //Get integer parts of position
            auto i = int(std::floor(p.x()));
            auto j = int(std::floor(p.y()));
            auto k = int(std::floor(p.z()));
            vec3 c[2][2][2];

                for (int di=0; di < 2; di++)
                    for (int dj=0; dj < 2; dj++)
                        for (int dk=0; dk < 2; dk++)
                            c[di][dj][dk] = m_randVec[
                                m_permX[(i+di) & 255] ^
                                m_permY[(j+dj) & 255] ^
                                m_permZ[(k+dk) & 255]
                            ];

                return perlinInterp(c, u, v, w);
        }

        double turb(const point3& p, int depth) const {
            double accum = 0.0;
            point3 temp_p = p;
            double weight = 1.0;
            while(depth > 0) {
                accum += noise(temp_p) * weight;
                weight *= 0.5;
                temp_p *= 2;
                --depth;
            }

            return std::fabs(accum);
        }

    private:
        static double perlinInterp(const vec3 c[2][2][2], double u, double v, double w) {
            auto uu = u*u*(3-2*u);
            auto vv = v*v*(3-2*v);
            auto ww = w*w*(3-2*w);
            auto accum = 0.0;

            for (int i=0; i < 2; i++)
                for (int j=0; j < 2; j++)
                    for (int k=0; k < 2; k++) {
                        vec3 weight_v(u-i, v-j, w-k);
                        accum += (i*uu + (1-i)*(1-uu))
                            * (j*vv + (1-j)*(1-vv))
                            * (k*ww + (1-k)*(1-ww))
                            * dot(c[i][j][k], weight_v);
                    }

            return accum;
        }

        static void perlinGeneratePerm(int* p) {
            for(int i = 0; i < POINT_COUNT; ++i) {
                p[i] = i;
            }

            permute(p, POINT_COUNT);
        }

        static void permute(int* p, int pointCount) {
            for (int i = pointCount-1; i > 0; i--) {
                int target = random_int(0, i);
                int tmp = p[i];
                p[i] = p[target];
                p[target] = tmp;
            }
        }
        static const int POINT_COUNT = 256;
        vec3 m_randVec[POINT_COUNT];
        int m_permX[POINT_COUNT];
        int m_permY[POINT_COUNT];
        int m_permZ[POINT_COUNT];

};

#endif