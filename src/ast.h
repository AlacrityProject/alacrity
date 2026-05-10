#ifndef AST_H
#define AST_H

// Enum definitions
typedef enum
{
    TOKEN_IDENTIFIER,
    TOKEN_EOF,

    TOKEN_LEFT_PAREN,
    TOKEN_RIGHT_PAREN,
    TOKEN_SEMICOLON,
    TOKEN_LEFT_CURLY_BRACKET,
    TOKEN_RIGHT_CURLY_BRACKET,

    // Comparison
    TOKEN_EQUALS,
    TOKEN_EQUAL_TO,
    TOKEN_LESS_THAN,
    TOKEN_GREATER_THAN,
    TOKEN_LESS_THAN_EQUAL_TO,
    TOKEN_GREATER_THAN_EQUAL_TO,
    TOKEN_NOT_EQUAL_TO,
    TOKEN_NOT,

    // Math
    TOKEN_ADD,
    TOKEN_INCREMENT,
    TOKEN_SUBTRACT,
    TOKEN_DECREMENT,
    TOKEN_MULTIPLY,
    TOKEN_DOUBLE_ASTERISK,
    TOKEN_DIVIDE,

    // Values
    TOKEN_STRING,
    TOKEN_INTEGER_LITERAL,

    // Types
    TOKEN_INT_TYPE,
    TOKEN_FLOAT_TYPE,
    TOKEN_BOOL_TYPE,

    // Keywords
    TOKEN_PRINT,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_ELIF,
    TOKEN_FOR,
    TOKEN_WHILE,
    TOKEN_FUNC,

    TOKEN_VARIABLE,

    TOKEN_NULL

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
    NODE_LITERAL,
    NODE_UNARY,
    NODE_BINARY,
    NODE_DECLARATION,
    NODE_PRINT,
    NODE_IF_ELSE,
    NODE_INCREMENT_DECREMENT,

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
            int operator;
            struct AstNode *operand;
        } unary;

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
            Token variable;
            int operator;
        } incrementDecrement;

        struct
        {
            struct AstNode *expression;
        } print;

        struct
        {
            struct AstNode *ifBody;
            struct AstNode *expression;
            struct AstNode *elseBody;
        } ifElse;

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
struct AstNode *parseIfElse(Parser *parser);

// Node Creation functions
struct AstNode *makeLiteralNode(Token token);
struct AstNode *makeUnaryNode(Token operator, struct AstNode * operand);
struct AstNode *makeBinaryNode(int operator, struct AstNode * left, struct AstNode *right);
struct AstNode *makeDeclarationNode(Token tokenTypeKeyword, Token tokenName, ASTNode *expression);
struct AstNode *makePrintNode(struct AstNode *expression);
struct AstNode *makeIfElseNode(struct AstNode *expression, struct AstNode *ifBody, struct AstNode *elseBody);
struct AstNode *makeIncrementDecrementNode(int operator, Token variable);

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
