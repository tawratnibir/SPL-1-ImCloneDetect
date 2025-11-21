#include "BMP.h"
#include "Normalizer.h"
#include <vector>
#include <iostream>
#include <cmath>
#include <iomanip>
double gaussian(double x, double y, double sigma)
{
    return (1.0 / (2.0 * M_PI * sigma * sigma)) * std::exp(-(x * x + y * y) / (2.0 * sigma * sigma));
}
std::vector<std::vector<double>> createKernel(int kernelRadius, double sigma)
{
    int kernelSize = 2 * kernelRadius + 1;
    std::vector<std::vector<double>> kernel(kernelSize, std::vector<double>(kernelSize));
    double sum = 0;
    for (int i = -kernelRadius; i <= kernelRadius; ++i)
    {
        for (int j = -kernelRadius; j <= kernelRadius; ++j)
        {
            kernel[i + kernelRadius][j + kernelRadius] = gaussian(i, j, sigma);
            sum += kernel[i + kernelRadius][j + kernelRadius];
        }
    }

    for (int i = 0; i < kernelSize; i++)
    {
        for (int j = 0; j < kernelSize; j++)
        {
            kernel[i][j] /= sum;
        }
    }
    return kernel;
}
std::vector<uint8_t> toNegative(BMP source, const char *destinationFileName)
{
    int height = source.bmpInfoHeader.height;
    int width = source.bmpInfoHeader.width;
    int bpp = source.bmpInfoHeader.bitCount / 8;
    int rowStride = bpp * width;

    vector<uint8_t> pixels = source.data;

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int idx = y * rowStride + x * bpp;

            pixels[idx + 0] = 255 - pixels[idx + 0];
            pixels[idx + 1] = 255 - pixels[idx + 1];
            pixels[idx + 2] = 255 - pixels[idx + 2];
        }
    }
    source.data = pixels;
    source.write(destinationFileName);
    return pixels;
}
std::vector<uint8_t> toGrayScale(BMP source, const char *destinationFileName)
{
    int height = source.bmpInfoHeader.height;
    int width = source.bmpInfoHeader.width;
    int bpp = source.bmpInfoHeader.bitCount / 8;
    int rowStride = bpp * width;

    vector<uint8_t> pixels = source.data;

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int idx = y * rowStride + x * bpp;

            uint8_t B = pixels[idx + 0];
            uint8_t G = pixels[idx + 1];
            uint8_t R = pixels[idx + 2];

            uint8_t Y = 0.299 * R + 0.587 * G + 0.114 * B;
            if (Y > 54)
            {
                pixels[idx + 0] = Y;
                pixels[idx + 1] = Y;
                pixels[idx + 2] = Y;
            }
            else
            {
                pixels[idx + 0] = 0;
                pixels[idx + 1] = 0;
                pixels[idx + 2] = 0;
            }
            // pixels[idx + 0] = Y - 40;
            // pixels[idx + 1] = Y - 40;
            // pixels[idx + 2] = Y - 40;
        }
    }
    source.data = pixels;
    source.write(destinationFileName);
    return pixels;
}
std::vector<uint8_t> toGsBlur(BMP source, int kernelRadius, double sigma, const char *destinationFileName)
{
    int height = source.bmpInfoHeader.height;
    int width = source.bmpInfoHeader.width;
    int bpp = source.bmpInfoHeader.bitCount / 8;
    int rowStride = bpp * width;

    vector<uint8_t> pixels = source.data;
    vector<vector<double>> kernel = createKernel(kernelRadius, sigma);
    for (int y = kernelRadius; y < height - kernelRadius; ++y)
    {
        for (int x = kernelRadius; x < width - kernelRadius; ++x)
        {
            uint8_t valB = 0, valG = 0, valR = 0;
            for (int i = x - kernelRadius; i < x + kernelRadius; ++i)
            {
                for (int j = y - kernelRadius; j < y + kernelRadius; ++j)
                {
                    int idx = j * rowStride + i * bpp;
                    valB += ((uint8_t)(pixels[idx + 0] * kernel[i - x + kernelRadius][j - y + kernelRadius]));
                    valG += ((uint8_t)(pixels[idx + 1] * kernel[i - x + kernelRadius][j - y + kernelRadius]));
                    valR += ((uint8_t)(pixels[idx + 2] * kernel[i - x + kernelRadius][j - y + kernelRadius]));
                }
            }
            int idx = y * rowStride + x * bpp;
            pixels[idx + 0] = valB;
            pixels[idx + 1] = valG;
            pixels[idx + 2] = valR;
        }
    }
    source.data = pixels;
    source.write(destinationFileName);
    return pixels;
}
std::vector<uint8_t> pixelGenerator(BMP source)
{
    return source.data;
}