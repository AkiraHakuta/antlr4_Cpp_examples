# > mkdir build
# > cd build
# > cmake -G "MinGW Makefiles" ..
# > mingw32-make.exe  
# > test_ModeTags.exe ../test.txt


cmake_minimum_required(VERSION 3.6)

project(ex05) 
set(CMAKE_CXX_STANDARD 17)
add_definitions(-DANTLR4CPP_STATIC)

set(ANTLR4_ROOT C:/antlr4/antlr4-4.13.1)
list(APPEND CMAKE_MODULE_PATH ${ANTLR4_ROOT}/cmake)
set(ANTLR_EXECUTABLE C:/Javalib/antlr-4.13.1-complete.jar)

include_directories(${ANTLR4_ROOT}/include/antlr4-runtime)

add_library(antlr4_static STATIC IMPORTED)
set_target_properties(antlr4_static PROPERTIES IMPORTED_LOCATION ${ANTLR4_ROOT}/lib/libantlr4-runtime-static.a)


find_package(ANTLR REQUIRED)
antlr_target(TagsGrammarLexer ModeTagsLexer.g4 LEXER)

antlr_target(TagsGrammarParser ModeTagsParser.g4 PARSER 
		LIB_DIRECTORY ${ANTLR_TagsGrammarParser_OUTPUT_DIR}
		DEPENDS_ANTLR TagsGrammarLexer
		COMPILE_FLAGS -lib ${ANTLR_TagsGrammarLexer_OUTPUT_DIR})


# include generated files in project environment
include_directories(${ANTLR_TagsGrammarLexer_OUTPUT_DIR})
include_directories(${ANTLR_TagsGrammarParser_OUTPUT_DIR})

add_executable(test_ModeTags test_ModeTags.cpp 
					${ANTLR_TagsGrammarLexer_CXX_OUTPUTS}
					${ANTLR_TagsGrammarParser_CXX_OUTPUTS})
target_link_libraries(test_ModeTags antlr4_static)

