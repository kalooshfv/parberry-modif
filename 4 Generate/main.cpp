/// \file main.cpp

//Copyright Ian Parberry, November 2013.
//
//This file is made available under the GNU All-Permissive License.
//
//Copying and distribution of this file, with or without
//modification, are permitted in any medium without royalty
//provided the copyright notice and this notice are preserved.
//This file is offered as-is, without any warranty.

#include <windows.h>
#include <MMSystem.h>
#include <stdio.h>
#include <conio.h>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <random>

#include "valuenoise.h"
#include "simplexnoise.h"

int NUMOCTAVES = 8; //number of octaves of 1/f noise
float WEIGHT = 0.0;
float LACUNARITY = 0.5f;
float AMPLITUDE = 2.0f;

CDesignerWorld g_cDesignerWorld;

struct Data {
    int originalDistributionCount;
    int distributionCount;
    int pointsSize;
    int minAltitude;
    int maxAltitude;
    std::vector<int> distribution;
    std::vector<std::pair<int, int>> points;

    void print() const {
        std::cout << "DistributionCount: " << distributionCount << std::endl;
        std::cout << "Distribution: ";
        for (int num : distribution) {
            std::cout << num << " ";
        }
        std::cout << std::endl;

        std::cout << "PointsSize: " << pointsSize << std::endl;
        std::cout << "Points: ";
        for (const auto& p : points) {
            std::cout << "(" << p.first << ", " << p.second << ") ";
        }
        std::cout << std::endl;
        std::cout << "MinAltitude: " << minAltitude << std::endl;
        std::cout << "MaxAltitude: " << maxAltitude << std::endl;
    }
};

Data readDataFromFile(const std::string& filename) {
    std::ifstream file(filename);
    Data data;
    std::string line, key;

    if (!file.is_open()) {
        std::cerr << "Failed to open file\n";
        return data;
    }

    while (getline(file, line)) {
        std::istringstream iss(line);
        iss >> key;
        if (key == "DistributionCount") {
            iss >> data.originalDistributionCount;
        } else if (key == "Distribution") {
            int value;
            int count = 0;
            while (iss >> value) {
                if (value > 0) {
                    data.distribution.push_back(value);
                    count++;
                }
            }
            data.distributionCount = count;
        } else if (key == "PointsSize") {
            iss >> data.pointsSize;
        } else if (key == "NewPoints") {
            std::string point;
            while (iss >> point) {
                size_t commaPos = point.find(',');
                int row = std::stoi(point.substr(0, commaPos));
                int col = std::stoi(point.substr(commaPos + 1));
                data.points.emplace_back(row, col);
            }
        } else if (key == "MinAltitude") {
            iss >> data.minAltitude;
        } else if (key == "MaxAltitude") {
            iss >> data.maxAltitude;
        }
    }

    file.close();
    return data;
}

int main(int argc, char *argv[]){ 
  std::string input_file = argv[1];
  int altitude = -1;
  if (argc > 2) WEIGHT = std::stof(argv[2]);
  if (argc > 3) altitude = atoi(argv[3]);
  if (argc > 4) NUMOCTAVES = atoi(argv[4]);
  if (argc > 5) LACUNARITY = std::stof(argv[5]);
  if (argc > 6) AMPLITUDE = std::stof(argv[6]);

  int seed = timeGetTime();
  srand(seed);
  printf("Pseudorandom number seed = %d\n", seed);

  Data d = readDataFromFile("..\\3 Analyze\\analyze.csv");

  if (altitude <= 0) { // set default altitude range if not provided
    altitude = 1.25 * d.maxAltitude;
  }

  printf("Running terrain generator with:\n"); 
  printf("simplexWeight: %0.2f, maxAltitude: %d, numOctaves: %d, lacunarity: %0.2f, amplitude: %0.2f\n", WEIGHT, altitude, NUMOCTAVES, LACUNARITY, AMPLITUDE);

  int distribution[d.distributionCount];
  std::copy(d.distribution.begin(), d.distribution.end(), distribution);
  
  //set up designer world
  g_cDesignerWorld.Initialize();
  g_cDesignerWorld.SetValueTable(distribution, d.distributionCount);

  char filename[MAX_PATH];
  sprintf_s(filename, "%s", argv[1]);
  FILE* output;
  fopen_s(&output, filename, "wt");
  
  fprintf(output, "Size %d\n", d.pointsSize);

  //generate and save grid heights to DEM file
  fprintf(output, "Points");
  for(std::pair<int, int> &p: d.points){
    fprintf(output, " %d,%d", p.first, p.second);
  }
  
  fprintf(output, "\n");

  //get random origin
  float x = (float)rand();
  float z = (float)rand();

  //generate and save grid heights to DEM file
  fprintf(output, "Values");

  int iter = 0;
  for(std::pair<int, int> &p: d.points){
    int i = p.first;
    int j = p.second;
    fprintf(output, " %0.2f", altitude *
        (1-g_cDesignerWorld.GetHeight(x + i/256.0f, z + j/256.0f, LACUNARITY, AMPLITUDE, NUMOCTAVES, WEIGHT)));
    if(iter%5000 == 0)printf(".");
    iter++;
  }

  //shut down and exit
  printf("\n");
  fclose(output);

  return 0;
}