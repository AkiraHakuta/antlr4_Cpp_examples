# > mkdir build
# > cd build
# > cmake -G "MinGW Makefiles" ..
# > mingw32-make.exe  
# > test_SimpleExpr2.exe ../test.txt

cmake_minimum_required(VERSION 3.6)

project(ex02) 

set(CMAKE_CXX_STANDARD 17)
add_definitions(-DANTLR4CPP_STATIC)

set(ANTLR4_ROOT C:/antlr4/antlr4-4.13.1)
list(APPEND CMAKE_MODULE_PATH ${ANTLR4_ROOT}/cmake)
set(ANTLR_EXECUTABLE C:/Javalib/antlr-4.13.1-complete.jar)

include_directories(${ANTLR4_ROOT}/include/antlr4-runtime)

add_library(antlr4_shared SHARED IMPORTED)
set(ANTLR4_SHARED_DLL ${ANTLR4_ROOT}/bin/libantlr4-runtime.dll)
set_target_properties(antlr4_shared PROPERTIES IMPORTED_IMPLIB ${ANTLR4_SHARED_DLL})

find_package(ANTLR REQUIRED)
antlr_target(ExprGrammar SimpleExpr2.g4 LISTENER)
include_directories(${ANTLR_ExprGrammar_OUTPUT_DIR})

add_executable(test_SimpleExpr2 test_SimpleExpr2.cpp ${ANTLR_ExprGrammar_CXX_OUTPUTS})
target_link_libraries(test_SimpleExpr2 antlr4_shared)

EXECUTE_PROCESS( 	
	COMMAND ${CMAKE_COMMAND} -E copy ${ANTLR4_SHARED_DLL} ${CMAKE_CURRENT_BINARY_DIR}
	COMMAND ${CMAKE_COMMAND} -E 
		echo "${CMAKE_COMMAND} -E copy ${ANTLR4_SHARED_DLL} ${CMAKE_CURRENT_BINARY_DIR}"
	)
	