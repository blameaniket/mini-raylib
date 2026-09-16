@echo off


:: Initialize MSVC environment for Visual Studio 18 Build Tools
Call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

doskey run=.\build\app.exe $*
doskey build=.\build_windows.bat $*
cmd /k
