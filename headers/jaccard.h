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

/// @brief Calculates the Jaccard similarity between two images.
/// Compares pixel-level presence (non-zero vs zero) between images.
/// Similarity is computed as: 1 - (diff / sum), where diff is the number
/// of mismatched pixels and sum is the total number of pixels present in either image.
/// @param image1 First BMP image for comparison.
/// @param image2 Second BMP image for comparison.
/// @return Jaccard similarity value between 0.0 (completely different) and 1.0 (identical).
double jaccardDistance(BMP &image1, BMP &image2);