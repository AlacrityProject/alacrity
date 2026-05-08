#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ast.h"

// MAIN PARSING FUNCTIONS
Token peek(Parser *parser) // Returns the current token without moving
{
    return parser->tokens[parser->currentToken];
}

Token advance(Parser *parser) // Returns the current token and moves the index forward by one
{
    Token current = parser->tokens[parser->currentToken];

    parser->currentToken++;

    return current;
}

Token expect(Parser *parser, TokenType type) // Calls advance but errors out if the token isn't the correct type
{
    Token token = advance(parser);

    if (token.type != type)
    {
        fprintf(stderr, "ERROR!\n Expected type: %s Got: %s\n", getTokenType(type), getTokenType(token.type));
        exit(1);
    }

    return token;
}

// PARSE
// Add:
// parseStatement - peeks at at current token and decides which parse to call
// parsePrint
// parseIf

struct AstNode *parseStatement(Parser *parser)
{
    Token currentToken = peek(parser);

    if (currentToken.type == TOKEN_INT_TYPE || currentToken.type == TOKEN_FLOAT_TYPE || currentToken.type == TOKEN_BOOL_TYPE)
    {
        return parseDeclaration(parser);
    }
    else if (currentToken.type == TOKEN_PRINT)
    {
        return parsePrint(parser);
    }
    else if (currentToken.type == TOKEN_IF)
    {
        return parseIfElse(parser);
    }
    else
    {
        fprintf(stderr, "ERROR!\n Unknown token in parseStatement at index %d: %s\n",
                parser->currentToken, getTokenType(peek(parser).type));
        exit(1);
    }
}

struct AstNode *parseDeclaration(Parser *parser)
{
    Token tokenTypeKeyword = advance(parser);

    Token tokenName = expect(parser, TOKEN_VARIABLE);
    expect(parser, TOKEN_EQUALS);
    struct AstNode *expression = parseExpression(parser, 0);
    expect(parser, TOKEN_SEMICOLON);

    struct AstNode *node = makeDeclarationNode(tokenTypeKeyword, tokenName, expression);

    return node;
}

struct AstNode *parseExpression(Parser *parser, int minimumBindingPower)
{
    Token currentToken = peek(parser);

    if (currentToken.type == TOKEN_INTEGER_LITERAL || currentToken.type == TOKEN_VARIABLE)
    {
        struct AstNode *left = makeLiteralNode(advance(parser));

        currentToken = peek(parser);
        while (isOperator(currentToken.type) && getTokenPrecedence(currentToken.type) > minimumBindingPower)
        {
            Token operatorToken = advance(parser);
            int operatorPrecendence = getTokenPrecedence(operatorToken.type);
            struct AstNode *right = parseExpression(parser, operatorPrecendence);
            left = makeBinaryNode(operatorToken.type, left, right);
            currentToken = peek(parser);
        }
        return left;
    }
    return NULL;
}

struct AstNode *parsePrint(Parser *parser)
{
    advance(parser);
    expect(parser, TOKEN_LEFT_PAREN);
    struct AstNode *expression = parseExpression(parser, 0);
    expect(parser, TOKEN_RIGHT_PAREN);
    expect(parser, TOKEN_SEMICOLON);

    struct AstNode *node = makePrintNode(expression);

    return node;
}

struct AstNode *parseIfElse(Parser *parser)
{
    advance(parser);
    expect(parser, TOKEN_LEFT_PAREN);
    struct AstNode *expression = parseExpression(parser, 0);
    expect(parser, TOKEN_RIGHT_PAREN);
    expect(parser, TOKEN_LEFT_CURLY_BRACKET);
    struct AstNode *ifBody = parseStatement(parser);
    expect(parser, TOKEN_RIGHT_CURLY_BRACKET);
    struct AstNode *elseBody = NULL;
    if (peek(parser).type == TOKEN_ELIF)
    {
        elseBody = parseIfElse(parser);
    }
    if (peek(parser).type == TOKEN_ELSE)
    {
        advance(parser);
        expect(parser, TOKEN_LEFT_CURLY_BRACKET);
        elseBody = parseStatement(parser);
        expect(parser, TOKEN_RIGHT_CURLY_BRACKET);
    }

    struct AstNode *node = makeIfElseNode(expression, ifBody, elseBody);
    return node;
}

