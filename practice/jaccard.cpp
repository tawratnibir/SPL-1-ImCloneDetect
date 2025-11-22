#include <iostream>
#include <vector>
#include "../headers/BMP.h"
#include "../headers/Normalizer.h"
#include <string.h>
#include "../headers/jaccard.h"
int main()
{
    freopen("jaccardTest.txt", "w", stdout);
    BMP image1("bubble_test_1_negative_blur.bmp");
    BMP image2("bubble_test_2_negative_blur.bmp");
    // cout << image1.bmpInfoHeader.height << " " << image1.bmpInfoHeader.width << endl;
    // cout << image2.bmpInfoHeader.height << " " << image2.bmpInfoHeader.width << endl;
    double distance = jaccardDistance(image1, image2);
    cout << "Similarity: " << 1 - distance << endl;
}