@echo off
setlocal enabledelayedexpansion

:: ============================================================
::  ImCloneDetect - OR-Tools Setup Script
::  Downloads and installs OR-Tools v9.12 for Windows (MSVC)
:: ============================================================

set "ORTOOLS_VER=9.12.4544"
set "ORTOOLS_TAG=v9.12"
set "ZIP_NAME=or-tools_x64_VisualStudio2022_cpp_v%ORTOOLS_VER%.zip"
set "DOWNLOAD_URL=https://github.com/google/or-tools/releases/download/%ORTOOLS_TAG%/%ZIP_NAME%"
set "DEST_DIR=optimizationLibs\ortools"

:: Check if OR-Tools is already installed
if exist "%DEST_DIR%\lib\ortools.lib" (
    echo [OK] OR-Tools is already installed at %DEST_DIR%.
    echo      To reinstall, delete the "%DEST_DIR%" folder and run this script again.
    goto :done
)

echo ============================================================
echo   ImCloneDetect - OR-Tools Setup
echo ============================================================
echo.
echo   Version  : %ORTOOLS_VER%
echo   Download : %DOWNLOAD_URL%
echo.

:: Create destination directories
if not exist "optimizationLibs" mkdir "optimizationLibs"

:: Download the zip file
echo [1/3] Downloading OR-Tools (%ZIP_NAME%)...
echo       This may take a few minutes (~50 MB)...
curl -L -o "%ZIP_NAME%" "%DOWNLOAD_URL%"
if errorlevel 1 (
    echo.
    echo [ERROR] Download failed. Please check your internet connection.
    echo         You can also download manually from:
    echo         %DOWNLOAD_URL%
    echo         and extract it to: %DEST_DIR%
    goto :fail
)
echo       Download complete.

:: Extract the zip file
echo [2/3] Extracting...
powershell -NoProfile -Command "Expand-Archive -Path '%ZIP_NAME%' -DestinationPath 'optimizationLibs\_tmp_ortools' -Force"
if errorlevel 1 (
    echo [ERROR] Extraction failed. Make sure PowerShell is available.
    goto :fail
)

:: Move extracted contents into the correct folder
:: The zip extracts into a subfolder like: or-tools_x64_VisualStudio2022_cpp_v9.12.4544
set "EXTRACTED_DIR=optimizationLibs\_tmp_ortools\or-tools_x64_VisualStudio2022_cpp_v%ORTOOLS_VER%"

if not exist "!EXTRACTED_DIR!" (
    :: Try to find the extracted folder dynamically
    for /d %%D in ("optimizationLibs\_tmp_ortools\or-tools*") do set "EXTRACTED_DIR=%%D"
)

if not exist "!EXTRACTED_DIR!" (
    echo [ERROR] Could not find extracted OR-Tools folder.
    goto :fail
)

:: Move to final destination
if exist "%DEST_DIR%" rd /s /q "%DEST_DIR%"
move "!EXTRACTED_DIR!" "%DEST_DIR%" >nul
if errorlevel 1 (
    echo [ERROR] Failed to move files to %DEST_DIR%.
    goto :fail
)
echo       Extraction complete.

:: Cleanup
echo [3/3] Cleaning up...
if exist "optimizationLibs\_tmp_ortools" rd /s /q "optimizationLibs\_tmp_ortools"
if exist "%ZIP_NAME%" del /q "%ZIP_NAME%"
echo       Cleanup complete.

echo.
echo ============================================================
echo   [OK] OR-Tools v%ORTOOLS_VER% installed successfully!
echo.
echo   Next steps:
echo     1. Open a "Developer Command Prompt for VS" terminal
echo     2. Run: build.bat main
echo     3. Run: main\run.bat
echo ============================================================
goto :done

:fail
echo.
echo Setup failed. Please resolve the error above and try again.
if exist "optimizationLibs\_tmp_ortools" rd /s /q "optimizationLibs\_tmp_ortools"
if exist "%ZIP_NAME%" del /q "%ZIP_NAME%"
exit /b 1

:done
endlocal
