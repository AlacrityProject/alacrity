#ifndef AST_H
#define AST_H

#include <stdbool.h>

extern char *source;

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
    TOKEN_ARROW,
    TOKEN_COMMA,

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
    TOKEN_STRING_LITERAL,
    TOKEN_INTEGER_LITERAL,
    TOKEN_FLOAT_LITERAL,
    TOKEN_BOOL_LITERAL_TRUE,
    TOKEN_BOOL_LITERAL_FALSE,

    // Types
    TOKEN_INT_TYPE,
    TOKEN_FLOAT_TYPE,
    TOKEN_BOOL_TYPE,
    TOKEN_STRING_TYPE,

    // Keywords
    TOKEN_PRINT,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_ELIF,
    TOKEN_FOR,
    TOKEN_WHILE,
    TOKEN_FUNC,
    TOKEN_RETURN,

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
    NODE_ASSIGNMENT,

    NODE_PRINT,

    NODE_IF_ELSE,

    NODE_WHILE,

    NODE_INCREMENT_DECREMENT,

    NODE_FUNCTION,
    NODE_FUNCTION_CALL,
    NODE_RETURN,
    NODE_BLOCK,

} ASTNodeType;

typedef enum
{
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_BOOL,
    TYPE_STRING,
    TYPE_NULL
} ValueType;

// Struct Definitions
typedef struct
{
    ValueType type;
    union
    {
        int int_value;
        double float_value;
        bool bool_value;
        char *string_value;
    } as;

} Value;

typedef struct
{
    char *start;
    int length;
} TokenValue;

typedef struct
{
    TokenType type;
    TokenValue value;
    int line;
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
            Token operator;
            struct AstNode *operand;
        } unary;

        struct
        {
            struct AstNode *left;
            struct AstNode *right;
            Token operator;
        } binary;

        struct
        {
            Token typeKeyword;
            Token name;
            struct AstNode *expression;
        } declaration;

        struct
        {
            Token name;
            struct AstNode *expression;
        } assignment;

        struct
        {
            Token variable;
            Token operator;
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

        struct
        {
            struct AstNode *expression;
            struct AstNode *body;
        } whileNode;

        struct
        {
            struct AstNode **statements;
            int totalCount;
        } nodeBlock;

        struct
        {
            Token name;
            Token *parameters;
            Token *parameterTypes;
            int parameterCount;
            Token returnTypeKeyword;
            struct AstNode *body;

        } function;

        struct
        {
            Token name;
            struct AstNode **arguments;
            int argumentCount;
        } functionCall;

        struct
        {
            struct AstNode *expression;
        } returnNode;

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
    Value value;
} Entry;

typedef struct
{
    Entry entries[1000];
    int totalCount;
} SymbolTable;

typedef struct
{
    char name[100];
    struct AstNode *node;
} FunctionEntry;

typedef struct
{
    FunctionEntry entries[1000];
    int totalCount;
} FunctionTable;

typedef struct
{
    Value value;
    bool returned;
} ReturnResult;

// Lexer functions
char *readFile(char filename[]);
void printErrorLine(Token token);
int tokenize(char source[], Token tokens[]);
int determine_identifier(char *token, int length, Keyword keywords[], int numberOfKeywords);

// Parsing functions
Token peek(Parser *parser);
Token peekNext(Parser *parser);
Token advance(Parser *parser);
Token expect(Parser *parser, TokenType type);

// Specific Parse functions
struct AstNode *parseStatement(Parser *parser);
struct AstNode *parseExpression(Parser *parser, int minimumBindingPower);
struct AstNode *parseDeclaration(Parser *parser);
struct AstNode *parseAssignment(Parser *parser);
struct AstNode *parsePrint(Parser *parser);
struct AstNode *parseIfElse(Parser *parser);
struct AstNode *parseWhile(Parser *parser);
struct AstNode *parseFunctionDeclaration(Parser *parser);
struct AstNode *parseFunctionCall(Parser *parser);
struct AstNode *parseBlock(Parser *parser);
struct AstNode *parseReturn(Parser *parser);

// Node Creation functions
struct AstNode *makeLiteralNode(Token token);
struct AstNode *makeUnaryNode(Token operator, struct AstNode * operand);
struct AstNode *makeBinaryNode(Token operator, struct AstNode * left, struct AstNode *right);
struct AstNode *makeDeclarationNode(Token tokenTypeKeyword, Token tokenName, struct AstNode *expression);
struct AstNode *makeAssignmentNode(Token tokenName, struct AstNode *expression);
struct AstNode *makePrintNode(struct AstNode *expression);
struct AstNode *makeIfElseNode(struct AstNode *expression, struct AstNode *ifBody, struct AstNode *elseBody);
struct AstNode *makeWhileNode(struct AstNode *expression, struct AstNode *body);
struct AstNode *makeIncrementDecrementNode(Token operator, Token variable);
struct AstNode *makeBlockNode(struct AstNode **statements, int totalCount);
struct AstNode *makeFunctionNode(Token name, Token *parameters, Token *parameterTypes, int parameterCount, Token returnType, struct AstNode *body);
struct AstNode *makeFunctionCallNode(Token name, struct AstNode **arguments, int argumentCount);
struct AstNode *makeReturnNode(struct AstNode *expression);

// Parsing Helper functions
void printAST(ASTNode *node, int indent);
char *getTokenType(TokenType type);
int getTokenPrecedence(TokenType type);
bool isOperator(TokenType type);
bool isLiteral(TokenType type);
void freeAST(ASTNode *node);

// Evaluator functions
Entry *findEntry(SymbolTable *table, char variableName[]);
void storeEntry(SymbolTable *table, char variableName[], Value valueToStore);

FunctionEntry *findFunctionEntry(FunctionTable *table, char functionName[]);
void storeFunctionEntry(FunctionTable *table, char functionName[], struct AstNode *nodeToStore);

Value evaluator(ASTNode *ast, SymbolTable *table, FunctionTable *functionTable, ReturnResult *result);

Value makeIntValue(int number);
Value makeBoolValue(bool boolean);
Value makeFloatValue(float number);
Value makeStringValue(char *string);
Value makeNullValue();

bool isTruthy(Value value);
void freeValue(Value value);

Value performFloatBinaryOp(float leftFloat, float rightFloat, Token operator);
Value performBinaryOp(Value left, Value right, Token operator);

#endif
