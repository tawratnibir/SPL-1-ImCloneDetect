#include "BMP.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

BMP::BMP(const char *fName)
{
    read(fName);
}

void BMP::read(const char *fName)
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
            }
            else
            {
                std::cerr << "Warning! The file \"" << fName << "Warning! The file \"" << fName << "\" does not seem to contain bit mask information\n";
                throw std::runtime_error("Error! Unrecognized file format");
            }
        }

        inp.seekg(fileHeader.offsetData, inp.beg);

        if (bmpInfoHeader.bitCount == 32)
        {
            bmpInfoHeader.size = sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);
            fileHeader.offsetData = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + sizeof(BMPColorHeader);
        }
        else
        {
            bmpInfoHeader.size = sizeof(BMPInfoHeader);
            fileHeader.offsetData = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);
        }
        fileHeader.fileSize = fileHeader.offsetData;

        if (bmpInfoHeader.height < 0)
        {
            throw std::runtime_error("Can't read BMP image properly for negative height!\n");
        }

        data.resize(bmpInfoHeader.height * bmpInfoHeader.width * bmpInfoHeader.bitCount / 8);
        if (bmpInfoHeader.width % 4 == 0)
        {
            inp.read((char *)data.data(), data.size());
            fileHeader.fileSize += data.size();
        }
        else
        {
            rowStride = bmpInfoHeader.width * bmpInfoHeader.bitCount / 8;
            uint32_t newStride = makeStrideAligned(4);

            uint32_t padding = newStride - rowStride;

            for (int y = 0; y < bmpInfoHeader.height; y++)
            {
                inp.read((char *)(data.data() + rowStride * y), rowStride);
                inp.seekg(padding, std::ios::cur);
            }
            fileHeader.fileSize += data.size() + bmpInfoHeader.height * padding;
        }
    }
    else
    {
        throw std::runtime_error("Unable to open the file!\n");
    }
}

BMP::BMP(int32_t width, int32_t height, bool hasAlpha = true)
{
}

void BMP::write(const char *fName)
{
    std::ofstream of{fName, std::ios::binary};
    if (of)
    {
        if (bmpInfoHeader.bitCount == 32)
        {
            writeHeadersAndData(of);
        }
        else if (bmpInfoHeader.bitCount == 24)
        {
            if (bmpInfoHeader.width % 4 == 0)
            {
                writeHeadersAndData(of);
            }
            else
            {
                rowStride = bmpInfoHeader.width * bmpInfoHeader.bitCount / 8;
                uint32_t newStride = makeStrideAligned(4);
                uint32_t padding = newStride - rowStride;

                writeHeaders(of);

                for (int y = 0; y < bmpInfoHeader.height; ++y)
                {
                    of.write((const char *)(data.data() + rowStride * y), rowStride);
                    for (uint32_t i = 0; i < padding; ++i)
                    {
                        of.put('\0');
                    }
                }
            }
        }
        else
        {
            throw std::runtime_error("Only 24 or 32 bit bmp is available!\n");
        }
    }
    else
    {
        throw std::runtime_error("Unable to open the output image!\n");
    }
}

double BMP::imageSize()
{
    return fileHeader.fileSize / 1024.0;
}

uint16_t BMP::imageType()
{
    return fileHeader.fileType;
}
void BMP::writeHeaders(std::ofstream &of)
{
    of.write((const char *)&fileHeader, sizeof(fileHeader));
    of.write((const char *)&bmpInfoHeader, sizeof(bmpInfoHeader));

    if (bmpInfoHeader.bitCount == 32)
    {
        of.write((const char *)&bmpColorHeader, sizeof(bmpColorHeader));
    }
}
void BMP::writeHeadersAndData(std::ofstream &of)
{
    writeHeaders(of);
    of.write((const char *)data.data(), data.size());
}

uint32_t BMP::makeStrideAligned(uint32_t alignStride)
{
    uint32_t newStride = rowStride;

    while (newStride % 4 != 0)
    {
        newStride++;
    }
    return newStride;
}
void BMP::checkColorHeader(BMPColorHeader &bmpColorHeader)
{
    BMPColorHeader expectedColorHeader;

    if (bmpColorHeader.alphaMask != expectedColorHeader.alphaMask ||
        bmpColorHeader.redMask != expectedColorHeader.redMask ||
        bmpColorHeader.greenMask != expectedColorHeader.greenMask ||
        bmpColorHeader.blueMask != expectedColorHeader.blueMask)
    {
        throw std::runtime_error(
            "Unexpected color mask format !"
            "The program expects the pixel data to be in BGRA format.");
    }

    if (expectedColorHeader.colorSpaceType != bmpColorHeader.colorSpaceType)
    {
        throw std::runtime_error(
            "Unexpected color space type! "
            "The program expects the pixel data to be in BGRA format.");
    }
}