// find_package(ANTLR REQUIRED)
// antlr_target(ExprGrammar SimpleExpr1.g4 VISITOR)

grammar SimpleExpr1;

stat : expr ;

expr : <assoc=right> expr EXPO expr # Expo
     | expr MULT expr               # Mult
     | expr ADD  expr               # Add
     | INT                          # Int
     ;

EXPO : '^' ;
MULT : '*' ;
ADD  : '+' ;
INT : [0-9]+ ;
WS : [ \t\n\r]+ -> skip ;
