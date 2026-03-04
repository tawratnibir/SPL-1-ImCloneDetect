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
   This automatically downloads and installs OR-Tools v9.12 into `optimizationLibs/ortools/`.

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

1. Place your BMP images in the `inputImages/` directory.
2. Run `main\run.bat`.
3. Choose an option from the menu:
   - **1** - Jaccard similarity for all pairs
   - **2** - pHash similarity for all pairs
   - **3** - EMD similarity for all pairs
   - **4** - Overall weighted score (20% Jaccard + 40% pHash + 40% EMD)
   - **5** - Single pair similarity (enter two filenames)
   - **6** - Generate GEXF graph (for [Gephi](https://gephi.org/) visualization)
   - **7** - Exit

## Output

- CSV results are saved in `outCSV/` (jaccard2.csv, pHash2.csv, emd2.csv, overall2.csv)
- GEXF graph file is saved in `graphJson/overall_graph.gexf`

## Project Structure

```
ImCloneDetect/
├── setup.bat          # Downloads OR-Tools dependency
├── build.bat          # Builds the project (calls nmake)
├── Makefile           # Build rules
├── main/
│   ├── main.cpp       # Main application
│   └── run.bat        # Runs the compiled program
├── headers/           # Header files (BMP, pHash, jaccard, emd, Normalizer)
├── implementation/    # Implementation files
├── inputImages/       # Place your BMP images here
├── outCSV/            # CSV output directory
└── graphFiles/         # GEXF graph output directory
```