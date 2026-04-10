# ImCloneDetect

Image clone detection tool that compares BMP images using three similarity metrics:
- **Jaccard Similarity** (20% weight)
- **Perceptual Hash (pHash)** (40% weight)
- **Earth Mover's Distance (EMD)** (40% weight)

## Prerequisites

- **Windows 10/11** (x64)
- **Visual Studio 2022** with C++ Desktop Development workload
  (provides the MSVC compiler `cl.exe` and `nmake`)

## Setup (First Time)

1. **Clone the repository:**
   ```
   git clone https://github.com/<your-username>/ImCloneDetect.git
   cd ImCloneDetect
   ```

2. **Download OR-Tools** (required dependency, ~50 MB download):
   ```
   setup.bat
   ```
   This downloads and installs OR-Tools v9.12 into `optimizationLibs/ortools/`.

3. **Build the project:**
   Open a **Developer Command Prompt for VS 2022** (or run `vcvars64.bat`), then:
   ```
   .\build.bat main
   ```

4. **Run:**
   ```
   main\run.bat
   ```

## Usage

1. Put your `.bmp` images in a folder inside the repository (for example: `inputImages/`).
2. Run `main\run.bat`.
3. Choose from the menu:
   - **1** - Pairwise similarity for all `.bmp` files in a directory
     - Enter a directory name relative to repo root (example: `inputImages`)
     - Computes Jaccard, pHash, EMD, overall score, and GEXF graph
   - **2** - Similarity of two images
     - Enter two file names from `inputImages/` (example: `b1.bmp`, `b2.bmp`)
     - Computes Jaccard, pHash, EMD, and overall score
   - **3** - Exit

## Output

- Pairwise CSV files are saved in `outCSV/`:
  - `jaccard.csv`
  - `pHash.csv`
  - `emd.csv`
  - `overall.csv`
- Single pair result is saved in `outCSV/singlePairSimilarity.csv`
- GEXF graph file is saved in `graphFiles/overall_graph.gexf`

## Project Structure

```
ImCloneDetect/
|-- setup.bat          # Downloads OR-Tools dependency
|-- build.bat          # Builds the project (calls nmake)
|-- Makefile           # Build rules
|-- main/
|   |-- main.cpp       # Main application
|   `-- run.bat        # Runs the compiled program
|-- headers/           # Header files
|-- implementation/    # Implementation files
|-- inputImages/       # Input BMP images
|-- outCSV/            # CSV output directory
`-- graphFiles/        # GEXF graph output directory
```
