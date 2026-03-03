/* 
Group Number: 7
Hamsini G - 2023A7PS0006P
Neha Gujjari - 2023A7PS0011P
Kushi Arun Kumar - 2023A7PS0546P
Kritika Pandey - 2023A7PS0022P
Laavanya Datta - 2023A7PS0536P
*/

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "lexer.h"
#include "parser.h"

// Creating symbol table and error log
static SymbolEntry *symbolTable[TABLE_SIZE];
static ErrorArray errorLog;
static int currentIndex = 1;

// Function to initialise error log
static void initErrorArray(ErrorArray *arr) {
    arr->size = 0;
    arr->capacity = INITIAL_CAPACITY;
    arr->data = (LexError*)malloc(sizeof(LexError) * arr->capacity);
    if (!arr->data) { 
        fprintf(stderr, "Memory allocation failed!\n"); 
        exit(1); }
}

// Function to log an error
static void logError(char *lexeme, int lineNo, const char *message) {
    if (errorLog.size >= errorLog.capacity) {
        errorLog.capacity *= 2;
        errorLog.data = (LexError *)realloc(errorLog.data, sizeof(LexError) * errorLog.capacity);
        if (!errorLog.data) { 
            fprintf(stderr, "Reallocation failed!\n"); 
            exit(1); 
        }
    }
    errorLog.data[errorLog.size].lexeme = strdup(lexeme);
    errorLog.data[errorLog.size].lineNo = lineNo;
    errorLog.data[errorLog.size].message = strdup(message);
    errorLog.size++;
}

// Function to free error log
static void freeErrorArray(ErrorArray *arr) {
    for (int i = 0; i < arr->size; i++) {
        free(arr->data[i].lexeme);
        free(arr->data[i].message);
    }
    free(arr->data);
}

// Function to print error log and free it
void printAndFreeErrors() {
    if (errorLog.size > 0) {
        printf("\nLexical Errors:\n");
        printf("%-8s %-40s %s\n", "Line", "Lexeme", "Message");
        printf("%-8s %-40s %s\n", "----", "------", "-------");
        for (int i = 0; i < errorLog.size; i++) {
            printf("%-8d %-40s %s\n", errorLog.data[i].lineNo, errorLog.data[i].lexeme, errorLog.data[i].message);
        }
        printf("\nProgram is lexically incorrect.\n");
    } else {
        printf("\nProgram is lexically correct.\n");
    }
    freeErrorArray(&errorLog);
}

// Djb2 hash function 
unsigned int hash(char *str) {
    unsigned int h = 5381;
    int c;
    while ((c = *str++))
        h = ((h << 5) + h) + c;
    return h % TABLE_SIZE;
}

// Lookup function for symbol table - returns index
int Lookup(char *s) {
    unsigned int h = hash(s);
    SymbolEntry* temp = symbolTable[h];
    while (temp != NULL) {
        if (strcmp(temp->lexeme, s) == 0) return temp->index;
        temp = temp->next;
    }
    return 0;
}

// Lookup function for symbol table - returns symbol table entry
SymbolEntry* LookupEntry(char *s) {
    unsigned int h = hash(s);
    SymbolEntry* temp = symbolTable[h];
    while (temp != NULL) {
        if (strcmp(temp->lexeme, s) == 0) 
            return temp;
        temp = temp->next;
    }
    return NULL;
}

// Function to insert entry into symbol table
void Insert(char *s, int t) {
    if (Lookup(s) == 0) {
        SymbolEntry* newEntry = (SymbolEntry*)malloc(sizeof(SymbolEntry));
        strcpy(newEntry->lexeme, s);
        newEntry->token = t;
        newEntry->index = currentIndex++;
        newEntry->next = NULL;
        unsigned int h = hash(s);
        newEntry->next = symbolTable[h];
        symbolTable[h] = newEntry;
    }
}

// Fuction to display symbol table
void displaySymbolTable() {
    printf("\nSymbol Table:\n");
    printf("%-8s %-30s %-6s\n", "Index", "Lexeme", "Token");
    printf("%-8s %-30s %-6s\n", "-----", "------", "-----");
    for (int i = 0; i < TABLE_SIZE; i++) {
        SymbolEntry *temp = symbolTable[i];
        while (temp != NULL) {
            printf("%-8d %-30s %-6d\n", temp->index, temp->lexeme, temp->token);
            temp = temp->next;
        }
    }
}

