# Language reference

## Lexical syntax

## Grammar in EBNF-notation

```
declaration             = IDENTIFIER ':' type_specifier '=' expression ';' ;

statement               = expression_statement
                        | block_statement
                        | while_statement
                        | if_statement
                        | break_statement
                        | continue_statement
                        | return_statement ;

expression_statement    = expression ';' ;
declaration_statement   = declaration ;
block_statement         = '{' statement* '}' ;
while_statement         = 'while' expression 'do' statement ;
if_statement            = 'if' expression 'then' statement ('else' statement)? ;
break_statement         = 'break' ';' ;
continue_statement      = 'continue' ';' ;
return_statement        = 'return' expression ';' ;

expression              = assignment ;
assignment              = variable ':=' assignment
                        | or ;
variable                = IDENTIFIER ;
or                      = and ( 'or' and )* ;
and                     = equality ( 'and' equality )* ;
equality                = relation ( ( '==' | '!=' ) relation )* ;
relation                = term ( ( '<' | '<=' | '>' | '>=' ) term )* ;
term                    = factor ( ( '+' | '-' ) factor )* ;
factor                  = unary ( ( '/' | '*' ) unary )* ;
unary                   = ( 'not' | '-' | '+' ) unary
                        | call ;
call                    = primary '(' arguments? ')' ;
index                   = primary '[' expression ']' ;
arguments               = expression ( ',' expression )* ;
primary                 = '(' expression ')'
                        | '(' parameter_list? ')' '=>' statement ';'
                        | literal ;

parameter_list          = IDENTIFIER ':' type_specifier ( ',' IDENTIFIER ':' type_specifier )* ;
type_specifier          = 'int' | 'bool' ;
literal                 = IDENTIFIER
                        | INTEGER
                        | BOOLEAN ;
```
