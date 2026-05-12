#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "ast.h"

/*
 TODO for store entry functions:
    check using findEntry() if a variable already exists for reassignment.
*/

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

void storeFunctionEntry(FunctionTable *functionTable, char functionName[], struct AstNode *nodeToStore)
{
    FunctionEntry entry;
    strncpy(entry.name, functionName, strlen(functionName));
    entry.name[strlen(functionName)] = '\0';
    entry.node = nodeToStore;
    functionTable->entries[functionTable->totalCount] = entry;
    functionTable->totalCount++;
}

FunctionEntry *findFunctionEntry(FunctionTable *functionTable, char functionName[])
{
    for (int i = 0; i < functionTable->totalCount; i++)
    {
        if (strcmp(functionTable->entries[i].name, functionName) == 0)
        {
            return &functionTable->entries[i];
        }
    }

    return NULL;
}

int evaluator(ASTNode *ast, SymbolTable *table, FunctionTable *functionTable, ReturnResult *result)
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
        int value = evaluator(ast->data.unary.operand, table, functionTable, result);

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
        int left = evaluator(ast->data.binary.left, table, functionTable, result);
        int right = evaluator(ast->data.binary.right, table, functionTable, result);
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
        int expression = evaluator(ast->data.declaration.expression, table, functionTable, result);
        char name[100];
        strncpy(name, ast->data.declaration.name.value.start, ast->data.declaration.name.value.length);
        name[ast->data.declaration.name.value.length] = '\0';
        storeEntry(table, name, expression);
        return 0;
    }

    if (ast->type == NODE_PRINT)
    {
        int expression = evaluator(ast->data.print.expression, table, functionTable, result);
        printf("%d\n", expression);
        return 0;
    }

    if (ast->type == NODE_IF_ELSE)
    {
        int expression = evaluator(ast->data.ifElse.expression, table, functionTable, result);
        if (result->returned)
        {
            return result->value;
        }

        if (expression == true)
        {
            evaluator(ast->data.ifElse.ifBody, table, functionTable, result);
        }
        else if (ast->data.ifElse.elseBody != NULL)
        {
            evaluator(ast->data.ifElse.elseBody, table, functionTable, result);
        }

        if (result->returned)
        {
            return result->value;
        }

        return 0;
    }

    if (ast->type == NODE_WHILE)
    {
        int expression = evaluator(ast->data.whileNode.expression, table, functionTable, result);
        if (result->returned)
        {
            return result->value;
        }

        while (expression == true)
        {
            evaluator(ast->data.whileNode.body, table, functionTable, result);
            if (result->returned)
            {
                break;
            }
            expression = evaluator(ast->data.whileNode.expression, table, functionTable, result);
        }

        if (result->returned)
        {
            return result->value;
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

    if (ast->type == NODE_FUNCTION)
    {
        char name[100];
        strncpy(name, ast->data.function.name.value.start, ast->data.function.name.value.length);
        name[ast->data.function.name.value.length] = '\0';
        storeFunctionEntry(functionTable, name, ast);

        return 0;
    }

    if (ast->type == NODE_FUNCTION_CALL)
    {
        char name[100];
        strncpy(name, ast->data.functionCall.name.value.start, ast->data.functionCall.name.value.length);
        name[ast->data.functionCall.name.value.length] = '\0';
        FunctionEntry *functionEntry = findFunctionEntry(functionTable, name);

        if (functionEntry == NULL)
        {
            fprintf(stderr, "ERROR!\n Cannot find function entry for function name: %s\n", name);
            exit(1);
        }

        SymbolTable localTable;
        localTable.totalCount = 0;

        struct AstNode *node = functionEntry->node;

        for (int i = 0; i < ast->data.functionCall.argumentCount; i++)
        {
            int value = evaluator(ast->data.functionCall.arguments[i], table, functionTable, result);

            char parameterName[100];
            Token parameterToken = node->data.function.parameters[i];
            strncpy(parameterName, parameterToken.value.start, parameterToken.value.length);
            parameterName[parameterToken.value.length] = '\0';

            storeEntry(&localTable, parameterName, value);
        }

        ReturnResult localResult = {0, false};

        evaluator(node->data.function.body, &localTable, functionTable, &localResult);

        return localResult.value;
    }
    if (ast->type == NODE_BLOCK)
    {
        for (int i = 0; i < ast->data.nodeBlock.totalCount; i++)
        {
            evaluator(ast->data.nodeBlock.statements[i], table, functionTable, result);

            if (result->returned)
            {
                break;
            }
        }
        return result->value;
    }
    if (ast->type == NODE_RETURN)
    {
        int expression = evaluator(ast->data.returnNode.expression, table, functionTable, result);
        result->value = expression;
        result->returned = true;
        return expression;
    }

    return 0;
}