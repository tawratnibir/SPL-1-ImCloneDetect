#pragma once

#include <iostream>
#include <vector>
#include "BMP.h"
#include "Normalizer.h"

std::vector<std::vector<uint8_t>> pixelVectorGenerator(BMP image, int maxWidth, int maxHeight);
double jaccardDistance(BMP &image1, BMP &image2);