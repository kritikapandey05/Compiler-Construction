/* 
Group Number: 7
Hamsini G - 2023A7PS0006P
Neha Gujjari - 2023A7PS0011P
Kushi Arun Kumar - 2023A7PS0546P
Kritika Pandey - 2023A7PS0022P
Laavanya Datta - 2023A7PS0536P
*/

#ifndef PARSER_H
#define PARSER_H

#include "parserDef.h"

#define NUM_SYMBOLS       113   /* 0..111 usable; EPSILON=112                */
#define NUM_TERMINALS 59
#define NT_PROGRAM 59 

//Grammar
typedef struct {
    GrammarRule rules[MAX_RULES];
    int         ruleCount;
} Grammar;

//ErrorList
typedef struct {
    Error errors[MAX_ERRORS];
    int   count;
} ErrorList;

//grammar.c
void loadGrammar(Grammar *g);
const char *getNTNameById(int id);
int getNonTerminalCount();
int getTerminalCount();

//parser.c
ParseTreeNode* createNode(int symbol);
void           addChild(ParseTreeNode *parent, ParseTreeNode *child);
char*          getTokenName(int token);
char*          getNonterminalName(int nt);

void           computeFirstAndFollow(Grammar *g, int ***first, int ***follow);
void           createParseTable(Grammar *g, int ***first, int ***follow, int **table);

Token          getNextTokenFromFile(FILE *fp);
ParseTreeNode* parseSourceCode(char *filename, Grammar *g, int *table, ErrorList *errors);
void           printParseTree(ParseTreeNode *root, char *outputFile);
void           freeParseTree(ParseTreeNode *node);

#endif /* PARSER_H */