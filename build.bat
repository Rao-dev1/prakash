@echo off
setlocal
cd /d "%~dp0"

echo ========================================================
echo          Building PRAKASH-DS C++ Core Engine
echo ========================================================

g++ -O3 -std=c++17 cpp/*.cpp -o prakash_ds.exe

if %ERRORLEVEL% EQU 0 (
    echo [OK] Compilation successful: prakash_ds.exe generated!
) else (
    echo [ERROR] Compilation failed!
    exit /b %ERRORLEVEL%
)
