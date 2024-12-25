grammar scheme;

scheme
    : expression+ EOF
    ;

// Punctuation Tokens
LPAREN : '(' ;
RPAREN : ')' ;
DOT : '.' ;
QUOTE : '\'' ;
BACKTICK : '`' ;
COMMA : ',' ;
COMMA_AT : ',@' ;

// expression: the core building block
expression
    : ATOMIC_SYMBOL
    | NUMBER
    | BOOLEAN
    | CHARACTER
    | STRING
    | quotedExpression
    | quasiquotedExpression
    | unquotedExpression
    | dottedPair
    | list
    ;


// list: a sequence of expressions, including empty lists
list
    : '(' (expression* '.')? expression? ')' // Allows both regular and dotted lists
    ;

// dotted Pair: A pair separated by a dot
dottedPair
    : '(' expression '.' expression ')'
    ;

// sirve para crear listas sin evaluarlas
// para poder usarlas como datos
// no admite la evaluación selectiva con ,
quotedExpression
    : '\'' expression // Example: '(a b c)
    ;

// sirve para crear listas sin evaluarlas
// para poder usarlas como datos
// con , podemos evaluar expresiones selectivamente
quasiquotedExpression
    : '`' expression // Example: `(a ,b ,@c)
    ;

// con , evaluamos la siguiente expresión,
// si el resultado de la expresión tendremos listas nested
// esto se puede evitar si usamos ,@ con lo que colocaremos
// los elementos de la lista evaluada como elementos
// de la lista superior, de forma que no estarán nested
unquotedExpression
    : ',' expression // Example: ,x
    | ',@' expression // Example: ,@list
    ;

// atomic symbol
ATOMIC_SYMBOL
    : [a-zA-Z!$%&*+./:<=>?@^_~][-a-zA-Z0-9!$%&*+./:<=>?@^_~]*
    ;

// number: includes integers, decimals, and negative numbers
NUMBER
    : '-'? [0-9]+ ('.' [0-9]+)? // Supports integers and decimals
    | '#e' '-'? [0-9]+ ('.' [0-9]+)? // Exact numbers
    | '#i' '-'? [0-9]+ ('.' [0-9]+)? // Inexact numbers
    ;

// boolean literals
BOOLEAN
    : '#t'
    | '#f'
    ;

// character literals
CHARACTER
    : '#\\' . // Example: #\a, #\newline
    ;

// string literals
STRING
    : '"' (~["\\] | '\\' .)* '"' // Allows escaping inside strings
    ;

// Comments and Whitespace
COMMENT
    : ';' ~[\r\n]* -> skip 
    | '#|' .*? '|#' -> skip 
    ;

WS
    : [ \r\n\t]+ -> skip
    ;)'
