@echo off
rem ============================================================
rem  Build with VS2022 MSVC (cl.exe)
rem  Usage: cl-build.bat <active-file-relative-to-workspace>
rem  Behavior:
rem   1. If the folder of the active file contains main.c / main.cpp,
rem      THAT file is the program entry and gets compiled, no matter
rem      which tab is active (folder == one program).
rem   2. Otherwise the active file itself is compiled.
rem   3. If exe exists and is newer than every .c/.cpp in the folder,
rem      the build is skipped (fast F5 when nothing changed).
rem  NOTE: keep this file pure ASCII (cmd parses bat with GBK)
rem ============================================================
setlocal EnableDelayedExpansion
if "%~1"=="" (
    echo [cl-build] error: no source file argument
    exit /b 1
)
set "ACTIVE=%~f1"
if not exist "%ACTIVE%" (
    echo [cl-build] error: source file not found: "%ACTIVE%"
    exit /b 1
)
for %%F in ("%ACTIVE%") do (
    set "DIR=%%~dpF"
    set "ANAME=%%~nF"
)

rem ---- pick the file to compile: prefer main.* in this folder ----
set "SRC=%ACTIVE%"
if /I not "%ANAME%"=="main" (
    if exist "%DIR%main.cpp" set "SRC=%DIR%main.cpp"
    if exist "%DIR%main.c" set "SRC=%DIR%main.c"
)
for %%F in ("%SRC%") do set "BASENAME=%%~nF"
rem output exe is named after the ACTIVE file so launch.json
rem (fileBasenameNoExtension) finds it: compiling main.cpp from the
rem question1.cpp tab produces question1.exe
set "BASENAME=%ANAME%"
set "EXE=%DIR%%BASENAME%.exe"

rem ---- up-to-date check: skip if exe newer than every source -----
if not exist "%EXE%" goto need_build
for %%F in ("%EXE%") do set "EXET=%%~tF"
for %%S in ("%DIR%*.c" "%DIR%*.cpp") do (
    set "SRCT=%%~tS"
    rem string compare, same date format on both sides, minute precision
    if "!SRCT!" GEQ "!EXET!" goto need_build
)
echo [cl-build] up to date, skip build
exit /b 0

:need_build
call "D:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
if errorlevel 1 (
    echo [cl-build] error: failed to call vcvars64.bat
    exit /b 1
)
cd /d "%DIR%"

set "STD=/std:c++17"
set "EH=/EHsc"
for %%F in ("%SRC%") do if /I "%%~xF"==".c" (
    set "STD=/std:c17"
    set "EH="
)

rem /MD (dynamic CRT) is REQUIRED: cl.exe defaults to /MT, but EGE's
rem prebuilt graphics.lib is built against the DLL CRT, so /MT leaves
rem __imp_* symbols unresolved (LNK2019) and no exe is produced.
cl /nologo /utf-8 /MD /W3 /Zi %EH% %STD% /Fe:"%BASENAME%.exe" "%SRC%"
exit /b %errorlevel%
