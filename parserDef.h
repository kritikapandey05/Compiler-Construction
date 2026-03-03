/* 
Group Number: 7
Hamsini G - 2023A7PS0006P
Neha Gujjari - 2023A7PS0011P
Kushi Arun Kumar - 2023A7PS0546P
Kritika Pandey - 2023A7PS0022P
Laavanya Datta - 2023A7PS0536P
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef PARSERDEF_H
#define PARSERDEF_H

#define MAX_RULES 100
#define MAX_RHS 15
#define MAX_LEXEME 100
#define MAX_ERRORS 100
#define EPSILON 112

// Terminals 
#define TK_MAIN 0
#define TK_END 1
#define TK_FUNID 2
#define TK_INPUT 3
#define TK_PARAMETER 4
#define TK_LIST 5
#define TK_SQL 6
#define TK_SQR 7
#define TK_OUTPUT 8
#define TK_INT 9
#define TK_REAL 10
#define TK_RECORD 11
#define TK_RUID 12
#define TK_UNION 13
#define TK_ID 14
#define TK_COMMA 15
#define TK_COLON 16
#define TK_GLOBAL 17
#define TK_ASSIGNOP 18
#define TK_CALL 19
#define TK_WITH 20
#define TK_PARAMETERS 21
#define TK_WHILE 22
#define TK_OP 23
#define TK_CL 24
#define TK_ENDWHILE 25
#define TK_IF 26
#define TK_THEN 27
#define TK_ELSE 28
#define TK_ENDIF 29
#define TK_READ 30
#define TK_WRITE 31
#define TK_SEM 32
#define TK_PLUS 33
#define TK_MINUS 34
#define TK_MUL 35
#define TK_DIV 36
#define TK_NOT 37
#define TK_AND 38
#define TK_OR 39
#define TK_LT 40
#define TK_LE 41
#define TK_EQ 42
#define TK_GE 43
#define TK_NE 44
#define TK_RETURN 45
#define TK_DEFINETYPE 46
#define TK_AS 47
#define TK_ENDRECORD 48
#define TK_ENDUNION 49
#define TK_FIELDID 50
#define TK_TYPE 51
#define TK_DOT 52
#define TK_RNUM 53
#define TK_NUM 54
#define TK_GT 55
#define TK_EOF 56
#define TK_COMMENT 57
#define TK_ERROR 58

// Token structure from lexer 
typedef struct {
    int type;
    char lexeme[MAX_LEXEME];
    int line;
    double value;
} Token;

// Parse tree node 
typedef struct ParseTreeNode {
    int symbol;
    int isLeaf;
    char lexeme[MAX_LEXEME];
    double value;
    int line;
    struct ParseTreeNode *parent;
    struct ParseTreeNode **children;
    int numChildren;
} ParseTreeNode;

// Grammar rule 
typedef struct {
    int lhs;
    int rhs[MAX_RHS];
    int rhs_len;
} GrammarRule;

// Error structure 
typedef struct {
    int line;
    char message[200];
} Error;

#endif
