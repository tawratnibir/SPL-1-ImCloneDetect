#pragma once

#include "BMP.h"
#include<vector>
void toNegative(BMP source, const char *destinationFileName);
void toGrayScale(BMP source, const char *destinationFileName);
std::vector<uint8_t> pixelGenerator(BMP source);
