# ImCloneDetect

ImCloneDetect is an image-based code clone detection tool implemented in C++. It compares BMP images of source-code fragments and reports how similar those fragments are by combining three image-similarity methods: Jaccard similarity, perceptual hashing (pHash), and Earth Mover's Distance (EMD). The final score is produced through a configurable weighted average.

The project is designed around the research direction presented in the proposal, midterm presentation, final presentation, and local reference papers. Instead of relying on raw source text only, the tool treats code as an image. This allows the comparison process to capture visual and structural similarity and makes the approach less dependent on programming-language syntax, variable names, and minor formatting changes.

## Project Purpose

The main objective of ImCloneDetect is to detect similar code fragments from their image representations. A user provides BMP images that contain source-code snippets, and the application computes pairwise or single-pair similarity scores. These scores are written to CSV files for tabular inspection and to GEXF graph files for visualization in Gephi.

The project scope follows these assumptions from the proposal and presentation materials:

- Input images should contain source-code fragments.
- Images should be provided in BMP format.
- Code images should preferably represent comparable fragments, such as functions from the same or similar language context.
- Formatting should be normalized before screenshots or image generation when possible.
- Syntax-highlighted code images are suitable inputs for the visual-comparison workflow.

The current executable does not convert source-code files into images automatically. It accepts existing BMP images. The built-in formatter feature can format source files before the user captures or generates images from them.

## Core Features

- Directory-based pairwise similarity calculation for all BMP files in a selected folder.
- Single-pair similarity calculation for two BMP images.
- Multi-algorithm comparison using Jaccard similarity, pHash similarity, and EMD similarity.
- Configurable algorithm weights through the interactive menu.
- CSV output for every algorithm and for the combined overall score.
- GEXF graph output for visualization in Gephi, including ForceAtlas2-style graph analysis workflows.
- Clang-format integration for formatting C and C++ source files before image capture.
- BMP reading, writing, grayscale conversion, Gaussian blur, bilinear resizing, DCT generation, and min-cost-flow-based EMD computation.

## Similarity Methods

### Jaccard Similarity

Jaccard similarity is used as the structural comparison method. The images are converted to grayscale, and the implementation treats nonzero pixels as present code structure. The method pads both images to a common dimension, compares pixel presence at each position, and computes:

```text
similarity = 1 - (different_presence_pixels / pixels_present_in_either_image)
```

This method emphasizes structural overlap. It does not primarily measure pixel intensity; it measures whether meaningful pixels exist in corresponding positions.

Default weight: `0.20`

Relevant files:

- `implementation/jaccard.cpp`
- `headers/jaccard.h`
- `implementation/Normalizer.cpp`
- `headers/Normalizer.h`

### Perceptual Hash Similarity

pHash is used as the frequency-domain comparison method. Each image is resized to `32 x 32`, transformed using the Discrete Cosine Transform (DCT), and reduced to an `8 x 8` low-frequency signature. The DC coefficient is excluded, and the remaining coefficients are converted into a binary hash based on the average coefficient value.

The two hash strings are compared with Hamming distance:

```text
similarity = 1 - (hamming_distance / hash_length)
```

This method is useful for detecting visual resemblance even when minor image-level differences exist.

Default weight: `0.40`

Relevant files:

- `implementation/pHash.cpp`
- `headers/pHash.h`
- `implementation/Normalizer.cpp`
- `headers/Normalizer.h`

### Earth Mover's Distance Similarity

EMD is used as the distribution-based comparison method. The implementation resizes each image to `300 x 300`, converts it to grayscale, applies Gaussian blur, divides it into a `6 x 6` grid, and builds a 36-element signature from regional pixel sums. These signatures are normalized into distributions.

The method then creates a `36 x 36` Euclidean distance matrix between grid-cell centroids and solves a min-cost flow problem with Google OR-Tools. The EMD value is converted into a similarity score:

```text
similarity = max(0, 1 - emd / emd_max)
```

Default weight: `0.40`

Relevant files:

- `implementation/emd.cpp`
- `headers/emd.h`
- `implementation/Normalizer.cpp`
- `headers/Normalizer.h`

## Overall Score

The final similarity score combines all three algorithm scores:

```text
overall = (jaccard_weight * jaccard_score)
        + (phash_weight * phash_score)
        + (emd_weight * emd_score)
```

The default weights are:

```text
Jaccard: 0.20
pHash:   0.40
EMD:     0.40
```

The weights can be changed from the application menu. The three values must each be greater than `0`, less than `1.0`, and must add up to exactly `1.0`.

## Application Workflow

The implemented workflow is:

