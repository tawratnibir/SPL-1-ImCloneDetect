CXX = cl
CXXFLAGS = /nologo /std:c++20 /EHsc /MD /O2 /IoptimizationLibs/ortools/include /Fo$(BUILD_DIR)\\
LDFLAGS = /link /LIBPATH:optimizationLibs/ortools/lib ortools.lib

# Build directory
BUILD_DIR = build

# Source files
IMPL_SRC = implementation\BMP.cpp implementation\Normalizer.cpp implementation\jaccard.cpp implementation\pHash.cpp implementation\emd.cpp

# Targets
TARGET_MAIN = $(BUILD_DIR)\main.exe
TARGET_EMD2 = $(BUILD_DIR)\emd2.exe
TARGET_TEST_EMD = $(BUILD_DIR)\testEMD.exe

all: $(TARGET_TEST_EMD)
	@echo Running testEMD...
	@cd $(BUILD_DIR) && testEMD.exe

DLL_DIR = optimizationLibs\ortools\bin

main: $(TARGET_MAIN)
	@copy $(TARGET_MAIN) main\main.exe >nul 2>&1
	@echo Running main...
	@for /f %%i in ('cd') do @set "PATH=%%i\$(DLL_DIR);%PATH%" && cd main && main.exe

$(TARGET_MAIN): main\main.cpp $(IMPL_SRC)
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) main\main.cpp $(IMPL_SRC) /Fe:$(TARGET_MAIN) $(LDFLAGS)
	@copy optimizationLibs\ortools\bin\*.dll $(BUILD_DIR)\ >nul 2>&1
	@if exist main\*.obj del /q main\*.obj
	@if exist practice\*.obj del /q practice\*.obj

testEMD: $(TARGET_TEST_EMD)
	@echo Running testEMD...
	@cd $(BUILD_DIR) && testEMD.exe

$(TARGET_TEST_EMD): main\testEMD.cpp $(IMPL_SRC)
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) main\testEMD.cpp $(IMPL_SRC) /Fe:$(TARGET_TEST_EMD) $(LDFLAGS)
	@copy optimizationLibs\ortools\bin\*.dll $(BUILD_DIR)\ >nul 2>&1
	@if exist main\*.obj del /q main\*.obj
	@if exist practice\*.obj del /q practice\*.obj

emd2: $(TARGET_EMD2)
	@echo Running emd2...
	@cd $(BUILD_DIR) && emd2.exe

$(TARGET_EMD2): practice\emd2.cpp $(IMPL_SRC)
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) practice\emd2.cpp $(IMPL_SRC) /Fe:$(TARGET_EMD2) $(LDFLAGS)
	@copy optimizationLibs\ortools\bin\*.dll $(BUILD_DIR)\ >nul 2>&1
	@if exist main\*.obj del /q main\*.obj
	@if exist practice\*.obj del /q practice\*.obj

clean:
	@if exist $(BUILD_DIR) rd /s /q $(BUILD_DIR)
	@if exist main\*.obj del /q main\*.obj
	@if exist practice\*.obj del /q practice\*.obj
	@echo Build directory cleaned.