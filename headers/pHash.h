#pragma once

#include <iostream>
#include <string.h>
#include <vector>
#include <iomanip>
#include "BMP.h"
#include "Normalizer.h"
#include "jaccard.h"

/// @brief Computes the Discrete Cosine Transform (DCT) of a 2D pixel matrix.
/// @param sourceVector 2D vector of pixel values to transform.
/// @return 2D vector containing the DCT coefficients.
vector<vector<double>> dct(vector<vector<uint8_t>> sourceVector);

/// @brief Generates a perceptual hash string for a BMP image.
/// @param source The source BMP image to hash.
/// @param k Number of low-frequency DCT coefficients to use (rows).
/// @param l Number of low-frequency DCT coefficients to use (columns).
/// @return Perceptual hash string representation of the image.
std::string generateHash(BMP source, int k, int l);

/// @brief Calculates the perceptual hash similarity between two hash strings.
/// @param s1 First perceptual hash string.
/// @param s2 Second perceptual hash string.
/// @return Similarity score indicating how similar the images are.
double pHash(string s1, string s2);