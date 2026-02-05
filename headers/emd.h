#pragma once
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <climits>
#include <cstdint>
#include "../headers/BMP.h"
#include "../headers/Normalizer.h"
#include "../headers/jaccard.h"
#include "ortools/graph/min_cost_flow.h"
using namespace operations_research;
using namespace std;

/**
 * @brief Normalizes signature values so they sum to 1.0
 * 
 * Converts raw signature values into a probability distribution by dividing
 * each element by the total sum. This is required for proper EMD computation.
 * 
 * @param sign Reference to the signature vector to be normalized (modified in-place)
 */
void normalizeSignatureValues(vector<double> &sign);

/**
 * @brief Computes Euclidean distance between two 2D points
 * 
 * @param x1 X-coordinate of first point
 * @param y1 Y-coordinate of first point
 * @param x2 X-coordinate of second point
 * @param y2 Y-coordinate of second point
 * @return double Euclidean distance between the two points
 */
double computeEuclid(int x1, int y1, int x2, int y2);

/**
 * @brief Generates EMD signature by dividing image into equal regions
 * 
 * Divides the image into divider×divider equal rectangles and computes
 * the sum of grey-scale pixel values in each region as the weight.
 * 
 * @param pixels 2D vector of pixel grey-scale values
 * @param divider Number of divisions per dimension (e.g., 6 creates 6×6=36 regions)
 * @return vector<double> Flattened signature with divider×divider elements
 */
vector<double> emdSignatureGenerator(vector<vector<uint8_t>> pixels, int divider);

/**
 * @brief Computes EMD-based similarity between two images
 * 
 * Implements Earth Mover's Distance (EMD) similarity metric as described in
 * image clone detection literature. The process:
 * 1. Resizes both images to 300×300
 * 2. Converts to grayscale and applies Gaussian blur
 * 3. Divides each image into 6×6 grid of regions
 * 4. Computes signature (sum of pixel values per region)
 * 5. Normalizes signatures to probability distributions
 * 6. Creates 36×36 Euclidean distance matrix between regions
 * 7. Solves min-cost flow problem to compute EMD
 * 8. Returns similarity = 1 - EMD/EMD_max
 * 
 * @param image1 First BMP image
 * @param image2 Second BMP image
 * @return double Similarity score between 0.0 (completely different) and 1.0 (identical)
 */
double computeEmd(BMP image1, BMP image2);