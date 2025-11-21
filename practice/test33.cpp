#include "../headers/BMP.h"
#include "../headers/Normalizer.h"
#include<iostream>
#include<vector>

int main() {
    freopen("gs.txt", "w", stdout);
    BMP image1("bubble1.bmp");
    BMP image2("bubble2.bmp");
    std::vector<uint8_t> negativePixels, grayPixels1, grayPixels2, normalPixels, blurPixels, blurPixels2;
    grayPixels1 = toGrayScale(image1, "bubble1_gray.bmp");
    grayPixels2 = toGrayScale(image2, "bubble2_gray.bmp");

    blurPixels = toGsBlur(image1, 5, 1.0, "bubble1_gs.bmp");
    blurPixels2 = toGsBlur(image2, 5, 1.0, "bubble2_gs.bmp");
    int height1 = image1.bmpInfoHeader.height;
    int width1 = image1.bmpInfoHeader.width;
    int bpp1 = image1.bmpInfoHeader.bitCount / 8;
    int rowStride1 = bpp1 * width1;
    cout << height1 << " " << width1 << endl;

    int height2 = image2.bmpInfoHeader.height;
    int width2 = image2.bmpInfoHeader.width;
    int bpp2 = image2.bmpInfoHeader.bitCount / 8;
    int rowStride2 = bpp2 * width2;
    cout << height2 << " " << width2 << endl;
    for(int y=0;y<height1;++y) {
        for(int x=0; x< width1; ++x) {
            int idx = y * rowStride1 + x * bpp1;
            cout << (int)grayPixels1[idx + 0] << " " << (int)grayPixels1[idx + 1] << " " << (int)grayPixels1[idx + 2] << " ";
        }
        // cout << endl;
        cout << endl;
    }
}