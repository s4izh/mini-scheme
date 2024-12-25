grammar scheme;



// Parser Rules
program
: expression+ EOF
;


expression
: literal
| quotedExpression
| quasiquotedExpression
| unquotedExpression
| '(' expression+ ')'   // S-expression: one or more expressions in parentheses
;


quotedExpression
: '\'' expression        // Handles '(expression)
;

quasiquotedExpression
: '`' expression         // Handles `(expression)
;

unquotedExpression
: ',' expression         // Handles ,expression
| ',@' expression        // Handles ,@expression
;


// Literals
literal
: NUMBER
| BOOLEAN
| IDENTIFIER
| STRING
;


// Lexer Rules
NUMBER
: '-'? [0-9]+ ('.' [0-9]+)? // Matches integers and floating-point numbers
;

BOOLEAN
: '#t'  // True
| '#f'  // False
;

IDENTIFIER
: [a-zA-Z!$%&*+./:<=>?@^_~][-a-zA-Z0-9!$%&*+./:<=>?@^_~]* // Matches valid Scheme identifiers
;

STRING
: '"' (~["\\] | '\\' .)* '"' // Matches double-quoted strings with escapes
;

// Skip Whitespace and Comments
WS
: [ \t\r\n]+ -> skip // Ignore whitespace
;


COMMENT
: ';' ~[\r\n]* -> skip // Ignore single-line comments
;
