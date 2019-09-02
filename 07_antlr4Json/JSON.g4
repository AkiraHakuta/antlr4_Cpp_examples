// Derived from http://json.org
// find_package(ANTLR REQUIRED)
// antlr_target(JsonGrammar JSON.g4 LISTENER)

grammar JSON;

json:   element
    ;

element
    :   value
    ;

value
    :   object      # ObjectValue
    |   array       # ArrayValue
    |   STRING      # String
    |   DOUBLE      # Double 
    |   INTEXP      # IntExp
    |   INTEGER     # Integer
    |   'true'      # Bool
    |   'false'     # Bool
    |   'null'      # Null
    ;

object
    :   '{' '}' ','?                        # EmptyObject
    |   '{' member (',' member)*  '}' ','?  # AnObject
    ;

array
    :   '[' ']' ','?                         # EmptyArray
    |   '[' element (',' element)* ']' ','?  # ArrayOfValues
    ;

member: STRING ':' element
    ;


LCURLY : '{' ;
LBRACK : '[' ;
STRING :  '"' (ESC | ~["\\])* '"' ;

fragment ESC :   '\\' (["\\/bfnrt] | UNICODE) ;
fragment UNICODE : 'u' HEX HEX HEX HEX ;
fragment HEX : [0-9a-fA-F] ;

// NUMBER
DOUBLE : '-'? INT '.' INT EXP?  ;
INTEXP : '-'? INT EXP           ;
INTEGER: '-'? INT               ;

fragment INT :   '0' | '1'..'9' '0'..'9'* ; // no leading zeros
fragment EXP :   [Ee] [+\-]? INT ; // \- since - means "range" inside [...]

WS  :   [ \t\n\r]+ -> skip ;

LINE_COMMENT : '//' .*? '\n' -> skip ;
COMMENT :   '/*' .*? '*/'    -> skip ;
