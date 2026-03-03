/* 
Group Number: 7
Hamsini G - 2023A7PS0006P
Neha Gujjari - 2023A7PS0011P
Kushi Arun Kumar - 2023A7PS0546P
Kritika Pandey - 2023A7PS0022P
Laavanya Datta - 2023A7PS0536P
*/
#include "parser.h"

#define GRAMMAR_FILE "grammar.txt"
#define _POSIX_C_SOURCE 200809L

#define MAX_NT 128
#define MAX_NAME_LEN 128

static char ntNames[MAX_NT][MAX_NAME_LEN];
static int  ntIds[MAX_NT];
static int  ntCount= 0;
static int  ntInitialized= 0;

//Strip angle brackets
static void stripAngles(const char *src, char *dest, size_t destSize){
    size_t len= strlen(src);
    if (len>= 2 && src[0]== '<' && src[len-1]== '>'){
        size_t inner= len-2;
        if (inner>= destSize) inner= destSize-1;
        strncpy(dest, src+1, inner);
        dest[inner]= '\0';
    } else{
        strncpy(dest, src, destSize-1);
        dest[destSize-1]= '\0';
    }
}

//Get the integer encoding of the non-terminal
static int findNTByName(const char *str){
    for (int i= 0; i < ntCount; i++)
        if (strcmp(ntNames[i], str)== 0)
            return ntIds[i];
    return -1;
}

//Extract non-terminals from the grammar
static void initNTBank(const char *grammarFile){
    if (ntInitialized) return;
    ntInitialized= 1;

    FILE *fp= fopen(grammarFile, "r");
    if (!fp){
        fprintf(stderr, "grammar.c: cannot open '%s' for NT bank\n", grammarFile);
        exit(1);
    }

    char line[1024];
    //Start non-terminal encoding from 59 (Terminals: 0 to 58)
    int  nextId= 59; 

    while (fgets(line, sizeof(line), fp)) {
        line[strcspn(line, "\r\n")]= '\0';

        char *p= line;

        while (*p== ' ' || *p== '\t') p++;
        if (*p== '\0' || *p== '#') continue;

        char *arrow= strstr(p, "===>");
        if (!arrow) continue;

        *arrow= '\0';
        char lhsBuf[MAX_NAME_LEN];
        if (sscanf(p, "%127s", lhsBuf)!= 1) continue;
        if (lhsBuf[0]!= '<') continue;

        char str[MAX_NAME_LEN];
        stripAngles(lhsBuf, str, sizeof(str));

        //If the non-terminal is not present in the bank, add it
        if (findNTByName(str)== -1) {
            if (ntCount >= MAX_NT) {
                fprintf(stderr, "grammar.c: too many nonterminals (max %d)\n", MAX_NT);
                break;
            }
            strncpy(ntNames[ntCount], str, MAX_NAME_LEN-1);
            ntIds[ntCount]= nextId++;
            ntCount++;
        }
    }
    fclose(fp);
}

//Check if the non-terminal is already added
static int lookupNT(const char *name){
    char str[MAX_NAME_LEN];
    stripAngles(name, str, sizeof(str));
    return findNTByName(str);
}

static const char *TERMINAL_NAMES[NUM_TERMINALS]={
    "TK_MAIN", "TK_END", "TK_FUNID", "TK_INPUT", "TK_PARAMETER",
    "TK_LIST", "TK_SQL", "TK_SQR", "TK_OUTPUT", "TK_INT",
    "TK_REAL", "TK_RECORD", "TK_RUID", "TK_UNION", "TK_ID",
    "TK_COMMA", "TK_COLON", "TK_GLOBAL", "TK_ASSIGNOP", "TK_CALL",
    "TK_WITH", "TK_PARAMETERS", "TK_WHILE", "TK_OP", "TK_CL",
    "TK_ENDWHILE", "TK_IF", "TK_THEN", "TK_ELSE", "TK_ENDIF",
    "TK_READ", "TK_WRITE", "TK_SEM", "TK_PLUS", "TK_MINUS",
    "TK_MUL", "TK_DIV", "TK_NOT", "TK_AND", "TK_OR",
    "TK_LT", "TK_LE", "TK_EQ", "TK_GE", "TK_NE",
    "TK_RETURN", "TK_DEFINETYPE", "TK_AS", "TK_ENDRECORD", "TK_ENDUNION",
    "TK_FIELDID", "TK_TYPE", "TK_DOT", "TK_RNUM", "TK_NUM",
    "TK_GT", "TK_EOF", "TK_COMMENT", "TK_ERROR"
};

