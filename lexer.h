/* 
Group Number: 7
Hamsini G - 2023A7PS0006P
Neha Gujjari - 2023A7PS0011P
Kushi Arun Kumar - 2023A7PS0546P
Kritika Pandey - 2023A7PS0022P
Laavanya Datta - 2023A7PS0536P
*/

#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "lexerDef.h"

void initLexer();

int lexerDone(twinBuffer *B);

void initBuffer(twinBuffer *B, FILE *f);

// Functions related to symbol table
void Insert(char *s, int t);
int Lookup(char *s);
SymbolEntry* LookupEntry(char *s);
void displaySymbolTable();

// Functions related to token array
void initTokenArray(TokenArray *arr);
void printTokens(TokenArray *arr);
void freeTokenArray(TokenArray *arr);
void printTokensToFile(FILE *fp, infoLine *arr, int size);

//Function related to removing comments
void removeComments(char *testcaseFile);

// Function related to error log
void printAndFreeErrors();


// Main lexer function
infoLine getNextToken(twinBuffer *B, TokenArray *tokenStream, int *lno, FILE *f);
void runLexer(char *srcFile);

#endif