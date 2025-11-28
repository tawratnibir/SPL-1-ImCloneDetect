#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
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
    string fileName;
    vector<uint8_t> data;

    BMP(const char *fName);
    void read(const char *fName);

    BMP(int32_t width, int32_t height, bool hasAlpha);
    void write(const char *fName);

    double imageSize();
    uint16_t imageType();

private:
    uint32_t rowStride{0};

    void writeHeaders(std::ofstream &of);
    void writeHeadersAndData(std::ofstream &of);
    uint32_t makeStrideAligned(uint32_t alignStride);
    void checkColorHeader(BMPColorHeader &bmpColorHeader);
};
