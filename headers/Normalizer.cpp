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
BMP toNegative(BMP source, const char *destinationFileName)
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
    return source;
}
BMP toGrayScale(BMP source, const char *destinationFileName)
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
    return source;
}
BMP toGsBlur(BMP source, int kernelRadius, double sigma, const char *destinationFileName)
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
    return source;
}
std::vector<uint8_t> pixelGenerator(BMP source)
{
    return source.data;
}
BMP resizeBilinear(BMP source, int outHeight, int outWidth, const char *outFileName)
{
    int srcH = source.bmpInfoHeader.height;
    int srcW = source.bmpInfoHeader.width;
    int srcBpp = source.bmpInfoHeader.bitCount / 8;
    int srcRowStride = srcW * srcBpp;

    bool hasAlpha = (source.bmpInfoHeader.bitCount == 32);
    BMP dest(outWidth, outHeight, hasAlpha);
    int dstBpp = dest.bmpInfoHeader.bitCount / 8;
    int dstRowStride = outWidth * dstBpp;

    std::vector<uint8_t> dstData = dest.data;
    const std::vector<uint8_t> &srcData = source.data;

    double scaleY = (double)srcH / (double)outHeight;
    double scaleX = (double)srcW / (double)outWidth;

    for (int y = 0; y < outHeight; ++y)
    {
        double srcY = (y + 0.5) * scaleY - 0.5;
        int y0 = (int)floor(srcY);
        int y1 = y0 + 1;
        double wy = srcY - y0;

        if (y0 < 0)
        {
            y0 = 0;
            y1 = 0;
            wy = 0.0;
        }
        if (y1 >= srcH)
        {
            y1 = srcH - 1;
            if (y0 >= srcH)
                y0 = srcH - 1;
        }

        for (int x = 0; x < outWidth; ++x)
        {
            double srcX = (x + 0.5) * scaleX - 0.5;
            int x0 = (int)floor(srcX);
            int x1 = x0 + 1;
            double wx = srcX - x0;

            if (x0 < 0)
            {
                x0 = 0;
                x1 = 0;
                wx = 0.0;
            }
            if (x1 >= srcW)
            {
                x1 = srcW - 1;
                if (x0 >= srcW)
                    x0 = srcW - 1;
            }

            for (int c = 0; c < dstBpp; ++c)
            {
                int idx00 = y0 * srcRowStride + x0 * srcBpp + c;
                int idx10 = y0 * srcRowStride + x1 * srcBpp + c;
                int idx01 = y1 * srcRowStride + x0 * srcBpp + c;
                int idx11 = y1 * srcRowStride + x1 * srcBpp + c;

                double v00 = srcData[idx00];
                double v10 = srcData[idx10];
                double v01 = srcData[idx01];
                double v11 = srcData[idx11];

                double val = (1.0 - wx) * (1.0 - wy) * v00 + wx * (1.0 - wy) * v10 + (1.0 - wx) * wy * v01 + wx * wy * v11;

                int didx = y * dstRowStride + x * dstBpp + c;
                int outv = (int)std::round(val);
                if (outv < 0)
                    outv = 0;
                if (outv > 255)
                    outv = 255;
                dstData[didx] = (uint8_t)outv;
            }
        }
    }

    dest.data = dstData;
    dest.write(outFileName);
    return dest;
}