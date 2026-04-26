@echo off
setlocal

set "ROOT=%~dp0"
set "ORTOOLS_TAG=v9.12"
set "ORTOOLS_VERSION=9.12.4544"
set "ORTOOLS_ARCHIVE=or-tools_x64_VisualStudio2022_cpp_v%ORTOOLS_VERSION%.zip"
set "ORTOOLS_URL=https://github.com/google/or-tools/releases/download/%ORTOOLS_TAG%/%ORTOOLS_ARCHIVE%"
set "DEST_DIR=%ROOT%optimizationLibs\ortools"
set "DOWNLOAD_DIR=%ROOT%build\downloads"
set "EXTRACT_DIR=%ROOT%build\ortools_extract"
set "ZIP_PATH=%DOWNLOAD_DIR%\%ORTOOLS_ARCHIVE%"
set "EXTRACTED_ROOT=%EXTRACT_DIR%\or-tools_x64_VisualStudio2022_cpp_v%ORTOOLS_VERSION%"

echo === ImCloneDetect dependency setup ===
echo.

if exist "%DEST_DIR%\lib\ortools.lib" if exist "%DEST_DIR%\bin\ortools.dll" (
    echo OR-Tools is already installed at:
    echo   %DEST_DIR%
    echo.
    goto :success
)

where powershell >nul 2>nul
if errorlevel 1 (
    echo ERROR: PowerShell is required to download and extract OR-Tools.
    exit /b 1
)

if not exist "%DOWNLOAD_DIR%" mkdir "%DOWNLOAD_DIR%"

if not exist "%ZIP_PATH%" (
    echo Downloading OR-Tools %ORTOOLS_VERSION% for Visual Studio 2022...
    echo   %ORTOOLS_URL%
    powershell -NoProfile -ExecutionPolicy Bypass -Command "[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; Invoke-WebRequest -Uri '%ORTOOLS_URL%' -OutFile '%ZIP_PATH%'"
    if errorlevel 1 (
        echo ERROR: OR-Tools download failed.
        echo Download it manually from:
        echo   %ORTOOLS_URL%
        exit /b 1
    )
) else (
    echo Using existing archive:
    echo   %ZIP_PATH%
)

if exist "%EXTRACT_DIR%" rmdir /s /q "%EXTRACT_DIR%"
mkdir "%EXTRACT_DIR%"

echo Extracting OR-Tools...
powershell -NoProfile -ExecutionPolicy Bypass -Command "Expand-Archive -LiteralPath '%ZIP_PATH%' -DestinationPath '%EXTRACT_DIR%' -Force"
if errorlevel 1 (
    echo ERROR: OR-Tools extraction failed.
    exit /b 1
)

if not exist "%EXTRACTED_ROOT%\lib\ortools.lib" (
    echo ERROR: Extracted OR-Tools package does not contain lib\ortools.lib.
    exit /b 1
)

if not exist "%EXTRACTED_ROOT%\bin\ortools.dll" (
    echo ERROR: Extracted OR-Tools package does not contain bin\ortools.dll.
    exit /b 1
)

if not exist "%DEST_DIR%" mkdir "%DEST_DIR%"

echo Installing OR-Tools into:
echo   %DEST_DIR%
xcopy "%EXTRACTED_ROOT%\*" "%DEST_DIR%\" /E /I /Y >nul
if errorlevel 2 (
    echo ERROR: Failed to copy OR-Tools files.
    exit /b 1
)

if not exist "%DEST_DIR%\lib\ortools.lib" (
    echo ERROR: Installation verification failed. Missing lib\ortools.lib.
    exit /b 1
)

if not exist "%DEST_DIR%\bin\ortools.dll" (
    echo ERROR: Installation verification failed. Missing bin\ortools.dll.
    exit /b 1
)

:success
echo.
call :check_visual_studio
echo.
echo Setup complete.
echo You can now build with:
echo   make build
exit /b 0

:check_visual_studio
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set "VSINSTALL="

if exist "%VSWHERE%" (
    for /f "usebackq tokens=*" %%I in (`"%VSWHERE%" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "VSINSTALL=%%I"
)

if defined VSINSTALL (
    if exist "%VSINSTALL%\VC\Auxiliary\Build\vcvars64.bat" (
        echo Visual Studio C++ build tools detected:
        echo   %VSINSTALL%
        exit /b 0
    )
)

echo WARNING: Visual Studio C++ build tools were not detected.
echo To build this project, install Visual Studio 2022 or Visual Studio Build Tools 2022
echo with the "Desktop development with C++" workload.
echo setup.bat installs OR-Tools only; it does not install Visual Studio.
exit /b 0
