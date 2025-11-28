#include "..\headers\BMP.h"

int main() {
    BMP bmp(200, 200, true);
    // bmp.data = {0};
    vector<uint8_t> pixels = bmp.data;
    for(auto x:pixels) {
        x = 255;
    }
    bmp.data = pixels;
    bmp.write("testImageBlank.bmp");
}