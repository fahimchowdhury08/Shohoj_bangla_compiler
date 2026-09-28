# Formal Grammar

Language name suggestion: **সহজ-বাংলা** ("Sohoj-Bangla" — "Simple Bangla")

## Lexical grammar (tokens)

```
DATATYPE_INT    ::= "পূর্ণ"
DATATYPE_FLOAT  ::= "ভগ্নাংশ"
KEYWORD_IF      ::= "যদি"
KEYWORD_ELSE    ::= "নয়তো"
KEYWORD_WHILE   ::= "যতক্ষণ_ধরে"

IDENTIFIER      ::= BANGLA_LETTER (BANGLA_LETTER | BANGLA_DIGIT | "_")*
NUMBER          ::= BANGLA_DIGIT+ ("." BANGLA_DIGIT+)?

BANGLA_LETTER   ::= any Unicode codepoint in the Bengali block U+0980-U+09FF
                     that is not a digit, | ASCII letter | "_"
BANGLA_DIGIT    ::= "০" | "১" | "২" | "৩" | "৪" | "৫" | "৬" | "৭" | "৮" | "৯"

ASSIGN          ::= "="
EQUAL_EQUAL     ::= "=="
PLUS ::= "+"     MINUS ::= "-"
MULTIPLY ::= "*" DIVIDE ::= "/"
GREATER ::= ">"  LESS ::= "<"
TERMINATOR      ::= "।"      (* Bengali danda -- ends every statement *)
LBRACE ::= "{"   RBRACE ::= "}"
```

Whitespace (space, tab, carriage return, newline) separates tokens and is
otherwise discarded; newlines increment the line counter used in error
messages. Any character that matches none of the rules above is reported
as an unexpected character and skipped (the lexer never stops the whole
compilation over one bad character).

## Syntax grammar (productions)

```
program       ::= statement* EOF

statement     ::= declaration
                | assignment
                | ifStatement
                | whileStatement

declaration   ::= (DATATYPE_INT | DATATYPE_FLOAT) IDENTIFIER "=" expression TERMINATOR

assignment    ::= IDENTIFIER "=" expression TERMINATOR

ifStatement   ::= KEYWORD_IF comparison "{" statement* "}"
                   (KEYWORD_ELSE "{" statement* "}")?

whileStatement::= KEYWORD_WHILE comparison "{" statement* "}"

comparison    ::= expression ((">" | "<" | "==") expression)*

expression    ::= term (("+" | "-") term)*

term          ::= factor (("*" | "/") factor)*

factor        ::= NUMBER
                | IDENTIFIER
```

Operator precedence (highest to lowest), all left-associative:
1. `*` `/`
2. `+` `-`
3. `>` `<` `==`

## Static semantics

- A variable must be declared (`declaration`) before it is used in any
  `assignment` or `expression`. Using an undeclared identifier is a
  semantic error.
- A variable may not be declared twice in the same scope.
- Assigning a literal decimal value (containing `.`) to a `পূর্ণ`
  (integer-typed) variable is a type-mismatch error.
- Two data types are supported: `পূর্ণ` (integer) and `ভগ্নাংশ`
  (decimal/float).

## Error recovery

On a syntax or semantic error, the compiler reports the offending line
and message, then discards tokens up to and including the next
`TERMINATOR` (`।`) or `RBRACE` (`}`), and resumes parsing from there. This
lets a single mistake be reported without aborting compilation of the
rest of the file.

## Target code

Each `Assignment` node lowers to a Python assignment followed by a
`print(...)` of the new value; `IfStatement`/`WhileLoop`/`Block` lower to
Python's own `if`/`else`/`while` with 4-space indentation per nesting
level. Bengali identifiers are valid Python 3 identifiers (Python 3 allows
Unicode identifiers per PEP 3131), so variable names are carried through
unchanged.