//Funtion to initialise lexer - initialise symbol table with keywords
void initLexer() {
    for (int i = 0; i < TABLE_SIZE; i++) symbolTable[i] = NULL;
    initErrorArray(&errorLog);
    Insert("with",       TK_WITH);
    Insert("parameters", TK_PARAMETERS);
    Insert("end",        TK_END);
    Insert("while",      TK_WHILE);
    Insert("union",      TK_UNION);
    Insert("endunion",   TK_ENDUNION);
    Insert("definetype", TK_DEFINETYPE);
    Insert("as",         TK_AS);
    Insert("type",       TK_TYPE);
    Insert("_main",      TK_MAIN);
    Insert("global",     TK_GLOBAL);
    Insert("parameter",  TK_PARAMETER);
    Insert("list",       TK_LIST);
    Insert("input",      TK_INPUT);
    Insert("output",     TK_OUTPUT);
    Insert("int",        TK_INT);
    Insert("endwhile",   TK_ENDWHILE);
    Insert("if",         TK_IF);
    Insert("then",       TK_THEN);
    Insert("endif",      TK_ENDIF);
    Insert("read",       TK_READ);
    Insert("return",     TK_RETURN);
    Insert("call",       TK_CALL);
    Insert("record",     TK_RECORD);
    Insert("endrecord",  TK_ENDRECORD);
    Insert("else",       TK_ELSE);
    Insert("real",       TK_REAL);
    Insert("write",      TK_WRITE);
}

// Function to initialise twin buffer from file stream
void initBuffer(twinBuffer *B, FILE *f) {
    B->bp = 0;
    B->fp = 0;
    B->buffer[BUFFER_SIZE] = (char)EOF;
    B->buffer[(2*BUFFER_SIZE) + 1] = (char)EOF;
    int n = fread(B->buffer, 1, BUFFER_SIZE, f);
    if (n < BUFFER_SIZE) 
        B->buffer[n] = (char)EOF;
}

// Function to get next character from twin buffer based on front pointer
static char getNextChar(twinBuffer *B, FILE *f) {
    char c = B->buffer[B->fp];
    if (c == EOF) {
        if (B->fp == BUFFER_SIZE) {
            int bytesRead = fread(B->buffer + BUFFER_SIZE + 1, 1, BUFFER_SIZE, f);
            if (bytesRead < BUFFER_SIZE) 
                B->buffer[BUFFER_SIZE + 1 + bytesRead] = EOF;
            B->fp++;
            c = B->buffer[B->fp];
        } else if (B->fp == 2 * BUFFER_SIZE + 1) {
            int bytesRead = fread(B->buffer, 1, BUFFER_SIZE, f);
            if (bytesRead < BUFFER_SIZE) 
                B->buffer[bytesRead] = EOF;
            B->fp = 0;
            c = B->buffer[B->fp];
        } else {
            return EOF;
        }
    }
    return c;
}

// Function to check if lexical analysis is complete - EOF reached and buffer exhausted
int lexerDone(twinBuffer *B) {
    char c = B->buffer[B->fp];
    return (c == (char)EOF) && (B->fp != BUFFER_SIZE) && (B->fp != 2 * BUFFER_SIZE + 1);
}

// Function to initialise token array to store information about each lexeme
void initTokenArray(TokenArray *arr) {
    arr->size = 0;
    arr->capacity = INITIAL_CAPACITY;
    arr->data = (infoLine *)malloc(sizeof(infoLine) * arr->capacity);
    if (!arr->data) { 
        fprintf(stderr, "Memory allocation failed!\n"); 
        exit(1); 
    }
}

// Function to resize token array
static void resizeTokenArray(TokenArray *arr) {
    arr->capacity *= 2;
    arr->data = (infoLine *)realloc(arr->data, sizeof(infoLine) * arr->capacity);
    if (!arr->data) { 
        fprintf(stderr, "Reallocation failed!\n"); 
        exit(1); 
    }
}

