#include <iostream>
#include <vector>
#include "../headers/BMP.h"
#include "../headers/Normalizer.h"
#include <string.h>

int main() {
    BMP image1("bubble_test_1_gray.bmp");
    BMP image2("bubble_test_2_gray.bmp");

    // BMP image3("bubble_test_1.bmp");
    image1 = toNegative(image1, "bubble_test_1_negative.bmp");
    // BMP image4("bubble_test_2.bmp");
    image2 = toNegative(image2, "bubble_test_2_negative.bmp");

    image1 = toGsBlur(image1, 3, 1.0, "bubble_test_1_negative_blur.bmp");
    image2 = toGsBlur(image2, 3, 1.0, "bubble_test_2_negative_blur.bmp");
}