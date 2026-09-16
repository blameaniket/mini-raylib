@echo off
setlocal enabledelayedexpansion

:: ============================================================
:: 1. Locate vcpkg.cmake toolchain
:: ============================================================

:: Visual Studio 2026 Community bundled vcpkg
set "VS_VCPKG_TOOLCHAIN=C:\Program Files\Microsoft Visual Studio\18\Community\VC\vcpkg\scripts\buildsystems\vcpkg.cmake"

if exist "%VS_VCPKG_TOOLCHAIN%" (
    set "TOOLCHAIN_FILE=%VS_VCPKG_TOOLCHAIN%"
    echo [INFO] Found Visual Studio vcpkg toolchain:
    echo        %TOOLCHAIN_FILE%
) else (
    :: Try to find standalone vcpkg through PATH
    for /f "delims=" %%I in ('where.exe vcpkg 2^>nul') do (
        set "VCPKG_EXE=%%I"
        goto :vcpkg_found
    )

    echo [ERROR] vcpkg.cmake toolchain not found!
    echo.
    echo Expected:
    echo %VS_VCPKG_TOOLCHAIN%
    echo.
    echo Make sure vcpkg is installed in Visual Studio Installer.
    exit /b 1
)

goto :toolchain_found

:vcpkg_found

:: Get folder containing vcpkg.exe
for %%I in ("%VCPKG_EXE%") do set "VCPKG_DIR=%%~dpI"

:: Strip trailing backslash
if "%VCPKG_DIR:~-1%"=="\" set "VCPKG_DIR=%VCPKG_DIR:~0,-1%"

:: Check for toolchain relative to vcpkg.exe
if exist "%VCPKG_DIR%\scripts\buildsystems\vcpkg.cmake" (
    set "TOOLCHAIN_FILE=%VCPKG_DIR%\scripts\buildsystems\vcpkg.cmake"
) else if exist "%VCPKG_DIR%\..\scripts\buildsystems\vcpkg.cmake" (
    set "TOOLCHAIN_FILE=%VCPKG_DIR%\..\scripts\buildsystems\vcpkg.cmake"
) else (
    echo [ERROR] Could not locate vcpkg.cmake toolchain!
    exit /b 1
)

:toolchain_found

echo.
echo [INFO] Using vcpkg toolchain:
echo        %TOOLCHAIN_FILE%
echo.

:: ============================================================
:: 2. Create build directory
:: ============================================================

if not exist build mkdir build
cd build

:: ============================================================
:: 3. Run CMake
:: ============================================================

cmake -G "NMake Makefiles" ^
    -DCMAKE_BUILD_TYPE=Debug ^
    -DCMAKE_TOOLCHAIN_FILE="%TOOLCHAIN_FILE%" ^
    ..

if %errorlevel% neq 0 (
    echo.
    echo [ERROR] CMake configuration failed!
    cd ..
    exit /b %errorlevel%
)

:: ============================================================
:: 4. Build with NMake
:: ============================================================

nmake

if %errorlevel% neq 0 (
    echo.
    echo [ERROR] Build failed!
    cd ..
    exit /b %errorlevel%
)

cd ..

echo.
echo [SUCCESS] Build completed successfully!
