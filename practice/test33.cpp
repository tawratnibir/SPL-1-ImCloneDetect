#include "../headers/BMP.h"
#include "../headers/Normalizer.h"
#include<iostream>
#include<vector>

int main() {
    freopen("gs.txt", "w", stdout);
    BMP image1("bubble1.bmp");
    BMP image2("bubble2.bmp");
    std::vector<uint8_t> negativePixels, grayPixels, normalPixels, blurPixels, blurPixels2;
    grayPixels = toGrayScale(image1, "test33_gray_2.bmp");
    negativePixels = toNegative(image1, "test33_neg_2.bmp");
    // BMP negativeImage("test33_neg.bmp");
    // negativePixels = pixelGenerator(negativeImage);
    // normalPixels = pixelGenerator(image1);
    normalPixels = pixelGenerator(image1);
    // cout << "Here" << endl;
    blurPixels = toGsBlur(image1, 5, 1.0, "bubble1_gs.bmp");
    blurPixels2 = toGsBlur(image2, 5, 1.0, "bubble2_gs.bmp");
    // int height = image1.bmpInfoHeader.height;
    // int width = image1.bmpInfoHeader.width;
    // int bpp = image1.bmpInfoHeader.bitCount / 8;
    // int rowStride = bpp * width;
    // cout << height << " " << width << endl;
    // cout << "Here" << endl;
    // for(int y=0;y<height;++y) {
    //     for(int x=0; x< width; ++x) {
    //         int idx = y * rowStride + x * bpp;
    //         cout << (int)grayPixels[idx + 0] << " " << (int)grayPixels[idx + 0] << " " << (int)grayPixels[idx + 0] << " ";
    //     }
    //     // cout << endl;
    //     cout << endl;
    // }
}