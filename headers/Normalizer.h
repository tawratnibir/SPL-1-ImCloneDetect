#pragma once

#include "BMP.h"
#include <vector>
#include <cmath>
#include <iomanip>

/// @brief Computes the Gaussian function value for the specified coordinates.
/// @param x X-coordinate value.
/// @param y Y-coordinate value.
/// @param sigma Standard deviation for the Gaussian distribution.
/// @return Gaussian function result.
double gaussian(double x, double y, double sigma);

/// @brief Creates a Gaussian kernel for image filtering operations.
/// @param kernelRadius Radius of the kernel in pixels.
/// @param sigma Standard deviation for the Gaussian distribution.
/// @return 2D vector representing the Gaussian kernel.
std::vector<std::vector<double>> createKernel(int kernelRadius, double sigma);

/// @brief Converts a BMP image to its negative (inverted colors).
/// @param source The source BMP image.
/// @param destinationFileName Path where the negative image will be saved.
/// @return BMP object containing the negated image.
BMP toNegative(BMP source, const char *destinationFileName);

/// @brief Converts a BMP image to grayscale format.
/// @param source The source BMP image.
/// @param destinationFileName Path where the grayscale image will be saved.
/// @return BMP object containing the grayscale image.
BMP toGrayScale(BMP source, const char *destinationFileName);

/// @brief Converts a BMP image to Gaussian blurred image.
/// @param source The source BMP image.
/// @param kernelRadius Radius of the Gaussian kernel in pixels.
/// @param sigma Standard deviation for the Gaussian blur.
/// @param destinationFileName Path where the blurred grayscale image will be saved.
/// @return BMP object containing the blurred grayscale image.
BMP toGsBlur(BMP source, int kernelRadius, double sigma, const char *destinationFileName);

/// @brief Extracts pixel values from a BMP image into a 1D vector.
/// @param source The source BMP image.
/// @return Vector containing all pixel values from the image.
std::vector<uint8_t> pixelGenerator(BMP source);

/// @brief Resizes a BMP image using bilinear interpolation.
/// @param source The source BMP image.
/// @param outHeight Target height for the resized image in pixels.
/// @param outWidth Target width for the resized image in pixels.
/// @param outFileName Path where the resized image will be saved.
/// @return BMP object containing the resized image.
BMP resizeBilinear(BMP source, int outHeight, int outWidth, const char *outFileName);