1. Prepare source-code fragments.
2. Optionally format source files with clang-format.
3. Capture or generate BMP images from the code fragments.
4. Provide the BMP image directory or two BMP image paths to ImCloneDetect.
5. Generate grayscale and resized intermediate images.
6. Compute Jaccard, pHash, and EMD similarity scores.
7. Compute the weighted overall score.
8. Write CSV files to `outCSV/`.
9. Write GEXF graph files to `graphFiles/`.
10. Optionally open the GEXF files in Gephi for visualization.

## Interactive Menu

When the program starts, it displays five options:

```text
1. Find pairwise similarity on a folder
2. Find similarity of two images
3. Format a file or folder
4. Configure algorithm weights
5. Exit
```

For option `1`, enter the full path of a directory containing BMP files.

For option `2`, enter the full path of each BMP image.

For option `3`, enter the full path of a source file or folder. Folder formatting is non-recursive and formats supported source files directly inside the selected folder.

For option `4`, enter new Jaccard, pHash, and EMD weights. The total must be `1.0`.

At supported prompts, type `back` or `b` to return to the main menu.

## Output Files

The application writes its primary outputs into these directories:

| Location | Purpose |
| --- | --- |
| `outCSV/jaccard.csv` | Pairwise Jaccard similarity scores. |
| `outCSV/pHash.csv` | Pairwise pHash similarity scores. |
| `outCSV/emd.csv` | Pairwise EMD similarity scores. |
| `outCSV/overall.csv` | Weighted pairwise overall similarity scores. |
| `outCSV/singlePairSimilarity.csv` | Jaccard, pHash, EMD, and overall scores for one selected image pair. |
| `graphFiles/overall_graph.gexf` | Overall similarity graph for Gephi. |
| `graphFiles/jaccard_graph.gexf` | Jaccard similarity graph for Gephi. |
| `graphFiles/pHash_graph.gexf` | pHash similarity graph for Gephi. |
| `graphFiles/emd_graph.gexf` | EMD similarity graph for Gephi. |
| `outputGray/` | Intermediate grayscale BMP files. |
| `resizedImage/` | Intermediate resized BMP files for pHash. |
| `dctImages/` | DCT visualization images generated during pHash processing. |
| `build/tmp_emd/` | Temporary EMD preprocessing images. |
| `graphOutput/` | Saved graph visualization artifacts and exported graph outputs. |

## Project Structure

