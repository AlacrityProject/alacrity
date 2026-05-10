#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "ast.h"

void storeEntry(SymbolTable *table, char variableName[], int valueToStore)
{
    Entry entry;
    strncpy(entry.name, variableName, strlen(variableName));
    entry.name[strlen(variableName)] = '\0';
    entry.value = valueToStore;
    table->entries[table->totalCount] = entry;
    table->totalCount++;
}

Entry *findEntry(SymbolTable *table, char variableName[])
{
    for (int i = 0; i < table->totalCount; i++)
    {
        if (strcmp(table->entries[i].name, variableName) == 0)
        {
            return &table->entries[i];
        }
    }

    return NULL;
}

int evaluator(ASTNode *ast, SymbolTable *table)
{
    if (ast == NULL)
    {
        return 0;
    }

    if (ast->type == NODE_LITERAL)
    {
        if (ast->data.token.type == TOKEN_INTEGER_LITERAL)
        {
            char buffer[100];
            strncpy(buffer, ast->data.token.value.start, ast->data.token.value.length);
            buffer[ast->data.token.value.length] = '\0';

            return atoi(buffer);
        }
        if (ast->data.token.type == TOKEN_VARIABLE)
        {
            char name[100];

            strncpy(name, ast->data.token.value.start, ast->data.token.value.length);
            name[ast->data.token.value.length] = '\0';
            Entry *entry = findEntry(table, name);

            if (entry == NULL)
            {
                fprintf(stderr, "ERROR!\n Cannot find entry for variable name: %s\n", name);
                exit(1);
            }

            return entry->value;
        }
    }

    if (ast->type == NODE_UNARY)
    {
        int operator = ast->data.unary.operator;
        int value = evaluator(ast->data.unary.operand, table);

        switch (operator)
        {
        case TOKEN_SUBTRACT:
            return -value;
        case TOKEN_NOT:
            return !value;
        default:
            break;
        }
    }

    if (ast->type == NODE_BINARY)
    {
        int left = evaluator(ast->data.binary.left, table);
        int right = evaluator(ast->data.binary.right, table);
        int operator = ast->data.binary.operator;

        switch (operator)
        {
        case TOKEN_ADD:
            return left + right;
        case TOKEN_SUBTRACT:
            return left - right;
        case TOKEN_MULTIPLY:
            return left * right;
        case TOKEN_DIVIDE:
            return left / right;
        case TOKEN_EQUAL_TO:
            return left == right;
        case TOKEN_LESS_THAN:
            return left < right;
        case TOKEN_GREATER_THAN:
            return left > right;
        case TOKEN_LESS_THAN_EQUAL_TO:
            return left <= right;
        case TOKEN_GREATER_THAN_EQUAL_TO:
            return left >= right;
        case TOKEN_NOT_EQUAL_TO:
            return left != right;
        default:
            break;
        }
    }

    if (ast->type == NODE_DECLARATION)
    {
        int expression = evaluator(ast->data.declaration.expression, table);
        char name[100];
        strncpy(name, ast->data.declaration.name.value.start, ast->data.declaration.name.value.length);
        name[ast->data.declaration.name.value.length] = '\0';
        storeEntry(table, name, expression);
        return 0;
    }

    if (ast->type == NODE_PRINT)
    {
        int expression = evaluator(ast->data.print.expression, table);
        printf("%d\n", expression);
        return 0;
    }

    if (ast->type == NODE_IF_ELSE)
    {
        int expression = evaluator(ast->data.ifElse.expression, table);

        if (expression == true)
        {
            evaluator(ast->data.ifElse.ifBody, table);
        }
        else if (ast->data.ifElse.elseBody != NULL)
        {
            evaluator(ast->data.ifElse.elseBody, table);
        }

        return 0;
    }

    if (ast->type == NODE_INCREMENT_DECREMENT)
    {
        int operator = ast->data.incrementDecrement.operator;
        char name[100];
        strncpy(name, ast->data.incrementDecrement.variable.value.start, ast->data.incrementDecrement.variable.value.length);
        name[ast->data.incrementDecrement.variable.value.length] = '\0';
        Entry *entry = findEntry(table, name);

        if (entry == NULL)
        {
            fprintf(stderr, "ERROR!\n Cannot find entry for variable name: %s\n", name);
            exit(1);
        }

        int entryValue = entry->value;

        switch (operator)
        {
        case TOKEN_INCREMENT:
            entryValue = entryValue + 1;
            break;
        case TOKEN_DECREMENT:
            entryValue = entryValue - 1;
            break;
        default:
            break;
        }

        entry->value = entryValue;
    }

    return 0;
}