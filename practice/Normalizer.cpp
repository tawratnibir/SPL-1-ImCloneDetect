#include "../headers/BMP.h"
#include<vector>
#include<iostream>
BMP toNegative(BMP &source, const char* destinationFileName)
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
int main() {
    BMP source("tester.bmp");
    BMP temp = source;
    temp = toNegative(source, "neg.bmp");
}