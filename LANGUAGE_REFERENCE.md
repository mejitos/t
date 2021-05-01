# Language reference

* 1. Lexical syntax
    * 1.1. Whitespace
    * 1.2. Comments
    * 1.3. Identifiers
    * 1.4. Keywords / reserved words
    * 1.5. Integer literals
    * 1.6. Boolean literals
    * 1.7. Punctuation
    * 1.8. Operators
* 2. Grammar and semantics
    * 2.1. Types
    * 2.2. Declarations
    * 2.3. Statements
    * 2.4. Expressions


## 1. Lexical syntax

### 1.1. Whitespace

Whitespace characters consists of space, tab and new line. All whitespace is
ignored.


### 1.2. Comments

Comments start with the character '#' and rest of the line after that will be
ignored. There is no multiline/block comments in the language.


### 1.3. Identifiers

Identifiers start with ASCII alphabet character or the '_' character. After
that they will continue with the starter characters and ASCII digits. 
Identifiers are either keywords / reserved words of the language, type
specifiers or names for variables.


### 1.4. Keywords / reserved words

```
int         bool        true        false
and         or          not         return
while       do          break       continue
if          then        else        
```

### 1.5. Integer literals

Only numbers the language supports, are integers. Integer literal are composed
of one or more digits between 0 and 9. Zeros at the beginning of integer will
be disregarded. The semantic value of the literal is the value of the lexeme
in base 10. Integers in this language are 4 bytes wide so the maximum value of
the integer is 2147483647 and the minimum value is -2147483648. Overflows of
the literals will be checked at compile-time but there is no runtime checks of
possible integer overflows.


### 1.6. Boolean literals

There is two different boolean literals and therefore two different values: 
'true' and 'false'.


### 1.7. Punctuation

```
(   )   {   }   [   ]
:   ;   ,   =>
```

### 1.8. Operators

```
+   -   *   /
:=
==  !=
<   <=  >   >=
```


## 2. Grammar and semantics

### 2.1. Types

```
type_specifier          = 'int' | 'bool' ;
```


### 2.2. Declarations

```
declaration             = IDENTIFIER ':' type_specifier '=' expression ';' ;
```


### 2.3. Statements

```
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
```


### 2.4. Expressions

```
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
literal                 = IDENTIFIER
                        | INTEGER
                        | BOOLEAN ;
```
