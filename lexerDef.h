/* 
Group Number: 7
Hamsini G - 2023A7PS0006P
Neha Gujjari - 2023A7PS0011P
Kushi Arun Kumar - 2023A7PS0546P
Kritika Pandey - 2023A7PS0022P
Laavanya Datta - 2023A7PS0536P
*/

#ifndef LEXER_DEF_H
#define LEXER_DEF_H

// Token ID definitions
#define TK_MAIN      0
#define TK_END       1
#define TK_FUNID     2
#define TK_INPUT     3
#define TK_PARAMETER 4
#define TK_LIST      5
#define TK_SQL       6
#define TK_SQR       7
#define TK_OUTPUT    8
#define TK_INT       9
#define TK_REAL      10
#define TK_RECORD    11
#define TK_RUID      12
#define TK_UNION     13
#define TK_ID        14
#define TK_COMMA     15
#define TK_COLON     16
#define TK_GLOBAL    17
#define TK_ASSIGNOP  18
#define TK_CALL      19
#define TK_WITH      20
#define TK_PARAMETERS 21
#define TK_WHILE     22
#define TK_OP        23
#define TK_CL        24
#define TK_ENDWHILE  25
#define TK_IF        26
#define TK_THEN      27
#define TK_ELSE      28
#define TK_ENDIF     29
#define TK_READ      30
#define TK_WRITE     31
#define TK_SEM       32
#define TK_PLUS      33
#define TK_MINUS     34
#define TK_MUL       35
#define TK_DIV       36
#define TK_NOT       37
#define TK_AND       38
#define TK_OR        39
#define TK_LT        40
#define TK_LE        41
#define TK_EQ        42
#define TK_GE        43
#define TK_NE        44
#define TK_RETURN    45
#define TK_DEFINETYPE 46
#define TK_AS        47
#define TK_ENDRECORD 48
#define TK_ENDUNION  49
#define TK_FIELDID   50
#define TK_TYPE      51
#define TK_DOT       52
#define TK_RNUM      53
#define TK_NUM       54
#define TK_GT       55
#define TK_EOF        56   
#define TK_COMMENT   57
#define TK_ERROR     58

// Constant definitions
#define TABLE_SIZE     250
#define MAX_LEXEME_LEN  50
#define INITIAL_CAPACITY 128
#define BUFFER_SIZE    512

// Token length constants as per language specifications
#define TK_ID_MIN_LEN 2
#define TK_ID_MAX_LEN 20
#define TK_FUNID_MAX_LEN 30

// Structure to store information about each token : 
// token type, lexeme, line number and numerical value (only applicable for numerical tokens)
typedef struct {
    int    type;
    char   lexeme[MAX_LEXEME_LEN];
    int    line;
    double value;   
} infoLine;

// Structure to store each entry of symbol table
typedef struct SymbolEntry {
    char lexeme[MAX_LEXEME_LEN];
    int  token;          
    int  index;
    struct SymbolEntry* next;
} SymbolEntry;

// Structure to store twin buffer along with front and back pointer
typedef struct {
    char buffer[2 * BUFFER_SIZE + 2];
    int  bp;   
    int  fp;   
} twinBuffer;

// Structure to store array of tokens
typedef struct {
    infoLine *data;
    int size;
    int capacity;
} TokenArray;

// Structure to store each lexical error - lexeme, line number, error message
typedef struct {
    char* lexeme;
    int   lineNo;
    char* message;
} LexError;

// Structure to store error log
typedef struct {
    LexError *data;
    int size;
    int capacity;
} ErrorArray;

#endif 
