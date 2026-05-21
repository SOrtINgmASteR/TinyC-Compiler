CC = gcc
LEX = flex
YACC = bison
CFLAGS = -Wall -Wextra -g -Isrc

PARSER_SRC = src/parser.y
LEXER_SRC = src/lexer.l

all: tinyc

tinyc: parser.tab.o lex.yy.o src/ast.o src/symbol_table.o src/tac.o src/codegen.o src/main.o
	$(CC) $(CFLAGS) -o tinyc parser.tab.o lex.yy.o src/ast.o src/symbol_table.o src/tac.o src/codegen.o src/main.o -lfl

parser.tab.c parser.tab.h: $(PARSER_SRC)
	$(YACC) -d -o parser.tab.c $(PARSER_SRC)

lex.yy.c: $(LEXER_SRC) parser.tab.h
	$(LEX) -o lex.yy.c $(LEXER_SRC)

parser.tab.o: parser.tab.c
	$(CC) $(CFLAGS) -c parser.tab.c

lex.yy.o: lex.yy.c
	$(CC) $(CFLAGS) -c lex.yy.c

src/ast.o: src/ast.c src/ast.h
	$(CC) $(CFLAGS) -c src/ast.c -o src/ast.o

src/symbol_table.o: src/symbol_table.c src/symbol_table.h
	$(CC) $(CFLAGS) -c src/symbol_table.c -o src/symbol_table.o

src/tac.o: src/tac.c src/tac.h
	$(CC) $(CFLAGS) -c src/tac.c -o src/tac.o

src/codegen.o: src/codegen.c src/codegen.h
	$(CC) $(CFLAGS) -c src/codegen.c -o src/codegen.o

src/main.o: src/main.c
	$(CC) $(CFLAGS) -c src/main.c -o src/main.o

clean:
	rm -f *.o lex.yy.c parser.tab.c parser.tab.h parser.output tinyc
	rm -f src/*.o
