#pragma once

#include <string>

/// @brief Formats a single source file in place using clang-format.
/// @param filePath Path to the source or header file to format.
void formatAFile(const std::string &filePath);

/// @brief Formats all supported source files in a directory using clang-format.
/// Processes regular files in the specified folder and applies formatting to
/// recognized C and C++ source/header extensions.
/// @param directoryPath Path to the directory containing files to format.
void formatAFolder(const std::string &directoryPath);
