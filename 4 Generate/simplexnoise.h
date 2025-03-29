#ifndef SIMPLEXNOISE_H
#define SIMPLEXNOISE_H

#include <array>

class SimplexNoise {
    public:
        static float noise(float xin, float yin);

    private:
        static int fastfloor(float x);
        static float dot(const int* g, float x, float y);
        static const std::array<int, 512> perm;

};

#endif // SIMPLEXNOISE_H