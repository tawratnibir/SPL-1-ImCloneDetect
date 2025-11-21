#include <iostream>
#include <vector>
#include "../headers/BMP.h"
#include "../headers/Normalizer.h"
#include <string.h>
double jaccardDistance(BMP &image1, BMP &image2)
{
    int width1 = image1.bmpInfoHeader.width;
    int height1 = image1.bmpInfoHeader.height;
    int bpp1 = image1.bmpInfoHeader.bitCount / 8;
    int rowStride1 = bpp1 * width1;
    vector<uint8_t> pixels1 = pixelGenerator(image1);

    int width2 = image2.bmpInfoHeader.width;
    int height2 = image2.bmpInfoHeader.height;
    int bpp2 = image2.bmpInfoHeader.bitCount / 8;
    int rowStride2 = bpp2 * width2;
    vector<uint8_t> pixels2 = image2.data;

    int maxWidth = max(width1, width2);
    int maxheight = max(height1, height2);

    vector<vector<uint8_t>> imageNorm1(maxheight);
    vector<vector<uint8_t>> imageNorm2(maxheight);
    vector<vector<uint8_t>> sumPixels(maxheight, vector<uint8_t>(maxWidth));
    vector<vector<uint8_t>> diffPixels(maxheight, vector<uint8_t>(maxWidth));

    double diff = 0, sum = 0;
    for (int y = 0; y < height1; ++y)
    {
        for (int x = 0; x < width1; ++x)
        {
            int idx = y * rowStride1 + x * bpp1;
            imageNorm1[y].push_back(pixels1[idx]);
        }
        for(int x=width1; x < maxWidth; ++x) {
            imageNorm1[y].push_back((uint8_t)0);
        }
    }
    for(int y=height1; y < maxheight; ++y) {
        for(int x=0; x < maxWidth; ++x) {
            imageNorm1[y].push_back((uint8_t)0);
        }
    }

    for(int y=0; y < height2; ++y) {
        for(int x=0; x<width2; ++x) {
            int idx = y * rowStride2 + x * bpp2;
            imageNorm2[y].push_back(pixels2[idx]);
        }
        for(int x=width2; x < maxWidth; x++) {
            imageNorm2[y].push_back((uint8_t)0);
        }
    }
    for(int y=height2; y < maxheight; ++y) {
        for(int x=0; x < maxWidth; ++x) {
            imageNorm2[y].push_back((uint8_t)0);
        }
    }
    cout << imageNorm1.size() << " " << imageNorm1[0].size() << endl;
    cout << imageNorm2.size() << " " << imageNorm2[0].size() << endl;
    // for (int y = 0; y < height1; ++y)
    // {
    //     for (int x = 0; x < width1; ++x)
    //     {
    //         int idx = y * rowStride1 + x * bpp1;
    //         std::cout << (int)pixels1[idx + 0] << " " << (int)pixels1[idx + 1] << " " << (int)pixels1[idx + 2] << " ";
    //     }
    //     std::cout << std::endl;
    // }

    for (int i = 0; i < maxheight; ++i)
    {
        for (int j = 0; j < maxWidth; ++j)
        {
            // cout << (int)imageNorm2[i][j] << " ";
            if((imageNorm1[i][j] | imageNorm2[i][j]) && (imageNorm1[i][j] ^ imageNorm2[i][j])) diff++;
            if(imageNorm1[i][j] + imageNorm2[i][j]) sum++;
        }
        // std::cout << std::endl;
    }
    cout << diff << " " << sum << endl;
    return diff / sum;
}
int main()
{
    freopen("jaccardTest.txt", "w", stdout);
    BMP image1("bubble1_gray.bmp");
    BMP image2("bubble2_gray.bmp");
    // cout << image1.bmpInfoHeader.height << " " << image1.bmpInfoHeader.width << endl;
    // cout << image2.bmpInfoHeader.height << " " << image2.bmpInfoHeader.width << endl;
    double distance = jaccardDistance(image1, image1);
    cout << "Similarity: " << 1 - distance << endl;
}