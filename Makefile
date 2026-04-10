SHELL := cmd
.SHELLFLAGS := /C

CXX = cl
CXXFLAGS = /nologo /std:c++20 /EHsc /MD /O2 /IoptimizationLibs/ortools/include
LDFLAGS = /link /LIBPATH:optimizationLibs/ortools/lib ortools.lib
VCVARS ?= C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvars64.bat

TARGET = main\main.exe
SRC = main\main.cpp
IMPL_SRC = implementation\BMP.cpp implementation\Normalizer.cpp implementation\jaccard.cpp implementation\pHash.cpp implementation\emd.cpp
DLL_DIR = optimizationLibs\ortools\bin

.PHONY: all build clean rebuild

all: $(TARGET)

build: all
	@echo Build complete.

rebuild: clean all
	@echo Rebuild complete.

$(TARGET): $(SRC) $(IMPL_SRC)
	if not exist "$(VCVARS)" (echo ERROR: vcvars64.bat not found at "$(VCVARS)". & exit /b 1) else (call "$(VCVARS)" >nul && $(CXX) $(CXXFLAGS) $(SRC) $(IMPL_SRC) /Fe$(TARGET) $(LDFLAGS) && copy /Y $(DLL_DIR)\*.dll main\ >nul)

clean:
	if exist main\main.exe del /q main\main.exe
	if exist main\*.dll del /q main\*.dll
	if exist main\*.obj del /q main\*.obj
	if exist implementation\*.obj del /q implementation\*.obj