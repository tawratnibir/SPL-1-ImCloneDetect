#pragma once

#include <iostream>
#include <string.h>
#include <vector>
#include <iomanip>
#include "BMP.h"
#include "Normalizer.h"
#include "jaccard.h"

vector<vector<double>> dct(vector<vector<uint8_t>> sourceVector);
std::string generateHash(BMP source, int k, int l);
double pHash(string s1, string s2);