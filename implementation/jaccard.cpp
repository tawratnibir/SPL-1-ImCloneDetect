#include <vector>
#include "../headers/BMP.h"
#include "../headers/jaccard.h"

std::vector<std::vector<uint8_t>> pixelVectorGenerator(BMP image, int maxWidth, int maxHeight)
{
    int width = image.bmpInfoHeader.width;
    int height = image.bmpInfoHeader.height;
    int bpp = image.bmpInfoHeader.bitCount / 8;
    int rowStride = bpp * width;
    vector<uint8_t> pixels = image.data;
    vector<vector<uint8_t>> imageNorm(maxHeight);

    for (int y = 0; y < height; ++y)
    {
        for (int x = 0; x < width; ++x)
        {
            int idx = y * rowStride + x * bpp;
            imageNorm[y].push_back(pixels[idx]);
        }
        for (int x = width; x < maxWidth; ++x)
        {
            imageNorm[y].push_back((uint8_t)0);
        }
    }
    for (int y = height; y < maxHeight; ++y)
    {
        for (int x = 0; x < maxWidth; ++x)
        {
            imageNorm[y].push_back((uint8_t)0);
        }
    }
    reverse(imageNorm.begin(), imageNorm.end());
    return imageNorm;
}
double jaccardDistance(BMP &image1, BMP &image2)
{
    int width1 = image1.bmpInfoHeader.width;
    int height1 = image1.bmpInfoHeader.height;

    int width2 = image2.bmpInfoHeader.width;
    int height2 = image2.bmpInfoHeader.height;

    int maxWidth = max(width1, width2);
    int maxHeight = max(height1, height2);

    vector<vector<uint8_t>> imageNorm1 = pixelVectorGenerator(image1, maxWidth, maxHeight);
    vector<vector<uint8_t>> imageNorm2 = pixelVectorGenerator(image2, maxWidth, maxHeight);

    double diff = 0, sum = 0;

    for (int i = 0; i < maxHeight; ++i)
    {
        for (int j = 0; j < maxWidth; ++j)
        {
            bool a = (imageNorm1[i][j] != 0);
            bool b = (imageNorm2[i][j] != 0);

            if (a || b)
                sum++;
            if (a != b)
                diff++;
        }
    }
    if(diff == 0 && sum == 0) return 1;
    return 1 - (diff / sum);
}