//Return the integer encoding of the input symbol
static int resolveSymbol(const char *sym){
    if (!sym || sym[0]== '\0') return -1;

    if (strcmp(sym, "epsilon") == 0) return EPSILON;

    //If symbol is a non-terminal
    if (sym[0]== '<') {
        size_t len= strlen(sym);
        if (sym[len-1] != '>') {
            fprintf(stderr, "grammar.c: malformed non-terminal '%s'\n", sym);
            return -1;
        }

        int id= lookupNT(sym);
        if (id== -1)
            fprintf(stderr, "grammar.c: unknown non-terminal '%s'\n", sym);
        return id;
    }

    for (int i = 0; i < NUM_TERMINALS; i++){
        if (strcmp(sym, TERMINAL_NAMES[i]) == 0)
            return i;
    }

    fprintf(stderr, "grammar.c: unknown symbol '%s'\n", sym);
    return -1;
}

//Read grammar.txt file and build the grammar structure
void loadGrammar(Grammar *g){
    g->ruleCount= 0;

    //Build non-terminal bank
    initNTBank(GRAMMAR_FILE);

    //Populate grammar productions
    FILE *fp= fopen(GRAMMAR_FILE, "r");
    if (!fp){
        fprintf(stderr, "grammar.c: cannot open '%s'\n", GRAMMAR_FILE);
        exit(1);
    }

    char line[1024];
    int  lineNo= 0;

    while (fgets(line, sizeof(line), fp)){
        lineNo++;
        line[strcspn(line, "\r\n")] = '\0';

        char *p= line;
        while (*p== ' ' || *p== '\t') p++;
        if (*p== '\0' || *p== '#') continue;

        char *arrow= strstr(p, "===>");
        if (!arrow){
            fprintf(stderr, "grammar.c: line %d: missing '===>' skipping\n", lineNo);
            continue;
        }

        //Store LHS
        *arrow= '\0';
        char lhsBuf[128];
        if (sscanf(p, "%127s", lhsBuf) != 1) continue;

        int lhsSym= resolveSymbol(lhsBuf);
        if (lhsSym< 59 || lhsSym> 111){
            fprintf(stderr, "grammar.c: line %d: bad LHS '%s'\n", lineNo, lhsBuf);
            continue;
        }

        if (g->ruleCount>= MAX_RULES){
            fprintf(stderr, "grammar.c: MAX_RULES (%d) exceeded\n", MAX_RULES);
            break;
        }

        GrammarRule *rule= &g->rules[g->ruleCount];
        rule->lhs= lhsSym;
        rule->rhs_len= 0;

        //Iterate through all RHS symbols and store them
        char *saveptr= NULL;
        char *word= strtok_r(arrow+4, " \t", &saveptr);
        while (word){
            if (rule->rhs_len >= MAX_RHS){
                fprintf(stderr, "grammar.c: Line %d: RHS too long (max %d)\n", lineNo, MAX_RHS);
                break;
            }
            int sym= resolveSymbol(word);
            if (sym== -1){
                fprintf(stderr, "grammar.c: line %d: unresolved symbol '%s'\n", lineNo, word);
            } else{
                rule->rhs[rule->rhs_len++]= sym;
            }
            word= strtok_r(NULL, " \t", &saveptr);
        }

        if (rule->rhs_len== 0) {
            fprintf(stderr, "grammar.c: line %d: empty RHS, skipping the rule\n", lineNo);
            continue;
        }
        g->ruleCount++;
    }

    fclose(fp);
    printf("Grammar loaded: %d rules\n", g->ruleCount);
}

//Get non-terminal name by its integer encoding
const char *getNTNameById(int id){
    for (int i = 0; i < ntCount; i++)
        if (ntIds[i] == id)
            return ntNames[i];
    return NULL;
}

int getNonTerminalCount(){
    return ntCount;
}

int getTerminalCount(){
    return 59;
}