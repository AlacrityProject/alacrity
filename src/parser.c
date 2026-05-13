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

Token peekNext(Parser *parser)
{
    if (parser->currentToken + 1 >= parser->tokenCount)
    {
        return parser->tokens[parser->tokenCount - 1];
    }
    return parser->tokens[parser->currentToken + 1];
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
    else if (currentToken.type == TOKEN_VARIABLE)
    {
        struct AstNode *node = parseExpression(parser, 0);
        expect(parser, TOKEN_SEMICOLON);
        return node;
    }
    else if (currentToken.type == TOKEN_FUNC)
    {
        return parseFunctionDeclaration(parser);
    }

    else if (currentToken.type == TOKEN_RETURN)
    {
        return parseReturn(parser);
    }

    else if (currentToken.type == TOKEN_WHILE)
    {
        return parseWhile(parser);
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

    if (currentToken.type == TOKEN_NOT || currentToken.type == TOKEN_SUBTRACT)
    {
        Token operator = currentToken;
        advance(parser);
        struct AstNode *operand = parseExpression(parser, UNARY);
        struct AstNode *unaryNode = makeUnaryNode(operator, operand);
        return unaryNode;
    }

    if (currentToken.type == TOKEN_INTEGER_LITERAL || currentToken.type == TOKEN_VARIABLE)
    {
        struct AstNode *left;

        if (peekNext(parser).type == TOKEN_LEFT_PAREN)
        {
            left = parseFunctionCall(parser);
        }
        else
        {
            left = makeLiteralNode(advance(parser));

            currentToken = peek(parser);

            if (currentToken.type == TOKEN_INCREMENT || currentToken.type == TOKEN_DECREMENT)
            {
                advance(parser);
                struct AstNode *node = makeIncrementDecrementNode(currentToken.type, left->data.token);
                return node;
            }
        }

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
    struct AstNode *ifBody = parseBlock(parser);
    expect(parser, TOKEN_RIGHT_CURLY_BRACKET);
    struct AstNode *elseBody = NULL;
    if (peek(parser).type == TOKEN_ELIF)
    {
        elseBody = parseIfElse(parser);
    }
    else if (peek(parser).type == TOKEN_ELSE)
    {
        advance(parser);
        expect(parser, TOKEN_LEFT_CURLY_BRACKET);
        elseBody = parseBlock(parser);
        expect(parser, TOKEN_RIGHT_CURLY_BRACKET);
    }

    struct AstNode *node = makeIfElseNode(expression, ifBody, elseBody);
    return node;
}

struct AstNode *parseWhile(Parser *parser)
{
    advance(parser);
    expect(parser, TOKEN_LEFT_PAREN);
    struct AstNode *expression = parseExpression(parser, 0);
    expect(parser, TOKEN_RIGHT_PAREN);
    expect(parser, TOKEN_LEFT_CURLY_BRACKET);
    struct AstNode *body = parseBlock(parser);
    expect(parser, TOKEN_RIGHT_CURLY_BRACKET);

    struct AstNode *node = makeWhileNode(expression, body);

    return node;
}

struct AstNode *parseFunctionDeclaration(Parser *parser)
{
    advance(parser);
    Token name = expect(parser, TOKEN_VARIABLE);
    expect(parser, TOKEN_LEFT_PAREN);

    Token currentToken = peek(parser);

    Token *parameters = malloc(sizeof(Token) * 100);
    Token *parameterTypes = malloc(sizeof(Token) * 100);
    int parameterCount = 0;

    while (currentToken.type != TOKEN_RIGHT_PAREN)
    {
        parameterTypes[parameterCount] = advance(parser);
        parameters[parameterCount] = advance(parser);

        parameterCount++;
        currentToken = peek(parser);

        if (currentToken.type == TOKEN_COMMA)
        {
            advance(parser);
            continue;
        }
        if (currentToken.type == TOKEN_RIGHT_PAREN)
        {
            break;
        }
    }

    expect(parser, TOKEN_RIGHT_PAREN);
    expect(parser, TOKEN_ARROW);
    Token returnType = advance(parser);
    expect(parser, TOKEN_LEFT_CURLY_BRACKET);
    struct AstNode *body = parseBlock(parser);
    expect(parser, TOKEN_RIGHT_CURLY_BRACKET);

    struct AstNode *node = makeFunctionNode(name, parameters, parameterTypes, parameterCount, returnType, body);

    return node;
}

struct AstNode *parseFunctionCall(Parser *parser)
{

    Token name = advance(parser);
    expect(parser, TOKEN_LEFT_PAREN);
    struct AstNode **arguments = malloc(sizeof(struct AstNode *) * 100);
    int argumentCount = 0;
    Token currentToken = peek(parser);

    while (currentToken.type != TOKEN_RIGHT_PAREN)
    {
        arguments[argumentCount] = parseExpression(parser, 0);
        argumentCount++;

        currentToken = peek(parser);

        if (currentToken.type == TOKEN_COMMA)
        {
            advance(parser);
            continue;
        }
        if (currentToken.type == TOKEN_RIGHT_PAREN)
        {
            break;
        }
    }

    expect(parser, TOKEN_RIGHT_PAREN);
    struct AstNode *node = makeFunctionCallNode(name, arguments, argumentCount);

    return node;
}

struct AstNode *parseBlock(Parser *parser)
{
    struct AstNode **statements = malloc(sizeof(struct AstNode *) * 100);
    int totalCount = 0;
    Token currentToken = peek(parser);
    while (currentToken.type != TOKEN_RIGHT_CURLY_BRACKET && currentToken.type != TOKEN_EOF)
    {
        statements[totalCount] = parseStatement(parser);
        totalCount++;
        currentToken = peek(parser);
    }

    return makeBlockNode(statements, totalCount);
}

struct AstNode *parseReturn(Parser *parser)
{
    advance(parser);
    struct AstNode *expression = parseExpression(parser, 0);
    expect(parser, TOKEN_SEMICOLON);
    struct AstNode *node = makeReturnNode(expression);

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

struct AstNode *makeUnaryNode(Token operator, struct AstNode *operand)
{
    struct AstNode *node = malloc(sizeof(struct AstNode));
    node->type = NODE_UNARY;
    node->data.unary.operator = operator.type;
    node->data.unary.operand = operand;
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

struct AstNode *makeIncrementDecrementNode(int operator, Token variable)
{
    struct AstNode *node = malloc(sizeof(struct AstNode));
    node->type = NODE_INCREMENT_DECREMENT;
    node->data.incrementDecrement.operator = operator;
    node->data.incrementDecrement.variable = variable;
    return node;
}

struct AstNode *makeDeclarationNode(Token tokenTypeKeyword, Token tokenName, struct AstNode *expression)
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

struct AstNode *makeWhileNode(struct AstNode *expression, struct AstNode *body)
{
    struct AstNode *node = malloc(sizeof(struct AstNode));
    node->type = NODE_WHILE;
    node->data.whileNode.expression = expression;
    node->data.whileNode.body = body;

    return node;
}

struct AstNode *makeBlockNode(struct AstNode **statements, int totalCount)
{
    struct AstNode *node = malloc(sizeof(struct AstNode));
    node->type = NODE_BLOCK;
    node->data.nodeBlock.statements = statements;
    node->data.nodeBlock.totalCount = totalCount;

    return node;
}

struct AstNode *makeFunctionNode(Token name, Token *parameters, Token *parameterTypes, int parameterCount, Token returnType, struct AstNode *body)
{
    struct AstNode *node = malloc(sizeof(struct AstNode));
    node->type = NODE_FUNCTION;
    node->data.function.name = name;
    node->data.function.parameters = parameters;
    node->data.function.parameterTypes = parameterTypes;
    node->data.function.parameterCount = parameterCount;
    node->data.function.returnTypeKeyword = returnType;
    node->data.function.body = body;

    return node;
}

struct AstNode *makeFunctionCallNode(Token name, struct AstNode **arguments, int argumentCount)
{
    struct AstNode *node = malloc(sizeof(struct AstNode));
    node->type = NODE_FUNCTION_CALL;
    node->data.functionCall.name = name;
    node->data.functionCall.arguments = arguments;
    node->data.functionCall.argumentCount = argumentCount;

    return node;
}

struct AstNode *makeReturnNode(struct AstNode *expression)
{
    struct AstNode *node = malloc(sizeof(struct AstNode));
    node->type = NODE_RETURN;
    node->data.returnNode.expression = expression;

    return node;
}

// HELPERS
void printAST(ASTNode *node, int indent)
{
    if (node == NULL)
    {
        for (int i = 0; i < indent; i++)
        {
            printf(" ");
        }
        printf("NULL\n");
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
    if (node->type == NODE_UNARY)
    {
        printf("UNARY: %s\n", getTokenType(node->data.unary.operator));
        printAST(node->data.unary.operand, indent + 1);
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
        for (int i = 0; i < indent; i++)
        {
            printf(" ");
        }
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
        for (int i = 0; i < indent; i++)
        {
            printf(" ");
        }
        printf("ELSE: \n");
        printAST(node->data.ifElse.elseBody, indent + 1);
    }

    if (node->type == NODE_WHILE)
    {
        printf("WHILE: \n");
        printAST(node->data.whileNode.expression, indent + 1);
        for (int i = 0; i < indent; i++)
        {
            printf(" ");
        }
        printf("THEN: \n");
        printAST(node->data.whileNode.body, indent + 1);
        for (int i = 0; i < indent; i++)
        {
            printf(" ");
        }
    }

    if (node->type == NODE_INCREMENT_DECREMENT)
    {
        printf("%s:  %.*s\n", getTokenType(node->data.incrementDecrement.operator), node->data.incrementDecrement.variable.value.length, node->data.incrementDecrement.variable.value.start);
    }

    if (node->type == NODE_FUNCTION)
    {
        printf("FUNCTION: %.*s (Args: %d)\nRETURN TYPE: %.*s\n", node->data.function.name.value.length, node->data.function.name.value.start, node->data.function.parameterCount, node->data.function.returnTypeKeyword.value.length, node->data.function.returnTypeKeyword.value.start);
        printAST(node->data.function.body, indent + 1);
    }

    if (node->type == NODE_FUNCTION_CALL)
    {
        printf("CALL: %.*s\n", node->data.functionCall.name.value.length, node->data.functionCall.name.value.start);
        for (int i = 0; i < node->data.functionCall.argumentCount; i++)
        {
            printAST(node->data.functionCall.arguments[i], indent + 1);
        }
    }

    if (node->type == NODE_RETURN)
    {
        printf("RETURN:\n");
        printAST(node->data.returnNode.expression, indent + 1);
    }
    if (node->type == NODE_BLOCK)
    {
        printf("BLOCK (%d statements):\n", node->data.nodeBlock.totalCount);
        for (int i = 0; i < node->data.nodeBlock.totalCount; i++)
        {
            printAST(node->data.nodeBlock.statements[i], indent + 1);
        }
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
    case TOKEN_ARROW:
        return "ARROW";
    case TOKEN_COMMA:
        return "COMMA";

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
    case TOKEN_RETURN:
        return "RETURN";

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
    if (node->type == NODE_UNARY)
    {
        freeAST(node->data.unary.operand);
    }
    if (node->type == NODE_DECLARATION)
    {
        freeAST(node->data.declaration.expression);
    }
    if (node->type == NODE_PRINT)
    {
        freeAST(node->data.print.expression);
    }
    if (node->type == NODE_IF_ELSE)
    {
        freeAST(node->data.ifElse.expression);
        freeAST(node->data.ifElse.ifBody);
        freeAST(node->data.ifElse.elseBody);
    }
    if (node->type == NODE_WHILE)
    {
        freeAST(node->data.whileNode.expression);
        freeAST(node->data.whileNode.body);
    }
    if (node->type == NODE_FUNCTION)
    {
        free(node->data.function.parameters);
        free(node->data.function.parameterTypes);
        freeAST(node->data.function.body);
    }
    if (node->type == NODE_FUNCTION_CALL)
    {
        int argumentCount = node->data.functionCall.argumentCount;
        for (int i = 0; i < argumentCount; i++)
        {
            freeAST(node->data.functionCall.arguments[i]);
        }
        free(node->data.functionCall.arguments);
    }
    if (node->type == NODE_RETURN)
    {
        freeAST(node->data.returnNode.expression);
    }
    if (node->type == NODE_BLOCK)
    {
        int totalCount = node->data.nodeBlock.totalCount;
        for (int i = 0; i < totalCount; i++)
        {
            freeAST(node->data.nodeBlock.statements[i]);
        }
        free(node->data.nodeBlock.statements);
    }

    free(node);
}