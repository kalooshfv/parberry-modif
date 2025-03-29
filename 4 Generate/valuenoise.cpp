/// \file valuenoise.cpp
/// \brief Code for the designer world class CDesignerWorld.

//Copyright Ian Parberry, November 2013.
//
//This file is made available under the GNU All-Permissive License.
//
//Copying and distribution of this file, with or without
//modification, are permitted in any medium without royalty
//provided the copyright notice and this notice are preserved.
//This file is offered as-is, without any warranty.

#include <stdlib.h>
#include <stdio.h>
#include <cmath>

#include "valuenoise.h"
#include "simplexnoise.h"

SimplexNoise simplexNoise;

/// Initialize the permutation table.
void CDesignerWorld::Initialize(){
  //start with identity permutation
  for(int i=0; i<SIZE; i++)
    m_nPermute[i] = i;

  //randomize 
  for(int i=SIZE-1; i>0; i--){
    int j = rand() % (i + 1); //note the fix to Perlin's original code
    int temp = m_nPermute[i];
    m_nPermute[i] = m_nPermute[j];
    m_nPermute[j] = temp;
  } //for
} //Initialize

/// Set the value table.
/// \param table Table of values.
/// \param n Size of table.

void CDesignerWorld::SetValueTable(int table[], const int n){
  //check that the values add up to 256
  int sum = 0;
  for(int i=0; i<n; i++)
    sum += table[i];
  if(sum != SIZE){
    printf("Height distribution values must sum to %d, not %d\n", SIZE, sum);
    return; //fail and bail
  } //if

  //fill the table
  float delta = 2.0f/(float)(n-1); //interval size
  float min = -1.0f; //lower limit if interval
  int k = 0; //fill in m_fPosition[k]
  for(int i=0; i<n-1; i++){
    for(int j=0; j<table[i]; j++)
      m_fPosition[k++] = min + delta * (float)rand()/(float)RAND_MAX;
    min += delta;
  } //for

  //missed the largest values, get them now
  for(int j=0; j<table[n-1]; j++)
    m_fPosition[k++] = 1.0f;
  POINTSIZE = n;

  // simplexNoise.mapToGrid(m_fPosition);
} //SetValueTable

/// Get random height value at a point in the terrain.
/// Computes 1/f noise using the sample table.
/// \param x X coordinate of point at which to sample
/// \param z Z coordinate of point at which to sample
/// \param a Frequency
/// \param b Lacunarity
/// \param n Number of octaves
/// \return Height value between 0.0 and 1.0

float CDesignerWorld::GetHeight(float x, float z, float a, float b, int n, float simplexWeight = 0.0) {
  float result = 0.0f; //resulting height
  float scale = 1.0f; //scale of next octave
  
  for (int i = 0; i < n; i++) { //for each octave 
    result += scale * (SimplexNoise::noise(x, z) * simplexWeight + value_noise(x,z) * (1 - simplexWeight));
    // result += scale * value_noise(x, z);
    scale *= a; //scale down amplitude of next octave
    x *= b; z *= b; //scale down wavelength of next octave
  } //for

  return (1.0f + result * 1.414213f * (a - 1.0f)/(scale - 1.0f))/2.0f; //scale to [0.0, 1.0]
}

/// Get random height value for a single octave at a point in the terrain.
/// This is Value Noise. If you understand Perlin Noise, then this code will
/// be hauntingly familiar except for the fact that it uses position instead
/// of gradient.
/// \param x X coordinate of point at which to sample
/// \param z Z coordinate of point at which to sample
/// \return Height value between -1.0 and 1.0
float CDesignerWorld::value_noise(float x, float z) {
    const int dx = (int)x;
    const float sx = s_curve(x - dx);
    const int i = m_nPermute[MASK & dx];
    const int j = m_nPermute[MASK & (dx + 1)];

    const int dz = (int)z;
    const float sz = s_curve(z - dz);

    // Compute Hermite interpolation coefficients
    const float sx1 = 3 * sx * sx - 2 * sx * sx * sx;
    const float sx2 = sx * sx * (sx - 1);
    const float sz1 = 3 * sz * sz - 2 * sz * sz * sz;
    const float sz2 = sz * sz * (sz - 1);

    // Perform Hermite interpolation in x direction
    const float u0 = m_fPosition[m_nPermute[(i + dz) & MASK]];
    const float u1 = m_fPosition[m_nPermute[(j + dz) & MASK]];
    const float v0 = u0 + sx1 * (u1 - u0);
    const float u2 = m_fPosition[m_nPermute[(i + dz + 1) & MASK]];
    const float u3 = m_fPosition[m_nPermute[(j + dz + 1) & MASK]];
    const float v1 = u2 + sx1 * (u3 - u2);

    // Perform Hermite interpolation in z direction
    const float v0z = v0 + sz1 * (v1 - v0);
    const float v1z = v0 + sz1 * (v1 - v0);

    // Perform final Hermite interpolation
    return v0z + sx2 * (v1z - v0z);
}
