#include "SimplexNoise.h"
#include <cmath>
#include <iostream>
#include <algorithm>
#include <random>
#include <array>
#include <math.h>
#include <numeric>

// Generage permutation table
std::array<int, 512> generatePermutationTable() {
    std::array<int, 256> perm;
    std::iota(perm.begin(), perm.end(), 0); // Fill with values from 0 to 255

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(perm.begin(), perm.end(), g);

    std::array<int, 512> permTable;
    for (int i = 0; i < 512; ++i) {
        permTable[i] = perm[i % 256];
    }

    return permTable;
}
const std::array<int, 512> SimplexNoise::perm = generatePermutationTable();

// Helpers
int SimplexNoise::fastfloor(float x) {
    return x > 0 ? (int)x : (int)x - 1; }

float SimplexNoise::dot(const int* g, float x, float y) {
    return g[0] * x + g[1] * y; }

// Based on pseudocode found in Stefan Gustavson's "Simplex noise demystified"
// https://cgvr.cs.uni-bremen.de/teaching/cg_literatur/simplexnoise.pdf
float SimplexNoise::noise(float xin, float yin) {

    int grad3[12][3] = {
        (1,1,0),(-1,1,0),(1,-1,0),(-1,-1,0),
        (1,0,1),(-1,0,1),(1,0,-1),(-1,0,-1),
        (0,1,1),(0,-1,1),(0,1,-1),(0,-1,-1)        
    };

    float n0, n1, n2;
    const float F2 = 0.5f * (sqrt(3.0f) - 1.0f);
    float s = (xin + yin) * F2;
    int i = fastfloor(xin + s);
    int j = fastfloor(yin + s);
    const float G2 = (3.0f - sqrt(3.0f)) / 6.0f;
    float t = (i + j) * G2;
    float X0 = i - t;
    float Y0 = j - t;
    float x0 = xin - X0;
    float y0 = yin - Y0;
    int i1, j1;
    if (x0 > y0) { i1 = 1; j1 = 0; } else { i1 = 0; j1 = 1; }
    float x1 = x0 - i1 + G2;
    float y1 = y0 - j1 + G2;
    float x2 = x0 - 1.0f + 2.0f * G2;
    float y2 = y0 - 1.0f + 2.0f * G2;
    int ii = i & 255;
    int jj = j & 255;
    int gi0 = perm[ii + perm[jj]] % 12;
    int gi1 = perm[ii + i1 + perm[jj + j1]] % 12;
    int gi2 = perm[ii + 1 + perm[jj + 1]] % 12;
    float t0 = 0.5f - x0 * x0 - y0 * y0;
    if (t0 < 0) n0 = 0.0f;
    else {
        t0 *= t0;
        n0 = t0 * t0 * dot(grad3[gi0], x0, y0);
    }
    float t1 = 0.5f - x1 * x1 - y1 * y1;
    if (t1 < 0) n1 = 0.0f;
    else {
        t1 *= t1;
        n1 = t1 * t1 * dot(grad3[gi1], x1, y1);
    }
    float t2 = 0.5f - x2 * x2 - y2 * y2;
    if (t2 < 0) n2 = 0.0f;
    else {
        t2 *= t2;
        n2 = t2 * t2 * dot(grad3[gi2], x2, y2);
    }
    float noiseValue = 70.0f * (n0 + n1 + n2);
    return noiseValue;
}