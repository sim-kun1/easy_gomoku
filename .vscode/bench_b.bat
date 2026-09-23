@echo off
call "D:\Program Files\Microsoft Visual Studio‚2\Community\VC\Auxiliary\Buildcvars64.bat" >/dev/null 2>&1
cl /nologo /utf-8 /std:c++17 /Fe:cpp\class2\main.exe cpp\class2\main.cpp