```text
ImCloneDetect/
|-- README.md
|-- setup.bat
|-- run.bat
|-- Makefile
|-- main/
|   |-- main.cpp
|   `-- main.exe
|-- headers/
|   |-- BMP.h
|   |-- Normalizer.h
|   |-- jaccard.h
|   |-- pHash.h
|   |-- emd.h
|   `-- formatter.h
|-- implementation/
|   |-- BMP.cpp
|   |-- Normalizer.cpp
|   |-- jaccard.cpp
|   |-- pHash.cpp
|   |-- emd.cpp
|   `-- formatter.cpp
|-- inputImages/
|-- outCSV/
|-- graphFiles/
|-- graphOutput/
|-- outputGray/
|-- resizedImage/
|-- dctImages/
|-- build/
|-- optimizationLibs/
|   `-- ortools/
|-- formatter/
|-- references/
`-- ppt/
```

## Implementation Overview and LOC

The following line counts are physical source lines counted from the current repository state on `2026-04-26`.

| File | Role | LOC |
| --- | --- | ---: |
| `main/main.cpp` | Interactive CLI, menu routing, validation, output directory creation, CSV writing, GEXF generation, pairwise and single-pair orchestration, and weight configuration. | 807 |
| `implementation/BMP.cpp` | BMP file reading, writing, header handling, stride alignment, and 24-bit/32-bit BMP support. | 209 |
| `implementation/Normalizer.cpp` | Grayscale conversion, negative image generation, Gaussian kernel creation, Gaussian blur, pixel extraction, and bilinear resizing. | 225 |
| `implementation/jaccard.cpp` | Pixel matrix normalization and Jaccard similarity calculation. | 59 |
| `implementation/pHash.cpp` | DCT calculation, DCT image generation, perceptual hash generation, and Hamming-distance-based pHash similarity. | 154 |
| `implementation/emd.cpp` | EMD preprocessing, grid signature generation, signature normalization, distance matrix creation, OR-Tools min-cost flow solving, and EMD similarity conversion. | 124 |
| `implementation/formatter.cpp` | clang-format detection and formatting support for individual files and non-recursive folders. | 65 |
| `headers/BMP.h` | BMP file, info, and color-header structures plus BMP class declarations. | 89 |
| `headers/Normalizer.h` | Declarations for image normalization and resizing utilities. | 44 |
| `headers/jaccard.h` | Declarations for pixel vector generation and Jaccard similarity. | 19 |
| `headers/pHash.h` | Declarations for DCT, hash generation, and pHash similarity. | 24 |
| `headers/emd.h` | Declarations for EMD signatures, Euclidean distance, normalization, and EMD similarity. | 62 |
| `headers/formatter.h` | Declarations for clang-format file and folder formatting. | 10 |

Total C++ source/header LOC: `1,891`

## Build Requirements

The project is configured for Windows and MSVC. These requirements apply when building from source. Users who run the already-compiled `main/main.exe` do not need the full Visual Studio compiler toolchain.

Required tools for building:

- Windows 10 or Windows 11, 64-bit.
- Microsoft Visual Studio 2022 or later, or Microsoft Visual Studio Build Tools 2022 or later.
- The `Desktop development with C++` workload, including the MSVC C++ toolset and Windows SDK.
- GNU Make for Windows, available as `make`.
- Google OR-Tools for C++.
- LLVM clang-format, required only for the built-in formatting feature.

The Makefile uses `cl.exe` and links with `optimizationLibs/ortools/lib/ortools.lib`. It also copies required OR-Tools DLL files from `optimizationLibs/ortools/bin/` into `main/` during build.

## Visual Studio Requirement

Yes, users who want to build this project need Visual Studio tooling. OR-Tools for C++ on Windows is distributed for the Visual Studio/MSVC toolchain, and the official OR-Tools Windows guide requires Visual Studio 2022 or later with the C++ toolset. The project also compiles with `cl.exe`, which is provided by Visual Studio or Visual Studio Build Tools.

Users do not need the full Visual Studio IDE if they only want command-line builds. They may install the lighter Visual Studio Build Tools package instead, as long as they select the `Desktop development with C++` workload.

Install one of the following:

- Visual Studio Community 2022 or later with `Desktop development with C++`.
- Visual Studio Build Tools 2022 or later with `Desktop development with C++`.

For a command-line build environment, use the `x64 Native Tools Command Prompt for VS 2022`, or run `make build` with the correct `VCVARS` path. The required file is usually one of:

```text
C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat
C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat
C:\Program Files\Microsoft Visual Studio\2022\Enterprise\VC\Auxiliary\Build\vcvars64.bat
```

Users who only run an already-built `main/main.exe` do not need the full compiler toolchain, but they still need the required OR-Tools DLL files beside the executable and the Microsoft Visual C++ runtime installed. For development, testing, or grading from source, installing Visual Studio or Build Tools is the recommended path.

## Quick Start Options

There are two supported ways to use the project.

### Option 1: Run the Prebuilt Executable

Use this option when `main/main.exe` is already included with the project. This path does not require rebuilding the C++ source code and does not require Visual Studio.

Run from the repository root:

```powershell
.\run.bat
```

The launcher runs `main/main.exe` from the correct working directory. It also checks whether the required OR-Tools DLL files are available beside `main.exe`. If the DLL files are missing but OR-Tools is installed in `optimizationLibs/ortools/bin/`, the launcher copies the DLL files into `main/`.

The prebuilt runtime folder should contain:

```text
main/main.exe
main/ortools.dll
main/abseil_dll.dll
main/libprotobuf.dll
main/libprotobuf-lite.dll
main/libprotoc.dll
main/ortools_flatzinc.dll
main/re2.dll
main/utf8_range.dll
main/utf8_validity.dll
main/zlib1.dll
```

If the project is distributed through a ZIP file, include the files above. If it is distributed through GitHub source control, note that executable and DLL files are usually ignored by `.gitignore`; in that case, attach the prebuilt runtime files as a release ZIP or force-add the required runtime files intentionally.

### Option 2: Build From Source

Use this option when `main/main.exe` is missing, outdated, or must be regenerated from the source code. This path requires Visual Studio or Visual Studio Build Tools with `Desktop development with C++`, plus OR-Tools.

The build sequence is:

```powershell
.\setup.bat
make build
.\run.bat
```

## OR-Tools Setup

The repository includes `setup.bat` for preparing the OR-Tools dependency. It downloads the Visual Studio 2022 C++ binary package for OR-Tools `9.12.4544` and installs it into `optimizationLibs/ortools/`. This script installs OR-Tools only; it does not install Visual Studio.

Run it from the repository root:

```powershell
.\setup.bat
```

If `optimizationLibs/ortools/` is already present with `include/`, `lib/`, and `bin/`, the script exits successfully without downloading anything.

The script also checks whether Visual Studio C++ build tools are detected. If they are missing, it prints a warning because the project cannot be built without them.

If the automatic setup fails, install OR-Tools manually:

1. Open the official OR-Tools C++ binary installation guide for Windows:
   `https://developers.google.com/optimization/install/cpp/binary_windows`
