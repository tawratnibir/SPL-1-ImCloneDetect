@echo off
setlocal

set "ROOT=%~dp0"
set "MAIN_DIR=%ROOT%main"
set "EXE=%MAIN_DIR%\main.exe"
set "ORTOOLS_BIN=%ROOT%optimizationLibs\ortools\bin"

if not exist "%EXE%" (
    echo ERROR: Prebuilt executable was not found:
    echo   %EXE%
    echo.
    echo Build it with:
    echo   make build
    exit /b 1
)

if not exist "%MAIN_DIR%\ortools.dll" (
    if exist "%ORTOOLS_BIN%\ortools.dll" (
        echo Runtime DLLs are missing from main\. Copying them from optimizationLibs\ortools\bin...
        copy /Y "%ORTOOLS_BIN%\*.dll" "%MAIN_DIR%\" >nul
        if errorlevel 1 (
            echo ERROR: Failed to copy OR-Tools DLL files.
            exit /b 1
        )
    ) else (
        echo ERROR: Required OR-Tools runtime DLLs were not found beside main.exe.
        echo Expected:
        echo   %MAIN_DIR%\ortools.dll
        echo.
        echo Run setup.bat to install OR-Tools, or distribute main.exe with the DLL files in main\.
        exit /b 1
    )
)

pushd "%MAIN_DIR%" >nul
main.exe
set "EXIT_CODE=%ERRORLEVEL%"
popd >nul

exit /b %EXIT_CODE%
