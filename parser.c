/* 
Group Number: 7
Hamsini G - 2023A7PS0006P
Neha Gujjari - 2023A7PS0011P
Kushi Arun Kumar - 2023A7PS0546P
Kritika Pandey - 2023A7PS0022P
Laavanya Datta - 2023A7PS0536P
*/

#include "parser.h"

//Get token name by its integer encoding
char *getTokenName(int token){
    int numTerminal= getTerminalCount();
    static const char *names[NUM_TERMINALS] = {
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
        "TK_FIELDID", "TK_TYPE", "TK_DOT", "TK_RNUM", "TK_NUM", "TK_GT", "TK_EOF", "TK_COMMENT", "TK_ERROR"
    };
    if (token >= 0 && token < numTerminal) return (char *)names[token];
    return "UNKNOWN";
}

//Get non-terminal name by its integer encoding
char *getNonterminalName(int token){
    const char *name= getNTNameById(token);
    return name ? (char *)name : "UNKNOWN";
}

//Create a new parse tree node
ParseTreeNode *createNode(int symbol){
    ParseTreeNode *n= (ParseTreeNode *)malloc(sizeof(ParseTreeNode));
    n->symbol= symbol;
    n->isLeaf= 0;
    n->numChildren= 0;
    n->children= NULL;
    n->parent= NULL;
    n->line= 0;
    n->value= 0.0;
    n->lexeme[0]= '\0';
    return n;
}

//Add a child node in parse tree
void addChild(ParseTreeNode *parent, ParseTreeNode *child){
    parent->numChildren++;
    parent->children= (ParseTreeNode **)realloc(
        parent->children,
        parent->numChildren*sizeof(ParseTreeNode *));
    parent->children[parent->numChildren-1]= child;
    child->parent= parent;
}

//Compute the FIRST and FOLLOW sets for all grammar symbols
void computeFirstAndFollow(Grammar *g, int ***first, int ***follow){
    int numTerminal= getTerminalCount();

    *first= (int **)malloc(NUM_SYMBOLS*sizeof(int *));
    *follow= (int **)malloc(NUM_SYMBOLS*sizeof(int *));

    for (int i= 0; i< NUM_SYMBOLS; i++) {
        (*first)[i]= (int *)calloc(numTerminal+1, sizeof(int));
        (*follow)[i]= (int *)calloc(numTerminal, sizeof(int));
    }

    int **first_set= *first;
    int **follow_set= *follow;

    //Compute FIRST sets

    //For every terminal t, FIRST(t) = {t}
    for (int t= 0; t< numTerminal; t++)
        first_set[t][t]= 1;

    //For non-terminals
    int changed= 1;
    while (changed) {
        changed= 0;

        for (int r= 0; r< g->ruleCount; r++){
            GrammarRule *rule= &g->rules[r];
            int lhs= rule->lhs;
            int all_nullable= 1;

            for (int i= 0; i< rule->rhs_len; i++){
                int sym= rule->rhs[i];

                //If LHS -> EPSILON, add EPSILON to FIRST(LHS)
                if (sym== EPSILON){
                    if (!first_set[lhs][numTerminal]){
                        first_set[lhs][numTerminal]= 1;
                        changed= 1;
                    }
                    all_nullable= 0;
                    break;
                }

                //If sym is a terminal, add sym to FIRST(LHS)
                if (sym>= 0 && sym< numTerminal){
                    if (!first_set[lhs][sym]){
                        first_set[lhs][sym]= 1;
                        changed= 1; }
                    all_nullable= 0;
                    break;
                }

                //If sym is a non-terminal, add FIRST(sym)-EPSILON to FIRST(LHS)
                for (int t= 0; t < numTerminal; t++){
                    if (first_set[sym][t] && !first_set[lhs][t]){
                        first_set[lhs][t]= 1;
                        changed= 1;
                    }
                }

                //If sym derives EPSILON, continue to next RHS symbol
                if (!first_set[sym][numTerminal]){
                    all_nullable= 0;
                    break;
                }
            }

            //If all RHS symbols derive EPSILON, add EPSILON to FIRST(LHS)
            if (all_nullable && !first_set[lhs][numTerminal]) {
                first_set[lhs][numTerminal]= 1;
                changed= 1;
            }
        }
    }

   //Compute FOLLOW sets

    //Add TK_EOF to FOLLOW(NT_PROGRAM) ($ to FOLLOW(start_symbol))
    follow_set[NT_PROGRAM][TK_EOF] = 1;

    changed= 1;
    while (changed){
        changed= 0;
        for (int r= 0; r < g->ruleCount; r++){
            GrammarRule *rule= &g->rules[r];
            int lhs= rule->lhs;

            for (int pos= 0; pos < rule->rhs_len; pos++){
                int sym= rule->rhs[pos];

                //Compute FOLLOW sets only for non-terminals
                if (sym < 59 || sym > 111) continue;

                int trailer_nullable= 1;

                for (int j= pos+1; j< rule->rhs_len; j++) {
                    int next= rule->rhs[j];

                    if (next== EPSILON) continue;

                    //If next symbol is a terminal, add next to FOLLOW(sym)
                    if (next>= 0 && next< numTerminal) {
                        if (!follow_set[sym][next]){ 
                            follow_set[sym][next]=1; 
                            changed = 1;
                        }
                        trailer_nullable= 0;
                        break;
                    }

                    //If next symbol is a non-terminal, add FIRST(next) to FOLLOW(sym)
                    for (int t = 0; t < numTerminal; t++){
                        if (first_set[next][t] && !follow_set[sym][t]){
                            follow_set[sym][t]= 1;
                            changed= 1;
                        }
                    }

                    //If next symbol does not derive EPSILON, break
                    if (!first_set[next][numTerminal]){
                        trailer_nullable= 0;
                        break;
                    }
                }

                //If everything after sym is nullable, add FOLLOW(LHS) to FOLLOW(sym)
                if (trailer_nullable){
                    for (int t=0; t < numTerminal; t++) {
                        if (follow_set[lhs][t] && !follow_set[sym][t]) {
                            follow_set[sym][t]= 1;
                            changed= 1;
                        }
                    }
                }
            }
        }
    }
}