// Function to print token array on console - used in case 2 of driver
void printTokens(TokenArray *arr) {
    printf("\nToken Stream:\n");
    printf("%-6s %-20s %-40s %-6s %-s\n", "ID", "Type", "Lexeme", "Line", "Value");
    printf("%-6s %-20s %-40s %-6s %-s\n", "--", "----", "------", "----", "-----");
    for (int i = 0; i < arr->size; i++) {
        infoLine *t = &arr->data[i];
        if (t->type == TK_NUM || t->type == TK_RNUM)
            printf("%-6d %-20s %-40s %-6d %g\n",
                t->type, getTokenName(t->type), t->lexeme, t->line, t->value);
        else
            printf("%-6d %-20s %-40s %-6d\n",
                t->type, getTokenName(t->type), t->lexeme, t->line);
    }
}

// Function to print token array to file which is passed to parser - used in case 3 of driver
void printTokensToFile(FILE *fp, infoLine *arr, int size) {
    if (fp == NULL || arr == NULL) {
        return;
    }
    for (int i = 0; i < size; i++) {
        if (arr[i].type==TK_COMMENT || arr[i].type==TK_ERROR) continue;
        fprintf(fp, "%d\t%s\t%d\t%lf\n", arr[i].type, arr[i].lexeme, arr[i].line, arr[i].value);
    }
}

// Function to free token array
void freeTokenArray(TokenArray *arr) {
    free(arr->data);
}

// Function to remove comments and print to console - used in case 1 of driver
void removeComments(char *testcaseFile){
    FILE *fp1 = fopen(testcaseFile, "r");
    if (fp1 == NULL) {
        printf("Error opening file.\n");
        return;
    }
    char ch;
    while ((ch = fgetc(fp1)) != EOF) {
        if (ch == '%') {
            while ((ch = fgetc(fp1)) != '\n' && ch != EOF);
        }
        else {
            putchar(ch);
        }
    }
    fclose(fp1);
    printf("\n");
}

// Function to extract lexeme being processed based on front pointer and back pointer
static char *extractLexeme(twinBuffer *B) {
    int bp = B->bp, fp = B->fp;
    char *lex;
    int   len;
    int bp_in_first = (bp <= BUFFER_SIZE);
    int fp_in_second = (fp > BUFFER_SIZE);
    if (bp_in_first && fp_in_second) {
        int p1 = BUFFER_SIZE - bp;
        int p2 = fp - (BUFFER_SIZE + 1);
        len = p1 + p2;
        lex = (char *)malloc(len + 1);
        if (!lex) { 
            fprintf(stderr, "malloc failed\n"); exit(1); 
        }
        memcpy(lex, B->buffer + bp, p1);
        memcpy(lex + p1, B->buffer + BUFFER_SIZE + 1, p2);
    } else if (fp < bp) {
        int p1 = (2 * BUFFER_SIZE + 1) - bp;
        int p2 = fp;
        len = p1 + p2;
        lex = (char *)malloc(len + 1);
        if (!lex) { 
            fprintf(stderr, "malloc failed\n"); exit(1); 
        }
        memcpy(lex, B->buffer + bp, p1);
        memcpy(lex + p1, B->buffer, p2);
    } else {
        len = fp - bp;
        lex = (char *)malloc(len + 1);
        if (!lex) { fprintf(stderr, "malloc failed\n"); exit(1); }
        memcpy(lex, B->buffer + bp, len);
    }
    lex[len] = '\0';
    return lex;
}

// Function to calculate current length of lexeme being processed based on front pointer and back pointer
static int currentLen(twinBuffer *B) {
    if (B->fp >= B->bp) 
        return B->fp - B->bp;
    return (BUFFER_SIZE - B->bp) + B->fp;
}

// Function to extract token from buffer and add to token array based on front pointer and back pointer
static void addTokenFromBuf(TokenArray *arr, twinBuffer *B, int type, int line) {
    if (arr->size >= arr->capacity) 
        resizeTokenArray(arr);
    char *lex = extractLexeme(B);
    infoLine *t = &arr->data[arr->size++];
    t->type  = type;
    t->line  = line;
    t->value = NAN;
    strncpy(t->lexeme, lex, MAX_LEXEME_LEN - 1);
    t->lexeme[MAX_LEXEME_LEN - 1] = '\0';
    free(lex);
}

