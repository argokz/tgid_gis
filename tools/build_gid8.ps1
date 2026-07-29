# Сборка приложения ТГИД (gid8) под MSVC.
#
# Окружение на этой машине:
#   MSVC и cmake уже есть в составе Visual Studio 2022 Build Tools;
#   Qt ставится в H:\Qt через aqtinstall (download.qt.io недоступен,
#   используется зеркало mirror.yandex.ru);
#   Boost 1.85.0 распакован в H:\cpp\boost_1_85_0 — нужны только заголовки.
#
# PROJ не требуется: proj.h подключается в boost/proj_coord.cpp, но ни одна
# функция PROJ API не вызывается и библиотека нигде не линкуется.
# Заглушка создаётся автоматически, чтобы не тянуть зависимость.

param(
    [string]$Source   = 'H:\projects\tgid-app-new\gid8\gid8',
    [string]$BuildDir = 'H:\build\gid8',
    [string]$QtDir    = 'H:\Qt\6.8.3\msvc2022_64',
    [string]$BoostDir = 'H:\cpp\boost_1_85_0',
    [string]$StubDir  = 'H:\cpp\stub_include',
    [string]$Config   = 'Release',
    [switch]$Configure
)

$ErrorActionPreference = 'Stop'

$vcvars = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\VC\Auxiliary\Build\vcvars64.bat'
if (-not (Test-Path $vcvars)) {
    $vcvars = 'C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat'
}
$cmake = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe'
if (-not (Test-Path $cmake)) {
    $cmake = 'C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe'
}

foreach ($p in @($vcvars, $cmake, $QtDir, $BoostDir)) {
    if (-not (Test-Path $p)) { throw "не найдено: $p" }
}

# Заглушка proj.h — заголовок подключается, но API не используется.
if (-not (Test-Path "$StubDir\proj.h")) {
    New-Item -ItemType Directory -Force -Path $StubDir | Out-Null
    @'
/* Заглушка вместо PROJ.
   boost/proj_coord.cpp подключает <proj.h>, но не вызывает ни одной
   функции PROJ API, и библиотека нигде не линкуется: проекции считает
   Boost.Geometry (srs::transformation). Заголовок нужен только для того,
   чтобы препроцессор нашёл файл. */
#pragma once
'@ | Set-Content -Path "$StubDir\proj.h" -Encoding utf8
    Write-Host "создана заглушка $StubDir\proj.h"
}

New-Item -ItemType Directory -Force -Path $BuildDir | Out-Null

$env:BOOST_ROOT = $BoostDir
$env:CMAKE_PREFIX_PATH = $QtDir

$cfgArgs = @(
    "-S", $Source,
    "-B", $BuildDir,
    "-G", "Ninja",
    "-DCMAKE_BUILD_TYPE=$Config",
    "-DCMAKE_PREFIX_PATH=$QtDir",
    "-DUSE_HASP=OFF",
    "-DUSE_ITWIN=OFF"
)

$bat = Join-Path $env:TEMP 'build_gid8_inner.bat'
$lines = @(
    '@echo off',
    "call `"$vcvars`" >nul",
    "set BOOST_ROOT=$BoostDir",
    # Каталог с заглушкой proj.h добавляем через INCLUDE, а не через
    # CMAKE_CXX_FLAGS: путь со слэшем cmake принимает за исходный каталог
    # и молча игнорирует ("Ignoring extra path from command line").
    "set INCLUDE=$StubDir;%INCLUDE%",
    "`"$cmake`" $($cfgArgs -join ' ')",
    'if errorlevel 1 exit /b 1'
)
if (-not $Configure) {
    $lines += "`"$cmake`" --build `"$BuildDir`" --config $Config"
}
$lines | Set-Content -Path $bat -Encoding ascii

Write-Host "Qt:    $QtDir"
Write-Host "Boost: $BoostDir"
Write-Host "сборка: $BuildDir"
& cmd.exe /c $bat