//Create LL(1) parse table
void createParseTable(Grammar *g, int ***first, int ***follow, int **table) {
    int numTerminal= getTerminalCount();
    int numNonTerminal= getNonTerminalCount();

    int **first_set= *first;
    int **follow_set= *follow;

    int size= numNonTerminal*numTerminal;
    *table = (int *)malloc(size*sizeof(int));

    //Initialise all entries to -1
    for (int i=0; i < size; i++) (*table)[i]= -1;

    int *parse_table= *table;

    for (int r= 0; r <g->ruleCount; r++){
        GrammarRule *rule= &g->rules[r];
        int lhs= rule->lhs;
        int lhs_idx= lhs-59;

        //Compute FIRST(RHS)
        int rhs_first[numTerminal];
        memset(rhs_first, 0, sizeof(rhs_first));
        int rhs_can_eps= 0;

        //If lhs -> EPSILON
        if (rule->rhs_len== 1 && rule->rhs[0]== EPSILON){
            rhs_can_eps = 1;
        } else{
            int all_nullable= 1;
            for (int i=0; i< rule->rhs_len; i++){
                int sym= rule->rhs[i];

                if (sym== EPSILON) continue;

                if (sym>= 0 && sym< numTerminal){
                    rhs_first[sym]= 1;
                    all_nullable= 0;
                    break;
                }

                for (int t= 0; t < numTerminal; t++){
                    if (first_set[sym][t]) rhs_first[t]= 1;
                }

                if (!first_set[sym][numTerminal]){
                    all_nullable= 0;
                    break;
                }
            }
            if (all_nullable) rhs_can_eps = 1;
        }

        //Fill table entries for FIRST(RHS)
        for (int t= 0; t< numTerminal; t++){
            if (rhs_first[t]){
                if (parse_table[lhs_idx*numTerminal+t] != -1)
                    fprintf(stderr,"LL(1) conflict: NT=<%s> terminal=%s rule %d vs %d\n",getNonterminalName(lhs), getTokenName(t),parse_table[lhs_idx*numTerminal+t], r);
                parse_table[lhs_idx*numTerminal+t]= r;
            }
        }

        //If RHS derives EPSILON, also fill for FOLLOW(LHS)
        if (rhs_can_eps){
            for (int t= 0; t< numTerminal; t++){
                if (follow_set[lhs][t]){
                    if (parse_table[lhs_idx*numTerminal+t] != -1)
                        fprintf(stderr,"LL(1) conflict (via FOLLOW): NT=<%s> terminal=%s rule %d vs %d\n",getNonterminalName(lhs), getTokenName(t),parse_table[lhs_idx*numTerminal+t], r);
                    parse_table[lhs_idx*numTerminal+t]= r;
                }
            }
        }
    }

    //Add sync entries where terminal is in FOLLOW(non_terminal)
    for (int i= 0; i< numNonTerminal; i++){
        for (int t= 0; t< numTerminal; t++){
            if (parse_table[i*numTerminal+t]== -1 && follow_set[i+59][t])
                parse_table[i*numTerminal+t]= -2;
        }
    }

    printf("Parse table successfully created.\n\n");
}