// Function to extract and add numerical token from buffer - populate value field of token
static void addNumTokenFromBuf(TokenArray *arr, twinBuffer *B, int type, int line) {
    if (arr->size >= arr->capacity) 
        resizeTokenArray(arr);
    char *lex = extractLexeme(B);
    infoLine *t = &arr->data[arr->size++];
    t->type = type;
    t->line = line;
    t->value = atof(lex);
    strncpy(t->lexeme, lex, MAX_LEXEME_LEN - 1);
    t->lexeme[MAX_LEXEME_LEN - 1] = '\0';
    free(lex);
}

// Function to add token to token array - used in cases where lexeme has already been extracted from buffer
static void addTokenWithLexeme(TokenArray *arr, char *lexeme, int type, int line) {
    if (arr->size >= arr->capacity) 
        resizeTokenArray(arr);
    infoLine *t = &arr->data[arr->size++];
    t->type = type;
    t->line = line;
    t->value = NAN;
    strncpy(t->lexeme, lexeme, MAX_LEXEME_LEN - 1);
    t->lexeme[MAX_LEXEME_LEN - 1] = '\0';
}

// Function to add error token to token array 
static void addErrorToken(TokenArray *tokenStream, char *lexeme, int line, const char *message) {
    logError(lexeme, line, message);
    addTokenWithLexeme(tokenStream, lexeme, TK_ERROR, line);
}

// Macro to report error - add error to error log with specified message
#define REPORT_ERROR_RETRACT(B, lno, msg) do {                      \
    char *_lex = extractLexeme((B));                                 \
    addErrorToken(tokenStream, _lex, (lno), (msg));                  \
    free(_lex);                                                      \
    (B)->bp = (B)->fp;                                               \
    state = 0;                                                       \
    continue;                                                        \
} while(0)

