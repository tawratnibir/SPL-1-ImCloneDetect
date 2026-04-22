#include "../headers/BMP.h"
#include "../headers/Normalizer.h"
#include "../headers/jaccard.h"
#include "../headers/pHash.h"
#include <vector>
#include <cmath>
#include <string>
#include <algorithm>
#include <cstdlib>

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

std::string dctImagePath(const std::string& sourceFileName)
{
    std::string outputPath = "..\\dctImages\\";
    int start = 0;
    int end = (int)sourceFileName.size();

    for (int i = (int)sourceFileName.size() - 1; i >= 0; i--)
    {
        if (sourceFileName[i] == '\\' || sourceFileName[i] == '/')
        {
            start = i + 1;
            break;
        }
    }

    for (int i = (int)sourceFileName.size() - 1; i >= start; i--)
    {
        if (sourceFileName[i] == '.')
        {
            end = i;
            break;
        }
    }

    for (int i = start; i < end; i++)
    {
        outputPath.push_back(sourceFileName[i]);
    }

    outputPath += "_dct.bmp";
    return outputPath;
}

void dctImageGeneration(const std::vector<std::vector<double>>& dctPixelValues, const std::string& sourceFileName)
{
    if (dctPixelValues.empty() || dctPixelValues[0].empty())
    {
        return;
    }

    int height = (int)dctPixelValues.size();
    int width = (int)dctPixelValues[0].size();

    double minValue = dctPixelValues[0][0];
    double maxValue = dctPixelValues[0][0];

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (dctPixelValues[i][j] < minValue) minValue = dctPixelValues[i][j];
            if (dctPixelValues[i][j] > maxValue) maxValue = dctPixelValues[i][j];
        }
    }

    double range = maxValue - minValue;
    BMP dctImage(width, height, false);
    std::vector<uint8_t> pixels;
    pixels.reserve(height * width * 3);

    for (int i = height - 1; i >= 0; i--)
    {
        for (int j = 0; j < width; j++)
        {
            double grayValue = 0.0;
            if (range != 0.0)
            {
                grayValue = ((dctPixelValues[i][j] - minValue) * 255.0) / range;
            }

            if (grayValue < 0.0) grayValue = 0.0;
            if (grayValue > 255.0) grayValue = 255.0;

            uint8_t gray = (uint8_t)std::round(grayValue);

            pixels.push_back(gray);
            pixels.push_back(gray);
            pixels.push_back(gray);
        }
    }

    dctImage.data = pixels;

    system("if not exist ..\\dctImages mkdir ..\\dctImages");

    std::string outputPath = dctImagePath(sourceFileName);
    dctImage.write(outputPath.c_str());
}

std::string generateHash(BMP source, int k, int l)
{
    int height = source.bmpInfoHeader.height;
    int width = source.bmpInfoHeader.width;

    std::vector<std::vector<uint8_t>> pixelValues = pixelVectorGenerator(source, width, height);
    std::vector<std::vector<double>> dctPixelValues = dct(pixelValues);
    dctImageGeneration(dctPixelValues, source.fileName);

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
