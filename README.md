# ImCloneDetect

## Overview

ImCloneDetect is an image-based code clone detection tool written in C++. It compares BMP images of source-code fragments and estimates how similar those fragments are. Instead of depending only on raw source text, the project treats code as an image and compares its visual structure. This approach helps detect similarity even when code has been changed through variable renaming, formatting changes, or other small textual modifications.

The tool combines three image-similarity methods:

- Jaccard similarity for structural pixel-presence comparison.
- Perceptual Hash (pHash) for frequency-domain visual comparison.
- Earth Mover's Distance (EMD) for distribution-based spatial comparison.

The final result is a weighted score. By default, Jaccard contributes `20%`, pHash contributes `40%`, and EMD contributes `40%`. These weights can be changed from the program menu.

The project was developed for Windows 10/11 x64 using C++, MSVC, Google OR-Tools, clang-format, CSV outputs, and GEXF graph outputs for Gephi visualization.

## Working Method

ImCloneDetect works by converting the visual content of source-code images into measurable image features. The user provides BMP images that contain code snippets. The program then preprocesses the images, computes several independent similarity scores, combines those scores, and saves the results for inspection and visualization.

### 1. Input Preparation

The tool expects BMP images as input. These images should represent source-code fragments, preferably comparable code units such as functions or similar blocks. The project assumptions are:

- Input files are BMP images.
- Images contain source-code fragments.
- Code images should be generated consistently when possible.
- Formatting should be normalized before image capture.
- Syntax-highlighted images are suitable for this workflow.

The current program does not automatically convert source-code files into images. It accepts existing BMP images. To help prepare cleaner source-code images, the program includes a clang-format option that can format C/C++ source files before screenshots or image generation.

### 2. Program Modes

The program provides an interactive menu:

```text
1. Find pairwise similarity on a folder
2. Find similarity of two images
3. Format a file or folder
4. Configure algorithm weights
5. Exit
```

In folder mode, the user provides the full path of a directory containing BMP images. The program compares every unique image pair in that directory.

In single-pair mode, the user provides the full path of two BMP images. The program compares only those two files.

In formatter mode, the user provides the full path of a file or folder. The program applies `clang-format` to supported C/C++ files. Folder formatting is non-recursive.

In weight-configuration mode, the user can change the three algorithm weights. Each value must be greater than `0`, less than `1.0`, and the total must be exactly `1.0`.

### 3. Preprocessing

Before similarity calculation, the program generates intermediate image representations:

- Grayscale images are written to `outputGray/`.
- Resized images for pHash are written to `resizedImage/`.
- DCT visualization images are written to `dctImages/`.
- Temporary EMD preprocessing files are written to `build/tmp_emd/`.

The preprocessing functions are implemented mainly in `implementation/Normalizer.cpp`. They include grayscale conversion, negative image generation, Gaussian kernel creation, Gaussian blur, pixel extraction, and bilinear resizing.

### 4. Jaccard Similarity

Jaccard similarity measures structural overlap. After grayscale conversion, each pixel is treated as either present or absent:

- Nonzero pixel: present.
- Zero pixel: absent.

If two images have different dimensions, the implementation pads them to a common size before comparison. The score is calculated from the number of matching and mismatching pixel-presence positions:

```text
similarity = 1 - (different_presence_pixels / pixels_present_in_either_image)
```

This method focuses on the shape and position of visible code content. It is useful when the visual layout of two snippets is similar.

Relevant files:

- `implementation/jaccard.cpp`
- `headers/jaccard.h`

### 5. Perceptual Hash Similarity

pHash compares images using frequency-domain features. The process is:

1. Convert the image to grayscale.
2. Resize the image to `32 x 32`.
3. Apply the Discrete Cosine Transform (DCT).
4. Take the low-frequency `8 x 8` DCT region.
5. Exclude the DC coefficient.
6. Generate a binary hash from the remaining coefficients.
7. Compare two hashes using Hamming distance.

The similarity is:

```text
similarity = 1 - (hamming_distance / hash_length)
```

pHash is useful because it captures the broader visual pattern of an image rather than comparing only exact pixel positions.

Relevant files:

- `implementation/pHash.cpp`
- `headers/pHash.h`

### 6. Earth Mover's Distance Similarity

EMD compares the spatial distribution of image intensity. The process is:

1. Resize both images to `300 x 300`.
2. Convert both images to grayscale.
3. Apply Gaussian blur.
4. Divide each image into a `6 x 6` grid.
5. Compute a 36-element signature from regional pixel sums.
6. Normalize each signature into a distribution.
7. Build a `36 x 36` Euclidean distance matrix between grid-cell centroids.
8. Use Google OR-Tools min-cost flow to calculate the transport cost between the two distributions.
9. Convert the EMD value into a similarity score.

The final EMD similarity is calculated as:

```text
similarity = max(0, 1 - emd / emd_max)
```

