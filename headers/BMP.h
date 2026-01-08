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

    /// @brief Constructs a BMP object and reads image data from file.
    /// @param fName Path to the BMP file to read.
    BMP(const char *fName);
    
    /// @brief Reads BMP image data from the specified file.
    /// @param fName Path to the BMP file to read.
    void read(const char *fName);

    /// @brief Constructs a BMP object with specified dimensions.
    /// @param width Image width in pixels.
    /// @param height Image height in pixels.
    /// @param hasAlpha Whether the image includes an alpha channel.
    BMP(int32_t width, int32_t height, bool hasAlpha);
    
    /// @brief Writes the BMP image to a file.
    /// @param fName Path where the BMP file will be written.
    void write(const char *fName);

    /// @brief Calculates the total size of the image in bytes.
    /// @return Image size in bytes.
    double imageSize();
    
    /// @brief Retrieves the image type/format identifier.
    /// @return Image type as uint16_t.
    uint16_t imageType();

private:
    uint32_t rowStride{0};

    /// @brief Writes BMP file headers to an output stream.
    /// @param of Output file stream where headers will be written.
    void writeHeaders(std::ofstream &of);
    
    /// @brief Writes both headers and image data to an output stream.
    /// @param of Output file stream where headers and data will be written.
    void writeHeadersAndData(std::ofstream &of);
    
    /// @brief Aligns row stride to the nearest multiple for proper BMP formatting.
    /// @param alignStride Current stride value to align.
    /// @return Aligned stride value.
    uint32_t makeStrideAligned(uint32_t alignStride);
    
    /// @brief Validates and corrects the BMP color header information.
    /// @param bmpColorHeader Color header structure to validate and modify.
    void checkColorHeader(BMPColorHeader &bmpColorHeader);
};