2. Download the Windows x64 C++ binary distribution for Visual Studio 2022.
3. Extract the package.
4. Copy or rename the extracted OR-Tools directory to:

```text
optimizationLibs/ortools/
```

After setup, these paths should exist:

```text
optimizationLibs/ortools/include/
optimizationLibs/ortools/lib/ortools.lib
optimizationLibs/ortools/bin/ortools.dll
```

## Building the Project

Open PowerShell or Command Prompt at the repository root:

```text
d:\ImCloneDetect
```

Build the executable:

```powershell
make build
```

If `make build` reports that `vcvars64.bat` was not found, pass the correct Visual Studio path manually:

```powershell
make build VCVARS="C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
```

For Visual Studio Build Tools, the path is usually:

```powershell
make build VCVARS="C:\Program Files\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat"
```

Other useful build targets:

```powershell
make clean
make rebuild
```

## Running the Project

The preferred run command is:

```powershell
.\run.bat
```

If you have just built the project and want to use the Makefile target, this is also valid:

```powershell
make run
```

You may also run the executable manually from inside the `main/` directory so the program's relative output paths resolve correctly:

```powershell
cd main
.\main.exe
```

Use full image paths when the program asks for input. Example:

```text
D:\ImCloneDetect\inputImages
D:\ImCloneDetect\inputImages\b1.bmp
D:\ImCloneDetect\inputImages\b2.bmp
```

## Installing clang-format

The formatter executable is named `clang-format`. The project presentations refer to it as Clang-formatter, and the program uses it as a formatting tool before image capture.

To install it on Windows:

1. Open the official LLVM download page:
   `https://releases.llvm.org/download.html`
2. Download the current Windows 64-bit LLVM pre-built binary or installer.
3. Run the installer.
4. Enable the option that adds LLVM to `PATH`, if the installer provides it.
5. If the installer does not update `PATH`, add this directory manually:

```text
C:\Program Files\LLVM\bin
```

Verify the installation:

```powershell
clang-format --version
```

After installation, start ImCloneDetect and choose menu option `3` to format a file or folder. The formatter supports these extensions:

```text
.c, .cc, .cpp, .cxx, .h, .hh, .hpp, .hxx
```

The project calls clang-format in-place with:

```text
clang-format -i <file>
```

## Presentation Materials

The `ppt/` directory contains the proposal, midterm, and final presentation materials used to define and explain the project:

- `ppt/Software-Project-Lab-1_proposal.pptx` introduces the project scope, assumptions, workflow, technology stack, and research references.
- `ppt/Software-Project-Lab-1-ImCloneDetect-Mid.pptx` documents the midterm implementation state, including BMP handling, normalization, Jaccard similarity, pHash, and the planned EMD workflow.
- `ppt/Software-Project-Lab-1-ImCloneDetect-Final.pptx` documents the final feature set, including Jaccard, pHash, EMD, weighted scoring, clang-format support, CSV output, and GEXF graph visualization.
- `ppt/tawrat_21.pptx` and `ppt/tawrat_21 (2).pptx` are final presentation variants with the same project narrative and implementation summary.

## References

1. Y. Wang and D. Liu, "Image-Based Clone Code Detection and Visualization," IEEE, 2019, doi: `10.1109/AIAM48774.2019.00041`.
2. C. Ragkhitwetsagul, J. Krinke, and B. Marnette, "A picture is worth a thousand words: Code clone detection based on image similarity," IEEE IWSC, 2018, doi: `10.1109/IWSC.2018.8327318`.
3. Solarian Programmer, "C++ reading and writing BMP images": `https://solarianprogrammer.com/2018/11/19/cpp-reading-writing-bmp-images/`
4. GeeksforGeeks, "Discrete Cosine Transform Algorithm Program": `https://www.geeksforgeeks.org/dsa/discrete-cosine-transform-algorithm-program/`
5. GeeksforGeeks, "Bilinear Interpolation": `https://www.geeksforgeeks.org/maths/what-is-bilinear-interpolation/`
6. Gephi Documentation, "GEXF File Format": `https://docs.gephi.org/desktop/User_Manual/Import/GEXF_File_Format/`
7. Google OR-Tools for C++: `https://developers.google.com/optimization/install/cpp`
8. ClangFormat Documentation: `https://clang.llvm.org/docs/ClangFormat.html`
9. Microsoft Visual Studio Build Tools workload documentation: `https://learn.microsoft.com/en-us/visualstudio/install/workload-component-id-vs-build-tools`
