# > mkdir build
# > cd build
# > cmake -G "MinGW Makefiles" ..
# > mingw32-make.exe  
# > CallGraph.exe ../test.txt
# see result.txt

cmake_minimum_required(VERSION 3.6)

project(CallGraph) 
set(CMAKE_CXX_STANDARD 17)
add_definitions(-DANTLR4CPP_STATIC)

set(ANTLR4_ROOT C:/antlr4/antlr4-4.13.1)
list(APPEND CMAKE_MODULE_PATH ${ANTLR4_ROOT}/cmake)
set(ANTLR_EXECUTABLE C:/Javalib/antlr-4.13.1-complete.jar)

include_directories(${ANTLR4_ROOT}/include/antlr4-runtime)

add_library(antlr4_static STATIC IMPORTED)
set_target_properties(antlr4_static PROPERTIES IMPORTED_LOCATION ${ANTLR4_ROOT}/lib/libantlr4-runtime-static.a)

find_package(ANTLR REQUIRED)
antlr_target(CymbolGrammar Cymbol.g4 LISTENER)
include_directories(${ANTLR_CymbolGrammar_OUTPUT_DIR})

add_executable(CallGraph CallGraph.cpp ${ANTLR_CymbolGrammar_CXX_OUTPUTS})
target_link_libraries(CallGraph antlr4_static)