//Read the next token from file
Token getNextTokenFromFile(FILE *fp){
    Token t;
    t.type= TK_EOF;
    t.line= 0;
    t.value= 0.0;
    t.lexeme[0]= '\0';

    char line[512];
    if (!fgets(line, sizeof(line), fp)){
        strcpy(t.lexeme, "EOF");
        return t;
    }

    char valueStr[64]= "nan";
    sscanf(line, "%d %99s %d %63s", &t.type, t.lexeme, &t.line, valueStr);
    //If the token is numerical
    if (strcmp(valueStr, "nan") != 0)
        t.value= atof(valueStr);

    return t;
}

//Panic Recovery Mode
static Token panicRecover(ParseTreeNode *nt_node, Token lookahead, int *table, FILE *fp, int *popped){
    int numTerminal= getTerminalCount();
    *popped= 0;
    int lhs_idx= nt_node->symbol-59;
    int entry= table[lhs_idx*numTerminal+lookahead.type];

    //If the entry is sync
    if (entry== -2){
        *popped= 1;
        return lookahead;
    }

    //Skip input tokens until we find a non error entry
    while (entry== -1 && lookahead.type != TK_EOF){
        lookahead= getNextTokenFromFile(fp);
        entry= table[lhs_idx*numTerminal+lookahead.type];
    }

    //If the entry is sync or input is exhausted, pop the stack
    if (entry== -2 || lookahead.type== TK_EOF)
        *popped= 1;

    return lookahead;
}

