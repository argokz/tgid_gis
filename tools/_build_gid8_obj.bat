@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat" >nul 2>&1
set BOOST_ROOT=H:\cpp\boost_1_85_0
cd /d H:\build\gid8-tgid-gis-20260802
"C:\PROGRA~2\MIB055~1\2022\BUILDT~1\Common7\IDE\COMMON~1\MICROS~1\CMake\Ninja\ninja.exe" CMakeFiles/gid8.dir/gidview/gidrSlot.cpp.obj CMakeFiles/gid8.dir/tu/tu_itog.cpp.obj
