#include "../headers/BMP.h"
#include "../headers/Normalizer.h"
#include "../headers/jaccard.h"
#include <vector>
#include <cmath>
#include <string>
#include <algorithm>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define pi M_PI

std::vector<std::vector<double>> dct(const std::vector<std::vector<uint8_t>>& sourceVector)
{
    int n = sourceVector.size();       // rows
    int m = sourceVector[0].size();    // cols

    std::vector<std::vector<double>> out(n, std::vector<double>(m, 0.0));

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < m; j++)
        {
            double ci = (i == 0) ? 1.0 / std::sqrt(n) : std::sqrt(2.0) / std::sqrt(n);
            double cj = (j == 0) ? 1.0 / std::sqrt(m) : std::sqrt(2.0) / std::sqrt(m);

            double sum = 0.0;
            for (int k = 0; k < n; k++)
            {
                for (int l = 0; l < m; l++)
                {
                    sum += sourceVector[k][l] *
                           std::cos((2 * k + 1) * i * pi / (2.0 * n)) *
                           std::cos((2 * l + 1) * j * pi / (2.0 * m));
                }
            }

            out[i][j] = ci * cj * sum;
        }
    }

    return out;
}

std::string generateHash(BMP source, int k, int l)
{
    int height = source.bmpInfoHeader.height;
    int width = source.bmpInfoHeader.width;

    std::vector<std::vector<uint8_t>> pixelValues = pixelVectorGenerator(source, width, height);
    std::vector<std::vector<double>> dctPixelValues = dct(pixelValues);

    k = std::min(k, (int)dctPixelValues.size());
    l = std::min(l, (int)dctPixelValues[0].size());

    std::string hashValue;
    double avg = 0.0;
    int count = 0;

    for (int i = 0; i < k; i++)
    {
        for (int j = 0; j < l; j++)
        {
            if (i == 0 && j == 0) continue;
            avg += dctPixelValues[i][j];
            count++;
        }
    }

    if (count > 0) avg /= count;

    for (int i = 0; i < k; i++)
    {
        for (int j = 0; j < l; j++)
        {
            if (i == 0 && j == 0) continue;
            hashValue.push_back(dctPixelValues[i][j] > avg ? '1' : '0');
        }
    }

    return hashValue;
}

double pHash(const std::string& s1, const std::string& s2)
{
    if (s1.size() != s2.size()) return -1.0;

    int n = (int)s1.size();
    int diff = 0;

    for (int i = 0; i < n; i++)
    {
        if (s1[i] != s2[i]) diff++;
    }

    return 1.0 - (double)diff / n;
}