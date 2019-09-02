// Example from The Definitive ANTLR 4 Reference by Terence Par 
// find_package(ANTLR REQUIRED)
// antlr_target(TagsGrammarParser ModeTagsParser.g4 PARSER 
// 		LIB_DIRECTORY ${ANTLR_TagsGrammarLexer_OUTPUT_DIR}
// 		DEPENDS_ANTLR TagsGrammarLexer)

parser grammar ModeTagsParser;

options { tokenVocab=ModeTagsLexer; } // use tokens from ModeTagsLexer.g4

file: (tag | TEXT)* ;

tag : '<' ID '>'
    | '<' '/' ID '>'
    ;
