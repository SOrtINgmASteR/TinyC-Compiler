# TinyC (Mini Compiler)

TinyC is a simple, beginner-friendly mini compiler built with Flex, Bison, and C. It demonstrates core compiler phases without advanced optimizations.

## Features
- Lexical analysis with Flex
- Parsing with Bison
- Abstract Syntax Tree (AST) generation
- Symbol table creation with simple semantic checks
- Three Address Code (TAC) generation
- Simple pseudo-assembly generation

## Technologies
- Flex (Lex)
- Bison (Yacc)
- C (standard C)

## Folder Structure
```
CSE_430_TinyC/
├── src/
│   ├── lexer.l
│   ├── parser.y
│   ├── symbol_table.c
│   ├── symbol_table.h
│   ├── ast.c
│   ├── ast.h
│   ├── tac.c
│   ├── tac.h
│   ├── codegen.c
│   ├── codegen.h
│   └── main.c
├── include/
├── input/
│   └── sample.tc
├── output/
│   ├── tokens.txt
│   ├── symbol_table.txt
│   ├── tac.txt
│   └── assembly.asm
├── Makefile
├── README.md
└── .gitignore
```

## Build Instructions
```bash
make clean
make
```

## Run
```bash
./tinyc input/sample.tc
```

## Output Files
- output/tokens.txt: token stream from the lexer
- output/symbol_table.txt: symbol table after parsing
- output/tac.txt: three-address code
- output/assembly.asm: pseudo-assembly output

## Sample Input (TinyC)
```c
int main() {
	int x;
	int y;

	x = 5;
	y = x + 10;

	if (y > 10) {
		print("Large");
	} else {
		print("Small");
	}

	while (x < y) {
		x = x + 1;
	}

	return 0;
}
```

## Compiler Phases (Simple Explanation)
1. **Lexical Analysis**: The lexer breaks input into tokens.
2. **Syntax Analysis**: The parser checks grammar and builds an AST.
3. **Semantic Checks**: Symbol table checks declarations and types.
4. **Intermediate Code**: TAC is generated from the AST.
5. **Code Generation**: TAC is converted to pseudo-assembly.