//Parse the input and return a parse tree
ParseTreeNode *parseSourceCode(char *filename, Grammar *g, int *table, ErrorList *errors){
    int numTerminal= getTerminalCount();
    FILE *fp= fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open token file '%s'\n", filename);
        return NULL;
    }

    ParseTreeNode *stack[10000];
    int  stack_top= -1;
    int  last_line_rep= -1;
    int  error_detected= 0;
    errors->count= 0;

    //Push TK_EOF to the bottom of the stack
    ParseTreeNode *eof_node= createNode(TK_EOF);
    eof_node->isLeaf= 1;
    strcpy(eof_node->lexeme, "EOF");
    stack[++stack_top]= eof_node;

    //Push the start symbol onto stack
    ParseTreeNode *root= createNode(NT_PROGRAM);
    stack[++stack_top]= root;

    Token lookahead= getNextTokenFromFile(fp);

    //Parse input as long as the stack is non empty
    while (stack_top>= 0){
        ParseTreeNode *top = stack[stack_top];

        //Parsing successful
        if (top->symbol== TK_EOF && lookahead.type== TK_EOF)
            break;

        //Terminal on top of stack
        if (top->symbol>= 0 && top->symbol< numTerminal){
            //Match
            if (top->symbol== lookahead.type){
                top->isLeaf= 1;
                top->line= lookahead.line;
                top->value= lookahead.value;
                strncpy(top->lexeme, lookahead.lexeme, MAX_LEXEME-1);
                top->lexeme[MAX_LEXEME-1]= '\0';
                stack_top--;
                lookahead= getNextTokenFromFile(fp);
            } else{
                //Terminal mismatch
                if (lookahead.type != TK_EOF && last_line_rep < lookahead.line){
                    printf("Line %d Error: Got %s ('%s'), expected %s\n",lookahead.line, getTokenName(lookahead.type), lookahead.lexeme,getTokenName(top->symbol));
                    if (errors->count< MAX_ERRORS){
                        errors->errors[errors->count].line = lookahead.line;
                        snprintf(errors->errors[errors->count].message,sizeof(errors->errors[0].message),"Token %s does not match expected %s",getTokenName(lookahead.type),getTokenName(top->symbol));
                        errors->count++;
                    }
                    last_line_rep= lookahead.line;
                }

                stack_top--;   //Pop unmatched expected terminal, keep lookahead
                error_detected= 1;
            }
            continue;
        }

        //Nonterminal on top of stack
        {
            int lhs_idx= top->symbol-59;
            int prod= table[lhs_idx*numTerminal+lookahead.type];

            //If error entry, go to panic error recovery mode
            if (prod== -1) {
                int error_line= lookahead.line;

                if (last_line_rep< error_line){
                    last_line_rep= error_line;
                    printf("Line %d Error: Invalid token %s encountered with value %s stack top %s\n",error_line,getTokenName(lookahead.type), lookahead.lexeme,getNonterminalName(top->symbol));
                    if (errors->count< MAX_ERRORS){
                        errors->errors[errors->count].line= error_line;
                        snprintf(errors->errors[errors->count].message,sizeof(errors->errors[0].message),"Invalid token %s encountered with value %s stack top %s", getTokenName(lookahead.type), lookahead.lexeme,getNonterminalName(top->symbol));
                        errors->count++;
                    }
                }

                error_detected= 1;
                int popped= 0;
                lookahead = panicRecover(top, lookahead, table, fp, &popped);
                if (popped) stack_top--;
                
                continue;
            }

            //If sync entry, pop the non-terminal on top of stack
            if (prod== -2) {
                if (last_line_rep < lookahead.line) {
                    printf("Line %d Error: Invalid token %s encountered with value %s stack top %s\n",lookahead.line,getTokenName(lookahead.type), lookahead.lexeme,getNonterminalName(top->symbol));
                    if (errors->count< MAX_ERRORS){
                        errors->errors[errors->count].line = lookahead.line;
                        snprintf(errors->errors[errors->count].message,sizeof(errors->errors[0].message),"Invalid token %s encountered with value %s stack top %s",getTokenName(lookahead.type), lookahead.lexeme,getNonterminalName(top->symbol));
                        errors->count++;
                    }

                    last_line_rep= lookahead.line;
                }

                stack_top--;
                error_detected= 1;
                continue;
            }

            //If the configuration is valid, expand using the production
            stack_top--;
            GrammarRule *rule= &g->rules[prod];

            //If the production is an epsilon production
            if (rule->rhs_len == 1 && rule->rhs[0] == EPSILON) {
                ParseTreeNode *eps = createNode(EPSILON);
                eps->isLeaf = 1;
                eps->line   = -1;
                strcpy(eps->lexeme, "eps");
                addChild(top, eps);
                continue;
            }

            //Create children left-to-right, push the symbols right-to-left on stack
            ParseTreeNode *children[MAX_RHS];
            for (int i= 0; i< rule->rhs_len; i++){
                children[i]= createNode(rule->rhs[i]);
                addChild(top, children[i]);
            }
            for (int i= rule->rhs_len-1; i>= 0; i--)
                stack[++stack_top]= children[i];
        }
    }

    // Drain any remaining tokens after parsing completes
    while (lookahead.type != TK_EOF) {
        printf("Line %d Error: Stray token %s ('%s') after end of program\n",
            lookahead.line,
            getTokenName(lookahead.type),
            lookahead.lexeme);
        if (errors->count < MAX_ERRORS) {
            errors->errors[errors->count].line = lookahead.line;
            snprintf(errors->errors[errors->count].message,
                sizeof(errors->errors[0].message),
                "Stray token %s after end of program",
                getTokenName(lookahead.type));
            errors->count++;
        }
        error_detected = 1;
        lookahead = getNextTokenFromFile(fp);
    }

    fclose(fp);
    if (error_detected)
        printf("\nProgram is syntactically incorrect.\n");
    else
        printf("\nProgram is syntactically correct.\n");

    return root;
}

