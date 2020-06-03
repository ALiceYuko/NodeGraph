@echo off
rem
rem CMake wrapper for current project, to ease the process of generating
rem project files or Makefiles
rem
rem Usage: run_cmake.bat <build_dir> <debug_runtime_output_dir> <runtime_output_dir> <source_dir> <cmake_generator>
rem
setlocal

set PWD=%cd%.

set BUILD_ARCH=%~1
if "%BUILD_ARCH%"=="" set BUILD_ARCH=x64

if "%BUILD_ARCH%"=="x64" (
	set CMAKE_GENERATOR=Visual Studio 15 2017 Win64
	set CMAKE_DEBUG_OUTPUT=%PWD%\bin\win\x64\Debug
	set CMAKE_RUNTIME_OUTPUT=%PWD%\bin\win\x64\Release
)

if "%BUILD_ARCH%"=="x86" (
	set CMAKE_GENERATOR=Visual Studio 15 2017
	set CMAKE_DEBUG_OUTPUT=%PWD%\bin\win\x86\Debug
	set CMAKE_RUNTIME_OUTPUT=%PWD%\bin\win\x86\Release
)

set CMAKE_BUILD_DIR=%~2
if "%CMAKE_BUILD_DIR%"=="" set CMAKE_BUILD_DIR=%PWD%\build

set SOURCE_DIR=%~3
if "%SOURCE_DIR%"=="" set SOURCE_DIR=%PWD%


if not exist "%CMAKE_BUILD_DIR%" mkdir "%CMAKE_BUILD_DIR%"
pushd "%CMAKE_BUILD_DIR%" && ^
cmake -G "%CMAKE_GENERATOR%" ^
    -Wno-dev ^
    -DCMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG="%CMAKE_DEBUG_OUTPUT%" ^
    -DCMAKE_RUNTIME_OUTPUT_DIRECTORY="%CMAKE_RUNTIME_OUTPUT%" ^
    -DCMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO="%CMAKE_RUNTIME_OUTPUT%" ^
    -DCMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE="%CMAKE_RUNTIME_OUTPUT%" ^
    -DCMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL="%CMAKE_RUNTIME_OUTPUT%" ^
    "%SOURCE_DIR%" && ^
popd

endlocal

pause
