grammar MiniScheme;

program: statement+;

statement
    : expression
    | define
    ;

define
    : 'define' SYMBOL expression
    ;

expression
    : NUMBER                              # NumberExpr
    | SYMBOL                              # SymbolExpr
    | '(' operator expression+ ')'        # OperationExpr
    | define                              # DefineExpr
    ;

NUMBER: [0-9]+;
SYMBOL: [a-zA-Z][a-zA-Z0-9]*;
operator: '+' | '-' | '*' | '/';

WS: [ \t\r\n]+ -> skip;