//Helper function to print one parse tree node
static void printOneNode(ParseTreeNode *node, FILE *fp){
    int numTerminal= getTerminalCount();
    int numNonTerminal= getNonTerminalCount();
    char line_buf[16], value_buf[32], parent_buf[32], nodesym_buf[32];

    const char *lexeme= (node->isLeaf && node->symbol != EPSILON) ? node->lexeme : "----";

    if (node->isLeaf && node->line > 0)
        snprintf(line_buf, sizeof(line_buf), "%d", node->line);
    else
        snprintf(line_buf, sizeof(line_buf), "----");

    const char *token_name = (node->isLeaf && node->symbol>= 0 && node->symbol < numTerminal)? getTokenName(node->symbol) : "----";

    if (node->isLeaf && (node->symbol== TK_NUM || node->symbol== TK_RNUM))
        snprintf(value_buf, sizeof(value_buf), "%g", node->value);
    else
        snprintf(value_buf, sizeof(value_buf), "----");

    if (node->parent== NULL)
        snprintf(parent_buf, sizeof(parent_buf), "ROOT");
    else if (node->parent->symbol>= 59 && node->parent->symbol<59+numNonTerminal)
        snprintf(parent_buf, sizeof(parent_buf), "%s",getNonterminalName(node->parent->symbol));
    else
        snprintf(parent_buf, sizeof(parent_buf), "%s",getTokenName(node->parent->symbol));

    const char *is_leaf= node->isLeaf ? "yes" : "no";

    if (!node->isLeaf && node->symbol>= 59 && node->symbol<59+numNonTerminal)
        snprintf(nodesym_buf, sizeof(nodesym_buf), "%s",getNonterminalName(node->symbol));
    else
        snprintf(nodesym_buf, sizeof(nodesym_buf), "----");

    fprintf(fp, "%-20s %-6s %-25s %-10s %-30s %-6s %s\n",lexeme, line_buf, token_name, value_buf,parent_buf, is_leaf, nodesym_buf);
}

//Iterative helper function to print the parse tree
static void printParseTreeHelper(ParseTreeNode *root_node, FILE *fp){
    if (!root_node) return;

    typedef struct { ParseTreeNode *node; int next_child; } Frame;
    Frame *stk = (Frame *)malloc(200000 * sizeof(Frame));
    int top= -1;

    stk[++top] = (Frame){ root_node, 0 };

    while (top>= 0) {
        Frame *f= &stk[top];

        if (f->next_child== 0)
            printOneNode(f->node, fp);

        if (f->next_child< f->node->numChildren){
            ParseTreeNode *child= f->node->children[f->next_child++];
            if (child)
                stk[++top]= (Frame){ child, 0 };
        } else{
            top--;
        }
    }

    free(stk);
}

//Print the inorder traversal of the parse tree to output file
void printParseTree(ParseTreeNode *root, char *outputFile) {
    FILE *fp= fopen(outputFile, "w");
    if (!fp){
        fprintf(stderr, "Error: Cannot open output file '%s'\n", outputFile);
        return;
    }
    printParseTreeHelper(root, fp);
    fclose(fp);
    printf("Parse tree written to '%s'\n", outputFile);
}

//Deallocate parse tree memory
void freeParseTree(ParseTreeNode *node) {
    if (!node) return;

    ParseTreeNode **s1= (ParseTreeNode **)malloc(100000 * sizeof(ParseTreeNode *));
    ParseTreeNode **postord= (ParseTreeNode **)malloc(100000 * sizeof(ParseTreeNode *));
    int top1= -1, top2= -1;

    s1[++top1]= node;
    while (top1>= 0) {
        ParseTreeNode *curr= s1[top1--];
        postord[++top2]= curr;
        for (int i= 0; i< curr->numChildren; i++)
            if (curr->children[i]) s1[++top1]= curr->children[i];
    }
    while (top2>= 0) {
        ParseTreeNode *curr= postord[top2--];
        free(curr->children);
        free(curr);
    }

    free(s1);
    free(postord);
}