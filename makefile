CC     = gcc
CFLAGS = -Wall -Wextra -g -std=c11
LIBS   = -lm

all: stage1exe

stage1exe: driver.o lexer.o parser.o grammar.o
	$(CC) $(CFLAGS) -o stage1exe driver.o lexer.o parser.o grammar.o $(LIBS)

driver.o: driver.c lexer.h lexerDef.h parser.h parserDef.h
	$(CC) $(CFLAGS) -c driver.c

lexer.o: lexer.c lexer.h lexerDef.h
	$(CC) $(CFLAGS) -c lexer.c

parser.o: parser.c parser.h parserDef.h lexerDef.h
	$(CC) $(CFLAGS) -c parser.c

grammar.o: grammar.c parser.h parserDef.h
	$(CC) $(CFLAGS) -c grammar.c

clean:
	rm -f *.o stage1exe dest.txt cleanFile.txt

run: stage1exe
	./stage1exe testcase5.txt parsetreeOutFile.txt

.PHONY: all clean run
