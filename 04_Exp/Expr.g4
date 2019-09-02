//find_package(ANTLR REQUIRED)
//antlr_target(ExprGrammar Expr.g4 VISITOR)

grammar Expr;

prog:   stat+ ; 

stat:   expr NEWLINE          # returnValue            
    |   ID '=' expr NEWLINE   # assignment    
    |   NEWLINE               # ignore
    ;

expr:  <assoc=right> expr '^' expr # expo
    |  expr op=('*'|'/') expr      # mul_div
    |  op=('+'|'-') expr           # pm_expr
    |   expr op=('+'|'-') expr     # add_sub
    |   FLOAT                      # float
    |   INT                        # integer
    |   ID                         # var
    |   '(' expr ')'               # paren
    ;

ID  : ALPHABET(ALPHABET|DIGIT)* ;
FLOAT:  DIGIT*  '.' DIGIT+ ;
INT : DIGIT+ ;
fragment ALPHABET: [a-zA-Z] ;
fragment DIGIT: [0-9] ;
NEWLINE:'\r'? '\n' ;
WS  :   [ \t]+ -> skip ;
