// #include <iostream>
// #include <vector>
// #include "../headers/BMP.h"
// #include "../headers/Normalizer.h"
// #include <string.h>
#include "../headers/jaccard.h"
int main()
{
    freopen("jaccardTest.txt", "w", stdout);
    BMP image1("bubble_test_1.bmp");
    BMP image2("bubble_test_2.bmp");

    BMP image4("bubble_test_1_gray.bmp");
    BMP image3("bubble_test_2_gray.bmp");

    BMP image6("bubble_test_1_blur.bmp");
    BMP image5("bubble_test_2_blur.bmp");

    BMP image8("bubble_test_1_negative_blur.bmp");
    BMP image7("bubble_test_2_negative_blur.bmp");
    // cout << image1.bmpInfoHeader.height << " " << image1.bmpInfoHeader.width << endl;
    // cout << image2.bmpInfoHeader.height << " " << image2.bmpInfoHeader.width << endl;
    double distance = jaccardDistance(image1, image2);
    cout << "Normal images, Similarity: " << 1 - distance << endl;

    distance = jaccardDistance(image3, image4);
    cout << "Grayscale, Similarity: " << 1 - distance << endl;

    distance = jaccardDistance(image6, image5);
    cout << "Blur, Similarity: " << 1 - distance << endl;

    distance = jaccardDistance(image8, image7);
    cout << "Negative blur, Similarity: " << 1 - distance << endl;
}