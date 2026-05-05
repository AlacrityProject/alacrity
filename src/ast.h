#ifndef AST_H
#define AST_H

// Enum definitions
typedef enum
{
    TOKEN_IDENTIFIER,      // 0
    TOKEN_LEFT_PAREN,      // 1
    TOKEN_RIGHT_PAREN,     // 2
    TOKEN_EQUALS,          // 3
    TOKEN_ADD,             // 4
    TOKEN_SUBTRACT,        // 5
    TOKEN_MULTIPLY,        // 6
    TOKEN_DIVIDE,          // 7
    TOKEN_STRING,          // 8
    TOKEN_INTEGER_LITERAL, // 9
    TOKEN_PRINT,           // 10
    TOKEN_IF,              // 11
    TOKEN_ELSE,            // 12
    TOKEN_ELIF,            // 13
    TOKEN_FOR,             // 14
    TOKEN_WHILE,           // 15
    TOKEN_FUNC,            // 16
    TOKEN_INT_TYPE,        // 17
    TOKEN_FLOAT_TYPE,      // 18
    TOKEN_BOOL_TYPE,       // 19
    TOKEN_EQUAL_EQUAL,     // 20
    TOKEN_VARIABLE,        // 21
    TOKEN_INCREMENT,       // 22
    TOKEN_SEMICOLON,       // 23
    TOKEN_DOUBLE_ASTERISK, // 24
    TOKEN_DECREMENT,       // 25
    TOKEN_EOF              // 26

} TokenType;

typedef enum
{
    NONE = 0,
    ASSIGNMENT = 1, // (=)
    COMPARISON = 2, // (==, <, >)
    SUM = 3,        // (-, +)
    PRODUCT = 4,    // (*, /)
    UNARY = 5,      // Prefixes (-, !)
    CALL = 6        // (., ())
} Precedence;

typedef enum
{
    NODE_LITERAL, // A number: 5
                  // NODE_UNARY,   // A prefix: -5
    NODE_BINARY,  // An operation: 5+5
    NODE_DECLARATION,
    NODE_PRINT,

} ASTNodeType;

// Struct Definitions
typedef struct
{
    char *start;
    int length;
} TokenValue;

typedef struct
{
    TokenType type;
    TokenValue value;
} Token;

typedef struct
{
    char *key;
    TokenType type;
} Keyword;

typedef struct AstNode
{
    ASTNodeType type;
    union
    {
        struct
        {
            struct AstNode *left;
            struct AstNode *right;
            int operator;
        } binary;

        struct
        {
            Token typeKeyword;
            Token name;
            struct AstNode *expression;
        } declaration;

        struct
        {
            struct AstNode *expression;
        } print;

        Token token;
    } data;
} ASTNode;

typedef struct
{
    Token *tokens;
    int currentToken;
    int tokenCount;

} Parser;

typedef struct
{
    char name[100];
    int value;
} Entry;

typedef struct
{
    Entry entries[1000];
    int totalCount;
} SymbolTable;

// Lexer functions
char *readFile(char filename[]);
int tokenize(char source[], Token tokens[]);
int determine_identifier(char *token, int length, Keyword keywords[], int numberOfKeywords);

// Parsing functions
Token peek(Parser *parser);
Token advance(Parser *parser);
Token expect(Parser *parser, TokenType type);

// Specific Parse functions
struct AstNode *parseStatement(Parser *parser);
struct AstNode *parseExpression(Parser *parser, int minimumBindingPower);
struct AstNode *parseDeclaration(Parser *parser);
struct AstNode *parsePrint(Parser *parser);
struct AstNode *parseIf(Parser *parser);

// Node Creation functions
struct AstNode *makeLiteralNode(Token token);
struct AstNode *makeBinaryNode(int operator, struct AstNode * left, struct AstNode *right);
struct AstNode *makeDeclarationNode(Token tokenTypeKeyword, Token tokenName, ASTNode *expression);
struct AstNode *makePrintNode(struct AstNode *expression);

// Parsing Helper functions
void printAST(ASTNode *node, int indent);
char *getTokenType(TokenType type);
int getTokenPrecedence(TokenType type);
int isOperator(TokenType type);
void freeAST(ASTNode *node);

// Evaluator functions
Entry *findEntry(SymbolTable *table, char variableName[]);
void storeEntry(SymbolTable *table, char variableName[], int valueToStore);
int evaluator(ASTNode *ast, SymbolTable *table);

#endif
