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
    |   NUMBER      # Atom
    |   'true'      # Atom
    |   'false'     # Atom
    |   'null'      # Atom
    ;

object
    :   '{' '}'                         # EmptyObject
    |   '{' member (',' member)*  '}'   # AnObject
    ;

array
    :   '[' ']'                          # EmptyArray
    |   '[' element (',' element)* ']'   # ArrayOfValues
    ;

member: STRING ':' element
    ;


LCURLY : '{' ;
LBRACK : '[' ;
STRING :  '"' (ESC | ~["\\])* '"' ;

fragment ESC :   '\\' (["\\/bfnrt] | UNICODE) ;
fragment UNICODE : 'u' HEX HEX HEX HEX ;
fragment HEX : [0-9a-fA-F] ;

NUMBER
    :   '-'? INT '.' INT EXP?   // 1.35, 1.35E-9, 0.3, -4.5
    |   '-'? INT EXP            // 1e10 -3e4
    |   '-'? INT                // -3, 45
    ;
fragment INT :   '0' | '1'..'9' '0'..'9'* ; // no leading zeros
fragment EXP :   [Ee] [+\-]? INT ; // \- since - means "range" inside [...]

WS  :   [ \t\n\r]+ -> skip ;