// Main lexical analyser function 
infoLine getNextToken(twinBuffer *B, TokenArray *tokenStream, int *lno, FILE *f) {
    char c;
    int  state = 0;
    while (1) {
        c = getNextChar(B, f);
        // EOF flush - when EOF is encountered, process 
        if (c == (char)EOF) {
            switch (state) {
                case 1:
                    addTokenFromBuf(tokenStream, B, TK_LT, *lno);
                    B->bp = B->fp;
                    break;
                case 7:
                    addTokenFromBuf(tokenStream, B, TK_GT, *lno);
                    B->bp = B->fp;
                    break;
                case 10: case 11: case 12:{
                    char *lex = extractLexeme(B);
                    SymbolEntry *e = LookupEntry(lex);
                    if (e == NULL) {
                        Insert(lex, TK_FIELDID);
                        addTokenWithLexeme(tokenStream, lex, TK_FIELDID, *lno);
                    } else {
                        addTokenWithLexeme(tokenStream, lex, e->token, *lno);
                    }
                    free(lex); B->bp = B->fp;
                    break;
                }
                case 13: case 14: case 15: case 16: {
                    char *lex = extractLexeme(B);
                    int len = strlen(lex);
                    if (len < TK_ID_MIN_LEN) {
                        addErrorToken(tokenStream, lex, *lno,
                            "TK_ID must be at least 2 characters long");
                    } else if (len > TK_ID_MAX_LEN) {
                        addErrorToken(tokenStream, lex, *lno,
                            "TK_ID exceeds maximum length of 20 characters");
                    } else {
                        SymbolEntry *e = LookupEntry(lex);
                        if (e == NULL) {
                            Insert(lex, TK_ID);
                            addTokenWithLexeme(tokenStream, lex, TK_ID, *lno);
                        } else {
                            addTokenWithLexeme(tokenStream, lex, e->token, *lno);
                        }
                    }
                    free(lex); B->bp = B->fp;
                    break;
                }
                case 17: case 25:
                    addNumTokenFromBuf(tokenStream, B, TK_NUM, *lno);
                    B->bp = B->fp; break;
                case 20: case 24: case 26:
                    addNumTokenFromBuf(tokenStream, B, TK_RNUM, *lno);
                    B->bp = B->fp; break;
                case 28: case 29: case 30: {
                    char *lex = extractLexeme(B);
                    SymbolEntry *e = LookupEntry(lex);
                    if (e == NULL) {
                        Insert(lex, TK_FUNID);
                        addTokenWithLexeme(tokenStream, lex, TK_FUNID, *lno);
                    } else {
                        addTokenWithLexeme(tokenStream, lex, e->token, *lno);
                    }
                    free(lex); B->bp = B->fp;
                    break;
                }
                case 32: case 33: {
                    char *lex = extractLexeme(B);
                    addTokenWithLexeme(tokenStream, lex, TK_RUID, *lno);
                    free(lex); B->bp = B->fp;
                    break;
                }
                case 2: case 3:                            
                case 18: case 19: case 21: case 22: case 23: 
                case 27:                                    
                case 31:                                    
                case 47: case 48:                           
                case 50: case 51:                           
                case 54:                                   
                case 56: {                                 
                    char *lex = extractLexeme(B);
                    addErrorToken(tokenStream, lex, *lno, "Unexpected end of file in token");
                    free(lex); B->bp = B->fp;
                    break;
                }
                default: break;
            }
            if (tokenStream->size > 0)
                return tokenStream->data[tokenStream->size - 1];
            infoLine eof_tok = { TK_EOF, "$eof", *lno, 0 };
            return eof_tok;
        }
        // Main DFA implementation using states
        switch (state) {
            case 0:
                B->fp++;  
                if (c == '<') state = 1;
                else if (c == '>') state = 7;
                else if (c == 'a' || (c >= 'e' && c <= 'z')) state = 10;
                else if (c >= 'b' && c <= 'd') state = 12;
                else if (c >= '0' && c <= '9') state = 17;
                else if (c == '_') state = 27;
                else if (c == '#') state = 31;
                else if (c == '%') state = 34;
                else if (c == '[') { 
                    addTokenFromBuf(tokenStream, B, TK_SQL,*lno); 
                    B->bp = B->fp; 
                    return tokenStream->data[tokenStream->size-1]; 
                }
                else if (c == ']') { 
                    addTokenFromBuf(tokenStream, B, TK_SQR, *lno); 
                    B->bp = B->fp; 
                    return tokenStream->data[tokenStream->size-1]; 
                }
                else if (c == ',') { 
                    addTokenFromBuf(tokenStream, B, TK_COMMA, *lno); 
                    B->bp = B->fp; 
                    return tokenStream->data[tokenStream->size-1]; 
                }
                else if (c == ';') { 
                    addTokenFromBuf(tokenStream, B, TK_SEM, *lno); 
                    B->bp = B->fp; 
                    return tokenStream->data[tokenStream->size-1]; 
                }
                else if (c == ':') { 
                    addTokenFromBuf(tokenStream, B, TK_COLON, *lno); 
                    B->bp = B->fp; 
                    return tokenStream->data[tokenStream->size-1]; 
                }
                else if (c == '.') { 
                    addTokenFromBuf(tokenStream, B, TK_DOT, *lno); 
                    B->bp = B->fp; 
                    return tokenStream->data[tokenStream->size-1]; 
                }
                else if (c == '(') { 
                    addTokenFromBuf(tokenStream, B, TK_OP, *lno); 
                    B->bp = B->fp; 
                    return tokenStream->data[tokenStream->size-1]; 
                }
                else if (c == ')') { 
                    addTokenFromBuf(tokenStream, B, TK_CL, *lno); 
                    B->bp = B->fp; 
                    return tokenStream->data[tokenStream->size-1]; 
                }
                else if (c == '+') { 
                    addTokenFromBuf(tokenStream, B, TK_PLUS, *lno); 
                    B->bp = B->fp; 
                    return tokenStream->data[tokenStream->size-1]; 
                }
                else if (c == '-') { 
                    addTokenFromBuf(tokenStream, B, TK_MINUS, *lno); 
                    B->bp = B->fp; 
                    return tokenStream->data[tokenStream->size-1]; 
                }
                else if (c == '*') { 
                    addTokenFromBuf(tokenStream, B, TK_MUL, *lno); 
                    B->bp = B->fp; 
                    return tokenStream->data[tokenStream->size-1]; 
                }
                else if (c == '/') { 
                    addTokenFromBuf(tokenStream, B, TK_DIV, *lno); 
                    B->bp = B->fp; 
                    return tokenStream->data[tokenStream->size-1]; 
                }
                else if (c == '~') { 
                    addTokenFromBuf(tokenStream, B, TK_NOT, *lno); 
                    B->bp = B->fp; 
                    return tokenStream->data[tokenStream->size-1]; 
                }
                else if (c == '&') state = 47;
                else if (c == '@') state = 50;
                else if (c == '=') state = 54;
                else if (c == '!') state = 56;
                else if (c == ' ' || c == '\t') state = 58;
                else if (c == '\n') {
                    B->bp = B->fp;
                    (*lno)++;
                    infoLine nl = { -1, "", *lno, 0 };
                    return nl;
                }
                else {
                    char *lex = extractLexeme(B);
                    addErrorToken(tokenStream, lex, *lno, "Unknown character");
                    free(lex);
                    B->bp = B->fp;
                    return tokenStream->data[tokenStream->size-1];
                }
                break;
            case 1:
                if (c == '-') { 
                    B->fp++; 
                    state = 2; 
                }
                else if (c == '=') { 
                    B->fp++; 
                    addTokenFromBuf(tokenStream, B, TK_LE, *lno); 
                    B->bp = B->fp; 
                    return tokenStream->data[tokenStream->size-1]; 
                }
                else { 
                    addTokenFromBuf(tokenStream, B, TK_LT, *lno); 
                    B->bp = B->fp; 
                    return tokenStream->data[tokenStream->size-1]; 
                }
                break;

            case 2:
                if (c == '-') { 
                    B->fp++; 
                    state = 3; 
                }
                else { 
                    REPORT_ERROR_RETRACT(B, *lno, "Expected '-' to continue assignment operator <---"); 
                }
                break;

            case 3:
                if (c == '-') { 
                    B->fp++; 
                    addTokenFromBuf(tokenStream, B, TK_ASSIGNOP, *lno); 
                    B->bp = B->fp; 
                    return tokenStream->data[tokenStream->size-1]; 
                }
                else { 
                    REPORT_ERROR_RETRACT(B, *lno, "Expected '-' to complete assignment operator <---"); 
                }
                break;
            case 7:
                if (c == '=') { 
                    B->fp++; 
                    addTokenFromBuf(tokenStream, B, TK_GE, *lno); 
                    B->bp = B->fp; 
                    return tokenStream->data[tokenStream->size-1]; 
                }
                else { 
                    addTokenFromBuf(tokenStream, B, TK_GT, *lno); 
                    B->bp = B->fp; 
                    return tokenStream->data[tokenStream->size-1]; 
                }
                break;
            case 10:
                if (c >= 'a' && c <= 'z') { 
                    B->fp++; state = 10; 
                }
                else state = 11;
                break;
            case 11: {
                char *lex = extractLexeme(B);
                SymbolEntry *e = LookupEntry(lex);
                if (e == NULL) {
                    Insert(lex, TK_FIELDID);
                    addTokenWithLexeme(tokenStream, lex, TK_FIELDID, *lno);
                } else {
                    addTokenWithLexeme(tokenStream, lex, e->token, *lno);
                }
                free(lex); B->bp = B->fp;
                return tokenStream->data[tokenStream->size-1];
            }
            case 12:
                if (c >= 'a' && c <= 'z') { 
                    B->fp++; 
                    state = 10; 
                }
                else if (c >= '2' && c <= '7') { 
                    B->fp++; 
                    state = 13; 
                }
                else { 
                    B->fp++; 
                    state = 11; 
                }
                break;
            case 13:
            case 14:
                if (c >= 'b' && c <= 'd')  { 
                    B->fp++; 
                    state = 14; 
                }
                else if (c >= '2' && c <= '7') { 
                    B->fp++; 
                    state = 15; }
                else state = 16;
                break;
            case 15:
                if (c >= '2' && c <= '7') { 
                    B->fp++; 
                    state = 15; 
                }
                else state = 16;
                break;
            case 16: {
                char *lex = extractLexeme(B);
                int len   = strlen(lex);
                if (len < TK_ID_MIN_LEN) {
                    addErrorToken(tokenStream, lex, *lno, "TK_ID must be at least 2 characters long");
                } else if (len > TK_ID_MAX_LEN) {
                    addErrorToken(tokenStream, lex, *lno, "TK_ID exceeds maximum length of 20 characters");
                } else {
                    SymbolEntry *e = LookupEntry(lex);
                    if (e == NULL) {
                        Insert(lex, TK_ID);
                        addTokenWithLexeme(tokenStream, lex, TK_ID, *lno);
                    } else {
                        addTokenWithLexeme(tokenStream, lex, e->token, *lno);
                    }
                }
                free(lex); B->bp = B->fp;
                return tokenStream->data[tokenStream->size-1];
            }
            case 17:
                if (c >= '0' && c <= '9') { 
                    B->fp++; 
                    state = 17; 
                }
                else if (c == '.') { 
                    B->fp++; 
                    state = 18; }
                else state = 25;
                break;
            case 18:
                if (c >= '0' && c <= '9') { 
                    B->fp++; 
                    state = 19; 
                }
                else { 
                    REPORT_ERROR_RETRACT(B, *lno, "Expected digit after decimal point in real number"); 
                }
                break;
            case 19:
                if (c >= '0' && c <= '9') { 
                    B->fp++; 
                    state = 20; 
                }
                else { 
                    REPORT_ERROR_RETRACT(B, *lno, "Expected second digit after decimal point in real number"); 
                }
                break;
            case 20:
                if (c == 'E') { 
                    B->fp++; 
                    state = 21; 
                }
                else state = 26;
                break;
            case 21:
                if (c == '+' || c == '-') { 
                    B->fp++; 
                    state = 22; 
                }
                else if (c >= '0' && c <= '9') { 
                    B->fp++; 
                    state = 23; 
                }
                else { 
                    REPORT_ERROR_RETRACT(B, *lno, "Expected sign or digit after 'E' in real number exponent"); 
                }
                break;
            case 22:
                if (c >= '0' && c <= '9') { 
                    B->fp++; 
                    state = 23; 
                }
                else { 
                    REPORT_ERROR_RETRACT(B, *lno, "Expected digit after sign in real number exponent"); }
                break;
            case 23:
                if (c >= '0' && c <= '9') {
                    B->fp++; 
                    state = 24; 
                }
                else { 
                    REPORT_ERROR_RETRACT(B, *lno, "Expected second digit in real number exponent"); 
                }
                break;
            case 24:   
                addNumTokenFromBuf(tokenStream, B, TK_RNUM, *lno);
                B->bp = B->fp; 
                return tokenStream->data[tokenStream->size-1];

            case 25:   
                addNumTokenFromBuf(tokenStream, B, TK_NUM, *lno);
                B->bp = B->fp; 
                return tokenStream->data[tokenStream->size-1];

            case 26: 
                addNumTokenFromBuf(tokenStream, B, TK_RNUM, *lno);
                B->bp = B->fp; 
                return tokenStream->data[tokenStream->size-1];
            case 27:
                if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
                    B->fp++; 
                    state = 28;
                } else {
                    REPORT_ERROR_RETRACT(B, *lno, "TK_FUNID must have at least one letter after '_'");
                }
                break;
            case 28:
                if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
                    if (currentLen(B) >= TK_FUNID_MAX_LEN) {
                        B->fp++;
                        while (1) {
                            char nc = getNextChar(B, f);
                            if ((nc >= 'a' && nc <= 'z') || (nc >= 'A' && nc <= 'Z') || (nc >= '0' && nc <= '9')) 
                                B->fp++; 
                            else break;
                        }
                        REPORT_ERROR_RETRACT(B, *lno, "TK_FUNID exceeds maximum length of 30 characters");
                    }
                    B->fp++; 
                    state = 28;
                } else if (c >= '0' && c <= '9') {
                    if (currentLen(B) >= TK_FUNID_MAX_LEN) {
                        B->fp++;
                        while (1) {
                            char nc = getNextChar(B, f);
                            if ((nc >= 'a' && nc <= 'z') || (nc >= 'A' && nc <= 'Z') || (nc >= '0' && nc <= '9')) 
                                B->fp++; 
                            else break;
                        }
                        REPORT_ERROR_RETRACT(B, *lno, "TK_FUNID exceeds maximum length of 30 characters");
                    }
                    B->fp++; 
                    state = 29;
                } else {
                    state = 30;
                }
                break;
            case 29:
                if (c >= '0' && c <= '9') {
                    if (currentLen(B) >= TK_FUNID_MAX_LEN) {
                        B->fp++;
                        while (1) {
                            char nc = getNextChar(B, f);
                            if ((nc >= 'a' && nc <= 'z') || (nc >= 'A' && nc <= 'Z') || (nc >= '0' && nc <= '9')) 
                                B->fp++; 
                            else break;
                        }
                        REPORT_ERROR_RETRACT(B, *lno, "TK_FUNID exceeds maximum length of 30 characters");
                    }
                    B->fp++; 
                    state = 29;
                } else {
                    state = 30;
                }
                break;
            case 30: {
                char *lex = extractLexeme(B);
                SymbolEntry *e = LookupEntry(lex);
                if (e == NULL) {
                    Insert(lex, TK_FUNID);
                    addTokenWithLexeme(tokenStream, lex, TK_FUNID, *lno);
                } else {
                    addTokenWithLexeme(tokenStream, lex, e->token, *lno);
                }
                free(lex); 
                B->bp = B->fp;
                return tokenStream->data[tokenStream->size-1];
            }
            case 31:
                if (c >= 'a' && c <= 'z') { B->fp++; state = 32; }
                else { 
                    REPORT_ERROR_RETRACT(B, *lno, "Expected lowercase letter after '#' in record union identifier"); 
                }
                break;

            case 32:
                if (c >= 'a' && c <= 'z') { 
                    B->fp++; 
                    state = 32; 
                }
                else 
                    state = 33;
                break;

            case 33: {
                char *lex = extractLexeme(B);
                addTokenWithLexeme(tokenStream, lex, TK_RUID, *lno);
                free(lex); 
                B->bp = B->fp;
                return tokenStream->data[tokenStream->size-1];
            }

            case 34:
                addTokenFromBuf(tokenStream, B, TK_COMMENT, *lno);
                B->bp = B->fp;   // discard '%'
                while (1) {
                    c = getNextChar(B, f);
                    if (c == (char)EOF) break;   // don't advance fp, don't increment lno
                    B->fp++;
                    if (c == '\n') {
                        B->bp = B->fp;
                        (*lno)++;
                        infoLine nl = { -1, "", *lno, 0 };
                        return nl;
                    }
                }
                B->bp = B->fp;
                state = 0;
                break;
            case 47:
                if (c == '&') { 
                    B->fp++; 
                    state = 48; 
                }
                else { 
                    REPORT_ERROR_RETRACT(B, *lno, "Expected '&&&' for AND operator"); 
                }
                break;

            case 48:
                if (c == '&') { 
                    B->fp++; 
                    addTokenFromBuf(tokenStream, B, TK_AND, *lno); 
                    B->bp = B->fp; 
                    return tokenStream->data[tokenStream->size-1]; 
                }
                else { 
                    REPORT_ERROR_RETRACT(B, *lno, "Expected '&&&' for AND operator"); 
                }
                break;
            case 50:
                if (c == '@') { 
                    B->fp++; 
                    state = 51; 
                }
                else { 
                    REPORT_ERROR_RETRACT(B, *lno, "Expected '@@@' for OR operator"); 
                }
                break;
            case 51:
                if (c == '@') { 
                    B->fp++; 
                    addTokenFromBuf(tokenStream, B, TK_OR, *lno); 
                    B->bp = B->fp; 
                    return tokenStream->data[tokenStream->size-1]; 
                }
                else { 
                    REPORT_ERROR_RETRACT(B, *lno, "Expected '@@@' for OR operator"); 
                }
                break;
            case 54:
                if (c == '=') { 
                    B->fp++; 
                    addTokenFromBuf(tokenStream, B, TK_EQ, *lno); 
                    B->bp = B->fp; 
                    return tokenStream->data[tokenStream->size-1]; 
                }
                else { 
                    REPORT_ERROR_RETRACT(B, *lno, "Expected '==' for equality operator"); 
                }
                break;
            case 56:
                if (c == '=') { 
                    B->fp++; 
                    addTokenFromBuf(tokenStream, B, TK_NE, *lno); 
                    B->bp = B->fp; 
                    return tokenStream->data[tokenStream->size-1]; 
                }
                else { 
                    REPORT_ERROR_RETRACT(B, *lno, "Expected '!=' for not-equal operator"); 
                }
                break;
            case 58:
                if (c == ' ' || c == '\t') { 
                    B->fp++; 
                    state = 58; 
                }
                else { 
                    B->bp = B->fp; 
                    state = 0; 
                }
                break;
        }
    }
}