// MAKE NODES
struct AstNode *makeLiteralNode(Token token)
{
    struct AstNode *node = malloc(sizeof(struct AstNode));
    node->type = NODE_LITERAL;
    node->data.token = token;

    return node;
}

struct AstNode *makeBinaryNode(int operator, struct AstNode *left, struct AstNode *right)
{
    struct AstNode *node = malloc(sizeof(struct AstNode));
    node->type = NODE_BINARY;
    node->data.binary.operator = operator;
    node->data.binary.left = left;
    node->data.binary.right = right;
    return node;
}

struct AstNode *makeDeclarationNode(Token tokenTypeKeyword, Token tokenName, ASTNode *expression)
{
    struct AstNode *node = malloc(sizeof(struct AstNode));
    node->type = NODE_DECLARATION;
    node->data.declaration.name = tokenName;
    node->data.declaration.typeKeyword = tokenTypeKeyword;
    node->data.declaration.expression = expression;

    return node;
}

struct AstNode *makePrintNode(struct AstNode *expression)
{
    struct AstNode *node = malloc(sizeof(struct AstNode));
    node->type = NODE_PRINT;
    node->data.print.expression = expression;

    return node;
}

struct AstNode *makeIfElseNode(struct AstNode *expression, struct AstNode *ifBody, struct AstNode *elseBody)
{
    struct AstNode *node = malloc(sizeof(struct AstNode));
    node->type = NODE_IF_ELSE;
    node->data.ifElse.expression = expression;
    node->data.ifElse.ifBody = ifBody;
    node->data.ifElse.elseBody = elseBody;

    return node;
}

// HELPERS
void printAST(ASTNode *node, int indent)
{
    if (node == NULL)
    {
        printf("NULL");
        return;
    }

    for (int i = 0; i < indent; i++)
    {
        printf(" ");
    }

    if (node->type == NODE_BINARY)
    {
        printf("BINARY: %s\n", getTokenType(node->data.binary.operator));
        printAST(node->data.binary.left, indent + 1);
        printAST(node->data.binary.right, indent + 1);
    }

    if (node->type == NODE_LITERAL)
    {
        printf("LITERAL: %.*s\n", node->data.token.value.length, node->data.token.value.start);
    }

    if (node->type == NODE_DECLARATION)
    {
        printf("DECLARATION: %.*s\n", node->data.declaration.typeKeyword.value.length, node->data.declaration.typeKeyword.value.start);
        for (int i = 0; i < indent; i++)
        {
            printf(" ");
        }
        printf("VARIABLE NAME: %.*s\n", node->data.declaration.name.value.length, node->data.declaration.name.value.start);
        printAST(node->data.declaration.expression, indent + 1);
    }

    if (node->type == NODE_PRINT)
    {
        for (int i = 0; i < indent; i++)
        {
            printf(" ");
        }
        printf("PRINT:\n");
        printAST(node->data.print.expression, indent + 1);
    }

    if (node->type == NODE_IF_ELSE)
    {
        printf("IF: \n");
        printAST(node->data.ifElse.expression, indent + 1);
        for (int i = 0; i < indent; i++)
        {
            printf(" ");
        }
        printf("THEN: \n");
        printAST(node->data.ifElse.ifBody, indent + 1);
        printf("ELSE: \n");
        printAST(node->data.ifElse.elseBody, indent + 1);
    }
}

int isOperator(TokenType type)
{
    int operators[] = {TOKEN_EQUALS,
                       TOKEN_ADD,
                       TOKEN_SUBTRACT,
                       TOKEN_MULTIPLY,
                       TOKEN_DIVIDE,
                       TOKEN_EQUAL_TO,
                       TOKEN_LESS_THAN,
                       TOKEN_GREATER_THAN,
                       TOKEN_LESS_THAN_EQUAL_TO,
                       TOKEN_GREATER_THAN_EQUAL_TO,
                       TOKEN_NOT_EQUAL_TO};

    int size = sizeof(operators) / sizeof(operators[0]);
    for (int i = 0; i < size; i++)
    {
        if (operators[i] == type)
        {
            return 1;
        }
    }
    return 0;
}