This method is useful for comparing how visual content is distributed across the image, even when exact pixel positions do not fully match.

Relevant files:

- `implementation/emd.cpp`
- `headers/emd.h`

### 7. Overall Similarity

The program combines all three algorithm scores into one final score:

```text
overall = (jaccard_weight * jaccard_score)
        + (phash_weight * phash_score)
        + (emd_weight * emd_score)
```

Default weights:

```text
Jaccard: 0.20
pHash:   0.40
EMD:     0.40
```

The combined score is written to CSV and GEXF outputs. The GEXF files can be opened in Gephi, where similar code fragments can be visualized as graph nodes connected by weighted similarity edges.

## Quick Start

### Option 1: Run the Prebuilt Executable

Use this option if `main/main.exe` and the required DLL files are included.

```powershell
.\run.bat
```

The launcher runs `main/main.exe` from the correct directory. It also checks for the required OR-Tools DLL files beside the executable.

When distributing the prebuilt version, include:

```text
main/main.exe
main/*.dll
run.bat
```

### Option 2: Build From Source

Use this option if the executable is missing or must be rebuilt.

```powershell
.\setup.bat
make build
.\run.bat
```

If the Visual Studio path differs, pass `VCVARS` manually:

```powershell
make build VCVARS="C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
```

## Requirements

- Windows 10/11 x64.
- Visual Studio 2022 or Visual Studio Build Tools 2022 with `Desktop development with C++` for building.
- GNU Make for Windows.
- Google OR-Tools for C++.
- LLVM clang-format for the optional formatting feature.

Users who only run the included `main/main.exe` do not need the full Visual Studio compiler toolchain, but they still need the runtime DLL files beside the executable.

## Platform Support

The current project distribution supports Windows 10/11 x64.

Linux is not currently supported by the provided executable or batch scripts. A Linux port is possible, but it would require a Linux build configuration, Linux-compatible OR-Tools binaries, and replacement of Windows-specific commands and path handling.

## Setup Notes

`setup.bat` installs OR-Tools into:

```text
optimizationLibs/ortools/
```

It does not install Visual Studio. If Visual Studio C++ build tools are missing, the script prints a warning.

clang-format can be installed from LLVM. After installation, verify it with:

```powershell
clang-format --version
```

## Output Files

| Location | Purpose |
| --- | --- |
| `outCSV/jaccard.csv` | Pairwise Jaccard scores. |
| `outCSV/pHash.csv` | Pairwise pHash scores. |
| `outCSV/emd.csv` | Pairwise EMD scores. |
| `outCSV/overall.csv` | Weighted overall pairwise scores. |
| `outCSV/singlePairSimilarity.csv` | Scores for one selected pair. |
| `graphFiles/*.gexf` | Graph files for Gephi visualization. |
| `outputGray/` | Grayscale intermediate images. |
| `resizedImage/` | Resized intermediate images. |
| `dctImages/` | DCT output images. |
| `build/tmp_emd/` | Temporary EMD files. |

## Project Structure

```text
ImCloneDetect/
|-- README.md
|-- setup.bat
|-- run.bat
|-- Makefile
|-- main/
|   |-- main.cpp
|   |-- main.exe
|   `-- *.dll
|-- headers/
|-- implementation/
|-- inputImages/
|-- outCSV/
|-- graphFiles/
|-- graphOutput/
|-- optimizationLibs/
|-- references/
`-- ppt/
```

## Implementation Summary

Current C++ source/header LOC: `1,891`.

| File | Role | LOC |
| --- | --- | ---: |
| `main/main.cpp` | CLI, validation, orchestration, CSV and GEXF generation. | 807 |
| `implementation/BMP.cpp` | BMP reading and writing. | 209 |
| `implementation/Normalizer.cpp` | Grayscale, blur, resizing, and pixel utilities. | 225 |
| `implementation/jaccard.cpp` | Jaccard similarity. | 59 |
| `implementation/pHash.cpp` | DCT and pHash similarity. | 154 |
| `implementation/emd.cpp` | EMD similarity using OR-Tools. | 124 |
| `implementation/formatter.cpp` | clang-format integration. | 65 |
| `headers/*.h` | Public declarations for all modules. | 348 |

## References

1. Y. Wang and D. Liu, "Image-Based Clone Code Detection and Visualization," IEEE, 2019.
2. C. Ragkhitwetsagul, J. Krinke, and B. Marnette, "A picture is worth a thousand words: Code clone detection based on image similarity," IEEE IWSC, 2018.
3. BMP handling reference: `https://solarianprogrammer.com/2018/11/19/cpp-reading-writing-bmp-images/`
4. OR-Tools C++: `https://developers.google.com/optimization/install/cpp`
5. clang-format: `https://clang.llvm.org/docs/ClangFormat.html`
6. Gephi GEXF format: `https://docs.gephi.org/desktop/User_Manual/Import/GEXF_File_Format/`
