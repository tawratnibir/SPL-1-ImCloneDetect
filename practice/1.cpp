#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using namespace std;
#pragma pack(push, 1)
struct BMPFileHeader
{
    uint16_t fileType{0x4D42};
    uint32_t fileSize{0};
    uint16_t reserved1{0};
    uint16_t reserved2{0};
    uint32_t offsetData{0};
};
struct BMPInfoHeader
{
    uint32_t size{0};
    int32_t width{0};
    int32_t height{0};

    uint16_t planes{1};
    uint16_t bitCount{0};
    uint32_t compression{0};
    uint32_t sizeImage{0};
    int32_t xPixelsPerMeter{0};
    int32_t yPixelsPerMeter{0};
    uint32_t colorsUsed{0};
    uint32_t colorsImportant{0};
};
struct BMPColorHeader
{
    uint32_t redMask{0x00ff0000};
    uint32_t blueMask{0x0000ff00};
    uint32_t greenMask{0x000000ff};
    uint32_t alphaMask{0xff000000};
    uint32_t colorSpaceType{0x73524742};
    uint32_t unused[16]{0};
};
#pragma pack(pop)
struct BMP
{
    BMPFileHeader fileHeader;
    BMPInfoHeader bmpInfoHeader;
    BMPColorHeader bmpColorHeader;

    vector<uint8_t> data;

    BMP(const char *fName)
    {
    }

    void read(const char *fName)
    {
        std::ifstream inp{fName, std::ios_base::binary};
        if (inp)
        {
            inp.read((char *)&fileHeader, sizeof(fileHeader));
            if (fileHeader.fileType != 0x4D42)
            {
                throw std::runtime_error("Error! Not a BMP file\n");
            }
            else
            {
                cout << "File read successfully\n";
            }
            inp.read((char *)&bmpInfoHeader, sizeof(bmpInfoHeader));

            if (bmpInfoHeader.bitCount == 32)
            {
                if (bmpInfoHeader.size >=
                    (sizeof(BMPInfoHeader) + sizeof(BMPColorHeader)))
                {
                    inp.read((char *)&bmpColorHeader, sizeof(BMPColorHeader));
                    checkColorHeader(bmpColorHeader);
                } else{
                    std::cerr << "Warning! The file \"" << fName << "Warning! The file \"" << fname << "\" does not seem to contain bit mask information\n";
                    throw std::runtime_error("Error! Unrecognized file format");
                }
            }

            inp.seekg(fileHeader.offsetData, inp.beg);
        }
    }

    BMP(int32_t width, int32_t height, bool hasAlpha = true)
    {
    }

    void write(const char *fName)
    {
    }

private:
    void checkColorHeader(BMPColorHeader &bmpColorHeader)
    {
        BMPColorHeader expectedColorHeader;

        if (bmpColorHeader.alphaMask != expectedColorHeader.alphaMask ||
            bmpColorHeader.redMask != expectedColorHeader.redMask ||
            bmpColorHeader.greenMask != expectedColorHeader.greenMask ||
            bmpColorHeader.blueMask != expectedColorHeader.blueMask)
        {
            throw std::runtime_error(
                "Unexpected color mask format !"
                "The program expects the pixel data to be in BGRA format.")
        }

        if (expectedColorHeader.colorSpaceType != bmpColorHeader.colorSpaceType)
        {
            throw std::runtime_error(
                "Unexpected color space type! "
                "The program expects the pixel data to be in BGRA format.")
        }
    }
};

int main()
{
    BMP bmp("tester.bmp");
    bmp.read("tester.bmp");
}