char *getTokenType(TokenType type)
{
    switch (type)
    {
    case TOKEN_IDENTIFIER:
        return "IDENTIFIER";
    case TOKEN_EOF:
        return "END OF FILE";

    case TOKEN_LEFT_PAREN:
        return "LEFT PAREN";
    case TOKEN_RIGHT_PAREN:
        return "RIGHT PAREN";
    case TOKEN_SEMICOLON:
        return "SEMICOLON";
    case TOKEN_LEFT_CURLY_BRACKET:
        return "LEFT CURLY BRACKET";
    case TOKEN_RIGHT_CURLY_BRACKET:
        return "RIGHT CURLY BRACKET";

    // Comparison
    case TOKEN_EQUALS:
        return "EQUALS";
    case TOKEN_EQUAL_TO:
        return "EQUAL TO";
    case TOKEN_LESS_THAN:
        return "LESS THAN";
    case TOKEN_GREATER_THAN:
        return "GREATER THAN";
    case TOKEN_LESS_THAN_EQUAL_TO:
        return "LESS THAN EQUAL TO";
    case TOKEN_GREATER_THAN_EQUAL_TO:
        return "GREATER THAN EQUAL TO";
    case TOKEN_NOT_EQUAL_TO:
        return "NOT EQUAL TO";

    // Math
    case TOKEN_ADD:
        return "ADD";
    case TOKEN_INCREMENT:
        return "INCREMENT";
    case TOKEN_SUBTRACT:
        return "SUBTRACT";
    case TOKEN_DECREMENT:
        return "DECREMENT";
    case TOKEN_MULTIPLY:
        return "MULTIPLY";
    case TOKEN_DOUBLE_ASTERISK:
        return "DOUBLE ASTERISK";
    case TOKEN_DIVIDE:
        return "DIVIDE";

    // Values
    case TOKEN_STRING:
        return "STRING";
    case TOKEN_INTEGER_LITERAL:
        return "INTEGER LITERAL";

    // Types
    case TOKEN_INT_TYPE:
        return "INT TYPE";
    case TOKEN_FLOAT_TYPE:
        return "FLOAT TYPE";
    case TOKEN_BOOL_TYPE:
        return "BOOL TYPE";

    // Keywords
    case TOKEN_PRINT:
        return "PRINT";
    case TOKEN_IF:
        return "IF";
    case TOKEN_ELSE:
        return "ELSE";
    case TOKEN_ELIF:
        return "ELIF";
    case TOKEN_FOR:
        return "FOR";
    case TOKEN_WHILE:
        return "WHILE";
    case TOKEN_FUNC:
        return "FUNCTION";

    case TOKEN_VARIABLE:
        return "VARIABLE";

    case TOKEN_NULL:
        return "NULL";

    default:
        return "UNKNOWN";
    }
}

int getTokenPrecedence(TokenType type)
{
    switch (type)
    {
    // Higher Precedence at the top
    case TOKEN_MULTIPLY:
        return PRODUCT;
    case TOKEN_DIVIDE:
        return PRODUCT;

    case TOKEN_ADD:
        return SUM;
    case TOKEN_SUBTRACT:
        return SUM;

    case TOKEN_EQUAL_TO:
        return COMPARISON;
    case TOKEN_LESS_THAN:
        return COMPARISON;
    case TOKEN_GREATER_THAN:
        return COMPARISON;
    case TOKEN_LESS_THAN_EQUAL_TO:
        return COMPARISON;
    case TOKEN_GREATER_THAN_EQUAL_TO:
        return COMPARISON;
    case TOKEN_NOT_EQUAL_TO:
        return COMPARISON;

    case TOKEN_EQUALS:
        return ASSIGNMENT;
    default:
        return 0;
    }
}

void freeAST(ASTNode *node)
{
    if (node == NULL)
    {
        return;
    }

    if (node->type == NODE_BINARY)
    {
        freeAST(node->data.binary.left);
        freeAST(node->data.binary.right);
    }
    else if (node->type == NODE_DECLARATION)
    {
        freeAST(node->data.declaration.expression);
    }
    else if (node->type == NODE_PRINT)
    {
        freeAST(node->data.print.expression);
    }
    else if (node->type == NODE_IF_ELSE)
    {
        freeAST(node->data.ifElse.expression);
        freeAST(node->data.ifElse.ifBody);
        freeAST(node->data.ifElse.elseBody);
    }

    free(node);
}