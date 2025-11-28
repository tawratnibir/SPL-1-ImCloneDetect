#include "../headers/pHash.h"

int main() {
    freopen("testPixel.txt", "w", stdout);
    BMP source1("image1_32x32.bmp");
    BMP source2("variant_blur.bmp");
    BMP source3("variant_brightness.bmp");
    BMP source4("variant_noise_strong.bmp");
    BMP source5("variant_perspective.bmp");
    BMP source6("variant_rotate_25.bmp");
    BMP source7("variant_shift_large.bmp");
    BMP source8("variant_stretch.bmp");
    string str1 = generateHash(source1, 8, 8);
    string str2 = generateHash(source2, 8, 8);
    string str3 = generateHash(source3, 8, 8);
    string str4 = generateHash(source4, 8, 8);
    string str5 = generateHash(source5, 8, 8);
    string str6 = generateHash(source6, 8, 8);
    string str7 = generateHash(source7, 8, 8);
    string str8 = generateHash(source8, 8, 8);
    // cout << str1 << endl << str2 << endl;
    cout << source2.fileName << " : " << pHash(str1, str2) * 100 << "%" << endl;
    cout << source3.fileName << " : " << pHash(str1, str3) * 100 <<"%" << endl;
    cout << source4.fileName << " : " <<  pHash(str1, str4) * 100 <<"%" << endl;
    cout << source5.fileName << " : " <<  pHash(str1, str5) * 100 <<"%" << endl;
    cout << source6.fileName << " : " <<  pHash(str1, str6) * 100 <<"%" << endl;
    cout << source7.fileName << " : " <<  pHash(str1, str7) * 100 <<"%" << endl;
    cout << source8.fileName << " : " <<  pHash(str1, str8)  * 100 <<"%" << endl;

    return 0;
}