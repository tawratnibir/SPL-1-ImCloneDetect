#include "BMP.h"
#include "Normalizer.h"
#include "jaccard.h"
#include <iostream>
#include <string.h>
#include <vector>
#include <iomanip>
#define pi 3.1415926
vector<vector<double>> dct(vector<vector<uint8_t>> sourceVector)
{
    int n = sourceVector.size();
    int m = sourceVector[0].size();

    vector<vector<double>> dct(n, vector<double>(m));

    double sum, dctTemp, ci, cj;
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            if (i == 0)
                ci = 1.0 / sqrt(m);
            else
                ci = sqrt(2) / sqrt(m);
            if (j == 0)
                cj = 1.0 / sqrt(n);
            else
                cj = sqrt(2) / sqrt(n);

            sum = 0;
            for (int k = 0; k < n; k++) {
                for (int l = 0; l < m; l++) {
                    dctTemp = sourceVector[k][l] * 
                           cos((2 * k + 1) * i * pi / (2 * m)) * 
                           cos((2 * l + 1) * j * pi / (2 * n));
                    sum = sum + dctTemp;
                }
            }
            dct[i][j] = ci * cj * sum;
        }
    }

    return dct;
}
std::string generateHash(BMP source, int k, int l)
{
    int height = source.bmpInfoHeader.height;
    int width = source.bmpInfoHeader.width;
    // cout << height << " " << width << endl;
    std::vector<vector<uint8_t>> pixelValues = pixelVectorGenerator(source, width, height);
    for(int i=0;i<height;i++) {
        for(int j=0;j<width;j++) {
            cout << (int)pixelValues[i][j] << "   ";
        }
        cout << endl;
    }
    std::vector<vector<double>> dctPixelValues = dct(pixelValues);
    string hashValue;
    double avg = 0;
    for(int i=0;i<k;i++) {
        for(int j=0;j<l;j++) {
            if(i == 0 && j == 0) continue;
            avg += dctPixelValues[i][j];
        }
    }
    avg /= (k * l - 1);

    for(int i=0;i<k;i++) {
        for(int j=0;j<l;j++) {
            // if(i == 0 && j == 0) continue;
            if(dctPixelValues[i][j] > avg) {
                hashValue.push_back('1');
            } else hashValue.push_back('0');
        }
    }
    return hashValue;    
}
double pHash(string s1, string s2) {
    int n = s1.size();
    int diff = 0;
    int sum = n;
    for(int i=0;i<n;i++) {
        if(s1[i] != s2[i]) diff++;
    }

    return (1.0 - ((double)diff / sum));
}