#include "../headers/BMP.h"
#include "../headers/Normalizer.h"
#include<iostream>
#include<vector>

int main() {
    BMP image1("tester.bmp");
    std::vector<uint8_t> negativePixels, grayPixels, normalPixels;
    grayPixels = toGrayScale(image1, "test33_gray.bmp");
    negativePixels = toNegative(image1, "test33_neg.bmp");
    BMP negativeImage("test33_neg.bmp");
    negativePixels = pixelGenerator(negativeImage);
    normalPixels = pixelGenerator(image1);
}