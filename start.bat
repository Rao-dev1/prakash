@echo off
title PRAKASH-DS Predictive Burn-in Screening System
echo ========================================================
echo                  PRAKASH-DS v1.0
echo     Burn-in Component Screening & Prioritization System
echo        (Data Structures & Algorithms Course Project)
echo ========================================================

REM Check if C++ binary is built
if not exist "prakash_ds.exe" (
    echo [*] prakash_ds.exe not found. Building C++ core...
    call build.bat
    if %ERRORLEVEL% NEQ 0 (
        echo [-] Compilation failed. Aborting.
        pause
        exit /b 1
    )
)

echo [*] Launching PRAKASH-DS Web Server at http://localhost:8000 ...
echo [*] Opening default web browser...

start http://localhost:8000
python server/server.py

pause
