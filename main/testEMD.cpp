#include "../headers/emd.h"

int main() {
    BMP image1("../inputImages/b1.bmp");
    BMP image2("../inputImages/b2.bmp");
    cout << computeEmd(image1, image2) << endl;
}