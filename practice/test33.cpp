#include "../headers/BMP.h"
#include "../headers/Normalizer.h"
#include<iostream>
#include<vector>

int main() {
    BMP image1("tester.bmp");
    toGrayScale(image1, "test33_gray.bmp");
    toNegative(image1, "test33_neg.bmp");
}