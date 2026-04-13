SHELL := cmd
.SHELLFLAGS := /C

CXX := cl
VCVARS ?= C:\Program Files\Microsoft Visual Studio\18\Insiders\VC\Auxiliary\Build\vcvars64.bat
WORKDIR := $(subst /,\,$(CURDIR))

TARGET := main\main.exe
SOURCES := main\main.cpp implementation\BMP.cpp implementation\Normalizer.cpp implementation\jaccard.cpp implementation\pHash.cpp implementation\emd.cpp implementation\formatter.cpp
HEADERS := headers\BMP.h headers\Normalizer.h headers\jaccard.h headers\pHash.h headers\emd.h headers\formatter.h

ORTOOLS_DIR := optimizationLibs\ortools
ORTOOLS_INCLUDE := $(ORTOOLS_DIR)\include
ORTOOLS_LIB := $(ORTOOLS_DIR)\lib
ORTOOLS_BIN := $(ORTOOLS_DIR)\bin
# MSVC still needs temporary .obj files to link multiple .cpp files, so keep them out of the repo.
TEMP_OBJ_DIR := %TEMP%\ImCloneDetect-obj

CXXFLAGS := /nologo /std:c++20 /EHsc /MD /O2 /I$(ORTOOLS_INCLUDE)
LDFLAGS := /link /LIBPATH:$(ORTOOLS_LIB) ortools.lib

.PHONY: all build run clean rebuild

all: build

build: $(TARGET)
	@echo Build complete: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	@if not exist "$(VCVARS)" (echo ERROR: vcvars64.bat not found at "$(VCVARS)". & exit /b 1)
	@if not exist "$(ORTOOLS_LIB)\ortools.lib" (echo ERROR: missing "$(ORTOOLS_LIB)\ortools.lib". & exit /b 1)
	@if not exist "$(ORTOOLS_BIN)\ortools.dll" (echo ERROR: missing "$(ORTOOLS_BIN)\ortools.dll". & exit /b 1)
	@if not exist "$(TEMP_OBJ_DIR)" mkdir "$(TEMP_OBJ_DIR)"
	@call "$(VCVARS)" >nul && cd /d "$(WORKDIR)" && $(CXX) $(CXXFLAGS) /Fo$(TEMP_OBJ_DIR)\ $(SOURCES) /Fe:$(TARGET) $(LDFLAGS) && copy /Y "$(ORTOOLS_BIN)\*.dll" main\ >nul
	@if exist "$(TEMP_OBJ_DIR)\*.obj" del /q "$(TEMP_OBJ_DIR)\*.obj"

run: $(TARGET)
	@cd /d main && main.exe

clean:
	@if exist "$(TARGET)" del /q "$(TARGET)"
	@if exist main\*.dll del /q main\*.dll
	@if exist *.ilk del /q *.ilk
	@if exist *.pdb del /q *.pdb
	@if exist "$(TEMP_OBJ_DIR)\*.obj" del /q "$(TEMP_OBJ_DIR)\*.obj"

rebuild: clean build
