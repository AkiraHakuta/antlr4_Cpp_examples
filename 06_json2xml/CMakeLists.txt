# $ mkdir build
# $ cd build
# $ cmake -G "MinGW Makefiles" ..
# $ mingw32-make.exe 
# $ json2xml.exe ../test.json

cmake_minimum_required(VERSION 3.6)
project(json2xml) 
set(CMAKE_CXX_STANDARD 11)

add_definitions(-DANTLR4CPP_STATIC)

set(ANTLR4_ROOT C:/mingw-w64/x86_64-8.1.0-posix-seh-rt_v6-rev0/mingw64/local)
list(APPEND CMAKE_MODULE_PATH ${ANTLR4_ROOT}/cmake)
set(ANTLR_EXECUTABLE C:/Javalib/antlr-4.7.2-complete.jar)

include_directories(${ANTLR4_ROOT}/include/antlr4-runtime)
add_library(antlr4_static STATIC IMPORTED)
set_target_properties(antlr4_static PROPERTIES IMPORTED_LOCATION ${ANTLR4_ROOT}/lib/libantlr4-runtime.a)

find_package(ANTLR REQUIRED)
antlr_target(JsonGrammar JSON.g4 LISTENER)
include_directories(${ANTLR_JsonGrammar_OUTPUT_DIR})

add_executable(json2xml json2xml.cpp ${ANTLR_JsonGrammar_CXX_OUTPUTS})
target_link_libraries(json2xml antlr4_static)
