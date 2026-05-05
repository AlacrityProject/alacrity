#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

int main()
{
    Keyword keywords[3] = {
        {"variable", TOKEN_VARIABLE},
        {"print", TOKEN_PRINT},
        {"int", TOKEN_INT_TYPE},
    };
    int numberOfKeywords = sizeof(keywords) / sizeof(keywords[0]);

    // Read source code
    char *source = readFile("src/main.al");
    if (source == NULL)
        return 1;

    // Assign Tokens
    Token tokens[10000] = {};
    int tokenCount = tokenize(source, tokens);
    printf("Total Token Count: %d\n", tokenCount);

    // Determine Identifier
    for (int i = 0; i < tokenCount; i++)
    {
        if (tokens[i].type == TOKEN_IDENTIFIER)
        {
            tokens[i].type = determine_identifier(tokens[i].value.start, tokens[i].value.length, keywords, numberOfKeywords);
        }
    }

    SymbolTable table = {};

    // Parser Initialization
    Parser parser;
    parser.tokens = tokens;
    parser.currentToken = 0;
    parser.tokenCount = tokenCount;

    // Retrieve and store nodes
    struct AstNode *asts[1000] = {};
    int size = 0;
    while (peek(&parser).type != TOKEN_EOF)
    {
        if (size < 1000)
        {
            struct AstNode *ast = parseStatement(&parser);
            if (ast != NULL)
            {
                asts[size] = ast;
                size++;
            }
        }
    }

    // Print Current tokens
    printf("\nCurrent Tokens: \n");
    for (int i = 0; i < tokenCount; i++)
    {
        printf("Token Type: %s, Length: %d\n", getTokenType(tokens[i].type), tokens[i].value.length);
    }

    // Print Abstract Syntax Tree
    printf("\nABSTRACT SYNTAX TREE:\n");
    for (int i = 0; i < size; i++)
    {
        printAST(asts[i], 0);
    }

    // Print evaluation
    printf("\nProgram Evaluation:\n");
    for (int i = 0; i < size; i++)
    {
        evaluator(asts[i], &table);
    }

    // Free ASTs
    for (int i = 0; i < size; i++)
    {
        freeAST(asts[i]);
    }

    // Free source file
    free(source);
    return 0;
}