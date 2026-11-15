@echo off
setlocal
cd /d "%~dp0"
title PRAKASH-DS Predictive Burn-in Screening System

echo ========================================================
echo                  PRAKASH-DS v1.0
echo     Burn-in Component Screening & Prioritization System
echo        (Data Structures & Algorithms Course Project)
echo ========================================================
echo.

REM Detect Python
set PY_CMD=
where python >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    set PY_CMD=python
) else (
    where py >nul 2>nul
    if %ERRORLEVEL% EQU 0 (
        set PY_CMD=py
    )
)

if "%PY_CMD%"=="" (
    echo [ERROR] Python was not found on your system PATH!
    echo Please make sure Python is installed and added to PATH.
    pause
    exit /b 1
)

REM Check if C++ binary exists, build if missing
if not exist "prakash_ds.exe" (
    echo [*] prakash_ds.exe not found. Compiling C++ core...
    call build.bat
    if %ERRORLEVEL% NEQ 0 (
        echo [ERROR] Compilation failed.
        pause
        exit /b 1
    )
)

echo [*] Starting PRAKASH-DS Server...
echo [*] Your default browser will open automatically at: http://localhost:8000
echo [*] Keep this command prompt window OPEN while using the dashboard.
echo [*] Press Ctrl+C in this window when you want to stop the server.
echo.

%PY_CMD% server/server.py

pause
