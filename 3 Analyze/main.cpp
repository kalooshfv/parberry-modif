#include <windows.h>

#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <numeric>

std::vector<float> splitAndConvertToFloat(const std::string& str, char delimiter) {
    std::vector<float> values;
    std::stringstream ss(str);
    std::string token;
    while (getline(ss, token, delimiter)) {
        values.push_back(std::stof(token));
    }
    return values;
}

void readSegmentFile(const std::string& filename, int& newNSize, int& nSize, std::string& pointsRow, std::vector<float>& pixelValues) {
    std::ifstream file(filename);
    std::string line;

    if (file.is_open()) {
        std::getline(file, line);
        newNSize = std::stoi(line.substr(line.find(' ') + 1));

        std::getline(file, line);
        nSize = std::stoi(line.substr(line.find(' ') + 1));

        std::getline(file, pointsRow);

        std::getline(file, line);
        pixelValues = splitAndConvertToFloat(line.substr(line.find(' ') + 1), ' ');

        file.close();
    } else {
        std::cout << "Failed to open file: " << filename << std::endl;
    }
}

int main(int argc, char *argv[]){
  char filename[MAX_PATH];
  sprintf_s(filename, "%s", argv[1]);
  int newNSize = 0;
  int nSize = 0;
  std::string pointsRow;
  std::vector<float> pixelValues;
  
  printf(filename);

  readSegmentFile(filename, newNSize, nSize, pointsRow, pixelValues);

  int nMinHt = 99999;
  int nMaxHt = -99999;
  float fSum = 0;
  int n = 0;
  float fHt;
  int nHt;
  for(int i=0; i<nSize; i++){
    fHt = pixelValues[i];
    nHt = (int)(fHt + 0.5f);
    if(nHt > 100){
      fSum += (float)nHt;
      if (nHt < nMinHt) {
        nMinHt = nHt;
      }
      if (nHt > nMaxHt) {
        nMaxHt = nHt;
      } 
      n++;
    }
  }

  const int TABLESIZE = 256;
  int nCount[TABLESIZE];
  for(int i=0; i<TABLESIZE; i++)
    nCount[i] = 0;

  printf("%d points found.\n", nSize);
  printf("Lowest Altitude  = %dm\n", nMinHt);
  printf("Average Altitude = %dm\n", (int)(fSum/n));
  printf("Highest Altitude = %dm\n", nMaxHt);
  
  printf("Analyzing...\n");
  const int DISTRIBUTIONSIZE = 32;
  const float GRANULARITY = (float)(nMaxHt - nMinHt + 1.0f)/(float)DISTRIBUTIONSIZE;
  for(int i=0; i<nSize; i++){
    float fHt = pixelValues[i];
    int nHt = (int)(fHt + 0.5f);
    if(nHt > 100.0f)
      nCount[(int)((nHt - nMinHt)/GRANULARITY)]++;
  }

  FILE* output;
  fopen_s(&output, "output.txt", "wt");
  int delta = (int)((float)(nMaxHt - nMinHt + 1)/GRANULARITY);
  if(output){
    for(int i=0; i<delta; i++)
      fprintf(output, "%0.2f\t%d\n", i*GRANULARITY + nMinHt, nCount[i]);
    fclose(output);
  } //if
 
  int nSum = 0;
  for(int i=0; i<=delta; i++)
    nSum += nCount[i];

  int nCount2[TABLESIZE];
  int nTally = 0;
  for(int i=0; i<=delta; i++){
    int count = (int)((float)(TABLESIZE*nCount[i])/nSum);
    nTally += count;
    nCount2[i] = count;
  }
  
  for(int i=0; i<=delta; i++)
    if(nTally < TABLESIZE){
      nCount2[i]++; nTally++;
    }

  int min = 0;
  while(nCount2[min] == 0)
    min++;

  int max = delta;
  while(nCount2[max] == 0)
    max--;

  fopen_s(&output, "analyze.csv", "wt");
  if(output){
    fprintf(output, "DistributionCount %d\n", max - min + 1);
    fprintf(output, "Distribution ");
    int sum = 0;
    for(int i=min; i<max; i++){
      fprintf(output, "%d ", nCount2[i]);
      sum += nCount2[i];
    } //for
    fprintf(output, "%d\n", nCount2[max]);
    sum += nCount2[max];

    fprintf(output, "PointsSize %d\n", newNSize);
    fprintf(output, "%s\n", pointsRow.c_str());
    fprintf(output, "MinAltitude %d\n", nMinHt);
    fprintf(output, "MaxAltitude %d", nMaxHt);

    printf("Checking: %d entries that sum to %d\n", max-min+1, sum);
    fclose(output);
  }

  // printf("Hit Almost Any Key to Exit...\n");

  // _getch();
  return 0;
}