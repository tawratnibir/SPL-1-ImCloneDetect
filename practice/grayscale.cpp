#include "../headers/BMP.cpp"
#include<iostream>
#include<vector>

int main() {
    BMP bmp("tester.bmp");
    int height = bmp.bmpInfoHeader.height;
    int width = bmp.bmpInfoHeader.width;
    int bpp = bmp.bmpInfoHeader.bitCount / 8;
    int rowStride = bpp * width;
    vector<uint8_t> pixels = bmp.data;

    for(int y=0;y<height;++y) {
        for(int x=0; x< width; ++x) {
            int idx = y * rowStride + x * bpp;

            uint8_t B = pixels[idx + 0];
            uint8_t G = pixels[idx + 1];
            uint8_t R = pixels[idx + 2];

            uint8_t Y = 0.299 * R + 0.587 * G + 0.114 * B;


            pixels[idx + 0] = Y;
            pixels[idx + 1] = Y;
            pixels[idx + 2] = Y;
        }
    }

    bmp.data = pixels;

    bmp.write("gray.bmp");
}