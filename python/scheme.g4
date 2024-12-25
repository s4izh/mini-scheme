grammar scheme;

// Top-Level Program
program
    : expression+ EOF
    ;

// Expressions
expression
    : literal
    | quotedExpression
    | quasiquotedExpression
    | '(' specialForm ')'    // Special forms like define, if, cond
    | '(' operator expression+ ')' // Arithmetic and relational operators
    | '(' expression+ ')'    // Generic S-expression
    ;

// Special Forms
specialForm
    : 'define' IDENTIFIER expression         // define <identifier> <value>
    | 'define' '(' IDENTIFIER parameters ')' expression // define (function)
    | 'if' expression expression expression? // if <test> <then> <else?>
    | 'cond' condClause+                      // cond <clauses>
    | 'null?' expression                      // null? <value>
    ;

// Cond Clauses
condClause
    : '(' expression expression+ ')'          // (test result...)
    ;

// Operators
operator
    : '+' | '-' | '*' | '/'                   // Arithmetic
    | '>' | '<' | '>=' | '<=' | '=' | '<>'    // Relational
    ;

// Parameters for Lambda
parameters
    : IDENTIFIER*
    ;

// Quoted Expressions
quotedExpression
    : '\'' expression
    ;

quasiquotedExpression
    : '`' expression
    ;

// Literals
literal
    : NUMBER
    | BOOLEAN
    | STRING
    ;

// Lexer Rules
NUMBER
    : '-'? [0-9]+ ('.' [0-9]+)?
    ;

BOOLEAN
    : '#t' | '#f'
    ;

IDENTIFIER
    : [a-zA-Z!$%&*+./:<=>?@^_~][-a-zA-Z0-9!$%&*+./:<=>?@^_~]*
    ;

STRING
    : '"' (~["\\] | '\\' .)* '"'
    ;

// Skip Whitespace and Comments
WS
    : [ \t\r\n]+ -> skip
    ;

COMMENT
    : ';' ~[\r\n]* -> skip
    ;
