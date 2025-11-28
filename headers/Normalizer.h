#pragma once

#include "BMP.h"
#include <vector>
#include <cmath>
#include <iomanip>
double gaussian(double x, double y, double sigma);
std::vector<std::vector<double>> createKernel(int kernelRadius, double sigma);
BMP toNegative(BMP source, const char *destinationFileName);
BMP toGrayScale(BMP source, const char *destinationFileName);
BMP toGsBlur(BMP source, int kernelRadius, double sigma, const char *destinationFileName);
std::vector<uint8_t> pixelGenerator(BMP source);
