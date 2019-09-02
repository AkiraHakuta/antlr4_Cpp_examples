// find_package(ANTLR REQUIRED)
// antlr_target(ExprGrammar SimpleExpr3.g4 LISTENER)

grammar SimpleExpr3;

stat : expr ;

expr locals[int value]
    : <assoc=right> expr EXPO expr # Expo
     | expr MULT expr              # Mult
     | expr ADD  expr              # Add
     | INT                         # Int
     ;

EXPO : '^' ;
MULT : '*' ;
ADD  : '+' ;
INT : [0-9]+ ;
WS : [ \t\n\r]+ -> skip ;
