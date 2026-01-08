#pragma once

#include <iostream>
#include <vector>
#include "BMP.h"
#include "Normalizer.h"

/// @brief Generates a 2D vector of pixel values from a BMP image.
/// @param image The source BMP image.
/// @param maxWidth Maximum width for pixel extraction.
/// @param maxHeight Maximum height for pixel extraction.
/// @return 2D vector containing pixel values from the image.
std::vector<std::vector<uint8_t>> pixelVectorGenerator(BMP image, int maxWidth, int maxHeight);

/// @brief Calculates the Jaccard distance similarity between two images.
/// @param image1 First BMP image for comparison.
/// @param image2 Second BMP image for comparison.
/// @return Jaccard distance value between 0 and 1, where 1 indicates identical images.
double jaccardDistance(BMP &image1, BMP &image2);