// find_package(ANTLR REQUIRED)
// antlr_target(ExprGrammar SimpleExpr2.g4 LISTENER)

grammar SimpleExpr2;

// parser rules
stat : expr ;
expr : <assoc=right> expr EXPO expr # Expo
     | expr MULT expr               # Mult
     | expr ADD  expr               # Add
     | INT                          # Int
     ;


// lexer rules
EXPO : '^' ;
MULT : '*' ;
ADD  : '+' ;
INT : [0-9]+ ;
WS : [ \t\n\r]+ -> skip ;
