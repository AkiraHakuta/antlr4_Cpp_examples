# > mkdir build
# > cd build
# > cmake -G "MinGW Makefiles" ..
# > mingw32-make.exe  
# > test_SimpleExpr1.exe ../test.txt

cmake_minimum_required(VERSION 3.6)

project(ex01) 
set(CMAKE_CXX_STANDARD 17)
add_definitions(-DANTLR4CPP_STATIC)

set(ANTLR4_ROOT C:/antlr4/antlr4-4.13.1)
list(APPEND CMAKE_MODULE_PATH ${ANTLR4_ROOT}/cmake)
set(ANTLR_EXECUTABLE C:/Javalib/antlr-4.13.1-complete.jar)

include_directories(${ANTLR4_ROOT}/include/antlr4-runtime)

add_library(antlr4_static STATIC IMPORTED)

set_target_properties(antlr4_static PROPERTIES IMPORTED_LOCATION ${ANTLR4_ROOT}/lib/libantlr4-runtime-static.a)

find_package(ANTLR REQUIRED)
antlr_target(ExprGrammar SimpleExpr1.g4 VISITOR)
include_directories(${ANTLR_ExprGrammar_OUTPUT_DIR})

add_executable(test_SimpleExpr1 test_SimpleExpr1.cpp ${ANTLR_ExprGrammar_CXX_OUTPUTS})
target_link_libraries(test_SimpleExpr1 antlr4_static)
