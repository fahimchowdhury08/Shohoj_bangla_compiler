# সহজ-বাংলা Compiler (C++ → Python)

A complete toy compiler for an invented Bangla programming language,
targeting executable Python 3.

## Build & run
```bash
g++ -std=c++17 -Wall -o bangla_compiler main.cpp Lexer.cpp Parser.cpp CodeGenerator.cpp
./bangla_compiler input.bng output.py
python3 output.py      # run the generated program
```
(`input.bng` and `output.py` are optional args; they default to those
names if omitted.)

## Pipeline
```
source.bng --[Lexer]--> tokens --[Parser + SemanticAnalyzer]--> AST --[CodeGenerator]--> output.py
```
This mirrors a standard compiler front-end + back-end split:
- **Lexer** — hand-written scanner, UTF-8 aware (decodes to Unicode
  codepoints internally since Bengali characters are multi-byte).
- **Parser** — recursive-descent (`statement → declaration/assignment/if/while
  → comparison → expression → term → factor`), builds an AST.
- **Symbol Table / Semantic Analyzer** — plugged into the parser at
  declaration/assignment/identifier-use points; checks undeclared
  variables, redeclaration, and int/decimal type mismatch.
- **Code Generator** — walks the AST and emits real, runnable Python.

See `GRAMMAR.md` for the full formal grammar (EBNF) and static-semantics
rules.

## Language cheat sheet

| Concept        | Keyword / symbol | Meaning              |
|----------------|-------------------|-----------------------|
| integer type   | পূর্ণ            | "whole" (number)      |
| decimal type   | ভগ্নাংশ          | "fraction"             |
| if / else      | যদি / নয়তো      | if / "otherwise"      |
| while          | যতক্ষণ_ধরে       | "for as long as"      |
| statement end  | । (danda)        | native Bangla full stop, not `;` |
| numbers        | ০-৯               | Bengali digits, converted to ASCII internally |
| operators      | `+ - * / > < == = { }` | standard math/logic notation |

## What's implemented (maps to the project's minimum requirements)

- [x] Two data types with type checking (পূর্ণ / ভগ্নাংশ, checked in
      `SemanticAnalyzer::checkTypeMismatch`)
- [x] Arithmetic with correct operator precedence (`* /` before `+ -`,
      via the `expression`/`term`/`factor` grammar layers)
- [x] Assignment statements
- [x] If-Else conditional
- [x] While loop
- [x] Basic syntax error recovery (`Parser::synchronize` skips to the
      next `।` or `}`)
- [x] No runtime crashes — verified against empty files, invalid UTF-8,
      undeclared variables, redeclaration, type mismatches, and empty
      if/while blocks (see "Tested edge cases" below)
- [x] Generates a valid, executable Python file (bonus/optional target
      per the spec)

## Tested edge cases
- Empty source file → compiles to just the header comment, no crash.
- Invalid/malformed UTF-8 bytes → caught and reported, exits cleanly
  (does not throw past `main`).
- Undeclared variable use → reported with line number, recovers,
  continues compiling the rest of the file.
- Redeclaring the same variable → reported as an error.
- Assigning a decimal literal to a `পূর্ণ` variable → reported as a type
  mismatch.
- Empty `{}` block on an if/while → generator emits Python's `pass` so
  the output file stays syntactically valid.
- Missing closing `}` at end of file → reported, does not crash or loop.

## Files
- `TokenType.h` / `Token.h` — token definitions
- `Utf8Utils.h` — UTF-8 ⇄ UTF-32 codepoint conversion
- `Lexer.h` / `Lexer.cpp` — lexer
- `ASTNode.h` — AST node + tree printer
- `SymbolTable.h` — declared-variable → type map
- `SemanticAnalyzer.h` — undeclared-use, redeclaration, type-mismatch checks
- `Parser.h` / `Parser.cpp` — recursive-descent parser, wired to the
  symbol table / semantic analyzer, with error recovery
- `CodeGenerator.h` / `CodeGenerator.cpp` — AST → Python emitter
- `main.cpp` — full driver: reads a `.bng` file, prints tokens, prints
  the AST, writes the generated `.py` file
- `input.bng` — sample program (both types, arithmetic precedence,
  if/else, while)
- `GRAMMAR.md` — formal grammar (EBNF) and static semantics

## Known limitation (worth mentioning to your professor)
Type-mismatch checking is currently static/literal-based: it catches
`পূর্ণ ক = ৫.৫।` directly, but can't (yet) catch a decimal value that
only becomes non-integer after arithmetic (e.g. from a future division
feature) without full expression-level type inference. That's a natural
"Future Roadmap" item for the presentation.
