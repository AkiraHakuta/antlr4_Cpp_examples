// Example from The Definitive ANTLR 4 Reference by Terence Par 
// find_package(ANTLR REQUIRED)
// antlr_target(TagsGrammarLexer ModeTagsLexer.g4 LEXER)

lexer grammar ModeTagsLexer;

// Default mode rules (the SEA)
OPEN  : '<'     -> mode(ISLAND) ;       // switch to ISLAND mode
TEXT  : ~'<'+ ;                         // clump all text together

mode ISLAND;
CLOSE : '>'     -> mode(DEFAULT_MODE) ; // back to SEA mode 
SLASH : '/' ;
ID    : [a-zA-Z]+ ;                     // match/send ID in tag to parser
