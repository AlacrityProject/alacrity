#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "ast.h"

void storeEntry(SymbolTable *table, char variableName[], Value valueToStore)
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

Value makeIntValue(int number)
{
    Value intValue;
    intValue.type = TYPE_INT;
    intValue.as.int_value = number;
    return intValue;
}
Value makeBoolValue(bool boolean)
{
    Value boolValue;
    boolValue.type = TYPE_BOOL;
    boolValue.as.bool_value = boolean;
    return boolValue;
}
Value makeFloatValue(float number)
{
    Value floatValue;
    floatValue.type = TYPE_FLOAT;
    floatValue.as.float_value = number;
    return floatValue;
}
Value makeStringValue(char *string)
{
    Value stringValue;
    stringValue.type = TYPE_STRING;
    stringValue.as.string_value = string;
    return stringValue;
}
Value makeNullValue()
{
    Value NullValue;
    NullValue.type = TYPE_NULL;
    NullValue.as.int_value = 0;
    return NullValue;
}

bool isTruthy(Value value)
{
    ValueType valueType = value.type;
    switch (valueType)
    {
    case TYPE_BOOL:
        return value.as.bool_value;
    case TYPE_INT:
        return value.as.int_value != 0;
    case TYPE_FLOAT:
        return value.as.float_value != 0.0;
    case TYPE_STRING:
        return value.as.string_value != NULL;
    case TYPE_NULL:
        return false;
    default:
        return false;
    }
}

void freeValue(Value value)
{
    switch (value.type)
    {
    case TYPE_STRING:
        free(value.as.string_value);
        break;
    default:
        break;
    }
}

char *getValueType(ValueType valueType)
{

    switch (valueType)
    {
    case TYPE_INT:
        return "int";
    case TYPE_FLOAT:
        return "float";
    case TYPE_BOOL:
        return "bool";
    case TYPE_STRING:
        return "string";
    case TYPE_NULL:
        return "null";
    default:
        return "unknown";
    }
}

Value performFloatBinaryOp(float leftFloat, float rightFloat, Token operator)
{
    switch (operator.type)
    {
    case TOKEN_ADD:
        return makeFloatValue(leftFloat + rightFloat);
    case TOKEN_SUBTRACT:
        return makeFloatValue(leftFloat - rightFloat);
    case TOKEN_MULTIPLY:
        return makeFloatValue(leftFloat * rightFloat);
    case TOKEN_DIVIDE:
        return makeFloatValue(leftFloat / rightFloat);
    case TOKEN_EQUAL_TO:
        return makeBoolValue(leftFloat == rightFloat);
    case TOKEN_LESS_THAN:
        return makeBoolValue(leftFloat < rightFloat);
    case TOKEN_GREATER_THAN:
        return makeBoolValue(leftFloat > rightFloat);
    case TOKEN_LESS_THAN_EQUAL_TO:
        return makeBoolValue(leftFloat <= rightFloat);
    case TOKEN_GREATER_THAN_EQUAL_TO:
        return makeBoolValue(leftFloat >= rightFloat);
    case TOKEN_NOT_EQUAL_TO:
        return makeBoolValue(leftFloat != rightFloat);
    default:
        char *operatorName = getTokenType(operator.type);
        char errorDescription[100];
        sprintf(errorDescription, "Operator '%s' is not valid", operatorName);
        reportError(operator, errorDescription);
    }
}

Value performBinaryOp(Value left, Value right, Token operator)
{
    if (left.type == right.type)
    {
        switch (left.type)
        {
        case TYPE_INT:
        {
            int leftValue = left.as.int_value;
            int rightValue = right.as.int_value;
            switch (operator.type)
            {
            case TOKEN_ADD:
                return makeIntValue(leftValue + rightValue);
            case TOKEN_SUBTRACT:
                return makeIntValue(leftValue - rightValue);
            case TOKEN_MULTIPLY:
                return makeIntValue(leftValue * rightValue);
            case TOKEN_DIVIDE:
                return makeIntValue(leftValue / rightValue);
            case TOKEN_EQUAL_TO:
                return makeBoolValue(leftValue == rightValue);
            case TOKEN_LESS_THAN:
                return makeBoolValue(leftValue < rightValue);
            case TOKEN_GREATER_THAN:
                return makeBoolValue(leftValue > rightValue);
            case TOKEN_LESS_THAN_EQUAL_TO:
                return makeBoolValue(leftValue <= rightValue);
            case TOKEN_GREATER_THAN_EQUAL_TO:
                return makeBoolValue(leftValue >= rightValue);
            case TOKEN_NOT_EQUAL_TO:
                return makeBoolValue(leftValue != rightValue);
            default:
                break;
            }
            break;
        }
        case TYPE_FLOAT:
        {
            float leftFloat = left.as.float_value;
            float rightFloat = right.as.float_value;
            return performFloatBinaryOp(leftFloat, rightFloat, operator);
        }
        case TYPE_BOOL:
            switch (operator.type)
            {
            case TOKEN_EQUAL_TO:
                return makeBoolValue(left.as.bool_value == right.as.bool_value);
            case TOKEN_NOT_EQUAL_TO:
                return makeBoolValue(left.as.bool_value != right.as.bool_value);
            default:
                break;
            }
            break;

        default:
            break;
        }
    }

    if ((left.type == TYPE_INT && right.type == TYPE_FLOAT))
    {
        float leftFloat = (float)left.as.int_value;
        float rightFloat = right.as.float_value;

        return performFloatBinaryOp(leftFloat, rightFloat, operator);
    }
    if ((left.type == TYPE_FLOAT && right.type == TYPE_INT))
    {
        float leftFloat = left.as.float_value;
        float rightFloat = (float)right.as.int_value;
        return performFloatBinaryOp(leftFloat, rightFloat, operator);
    }

    // Error handling

    char *operatorName = getTokenType(operator.type);
    char *leftType = getValueType(left.type);
    char *rightType = getValueType(right.type);

    char errorDescription[100];
    sprintf(errorDescription, "Cannot apply operator '%s' to types '%s' and '%s'", operatorName, leftType, rightType);
    reportError(operator, errorDescription);
}

Value evaluator(ASTNode *ast, SymbolTable *table, FunctionTable *functionTable, ReturnResult *result)
{
    if (ast == NULL)
    {
        return makeNullValue();
    }

    if (ast->type == NODE_LITERAL)
    {
        if (ast->data.token.type == TOKEN_INTEGER_LITERAL)
        {
            char buffer[100];
            strncpy(buffer, ast->data.token.value.start, ast->data.token.value.length);
            buffer[ast->data.token.value.length] = '\0';

            return makeIntValue(atoi(buffer));
        }
        if (ast->data.token.type == TOKEN_FLOAT_LITERAL)
        {
            char buffer[100];
            strncpy(buffer, ast->data.token.value.start, ast->data.token.value.length);
            buffer[ast->data.token.value.length] = '\0';

            return makeFloatValue(atof(buffer));
        }
        if (ast->data.token.type == TOKEN_STRING_LITERAL)
        {
            char *stringMemory = (char *)malloc(sizeof(char) * (ast->data.token.value.length + 1));

            strncpy(stringMemory, ast->data.token.value.start, ast->data.token.value.length);
            stringMemory[ast->data.token.value.length] = '\0';

            return makeStringValue(stringMemory);
        }

        if (ast->data.token.type == TOKEN_BOOL_LITERAL_TRUE)
        {
            return makeBoolValue(true);
        }
        if (ast->data.token.type == TOKEN_BOOL_LITERAL_FALSE)
        {
            return makeBoolValue(false);
        }
        if (ast->data.token.type == TOKEN_VARIABLE)
        {
            char name[100];

            strncpy(name, ast->data.token.value.start, ast->data.token.value.length);
            name[ast->data.token.value.length] = '\0';
            Entry *entry = findEntry(table, name);

            if (entry == NULL)
            {
                char errorDescription[100];
                sprintf(errorDescription, "Cannot find entry for variable name: %s\n", name);
                reportError(ast->data.token, errorDescription);
            }

            return entry->value;
        }
    }

    if (ast->type == NODE_UNARY)
    {
        Token operator = ast->data.unary.operator;
        Value value = evaluator(ast->data.unary.operand, table, functionTable, result);
        ValueType valueType = value.type;

        if (valueType == TYPE_INT && operator.type == TOKEN_SUBTRACT)
        {
            int intValue = value.as.int_value;
            return makeIntValue(-intValue);
        }

        if (valueType == TYPE_FLOAT && operator.type == TOKEN_SUBTRACT)
        {
            float floatValue = value.as.float_value;
            return makeFloatValue(-floatValue);
        }

        return makeBoolValue(!isTruthy(value));
    }

    if (ast->type == NODE_BINARY)
    {
        Value left = evaluator(ast->data.binary.left, table, functionTable, result);
        Value right = evaluator(ast->data.binary.right, table, functionTable, result);
        Token operator = ast->data.binary.operator;

        return performBinaryOp(left, right, operator);
    }

    if (ast->type == NODE_DECLARATION)
    {
        Value expression = evaluator(ast->data.declaration.expression, table, functionTable, result);
        char name[100];
        strncpy(name, ast->data.declaration.name.value.start, ast->data.declaration.name.value.length);
        name[ast->data.declaration.name.value.length] = '\0';
        storeEntry(table, name, expression);
        return makeNullValue();
    }

    if (ast->type == NODE_ASSIGNMENT)
    {
        char name[100];
        strncpy(name, ast->data.assignment.name.value.start, ast->data.assignment.name.value.length);
        name[ast->data.assignment.name.value.length] = '\0';

        Entry *entry = findEntry(table, name);

        if (entry)
        {
            Value expression = evaluator(ast->data.assignment.expression, table, functionTable, result);
            freeValue(entry->value);
            entry->value = expression;
            return expression;
        }

        char errorDescription[100];
        sprintf(errorDescription, "Variable does not exist!");
        reportError(ast->data.assignment.name, errorDescription);
    }

    if (ast->type == NODE_PRINT)
    {
        Value expression = evaluator(ast->data.print.expression, table, functionTable, result);

        ValueType expressionType = expression.type;

        switch (expressionType)
        {
        case TYPE_INT:
            printf("%d\n", expression.as.int_value);
            break;
        case TYPE_FLOAT:
            printf("%f\n", expression.as.float_value);
            break;
        case TYPE_STRING:
            printf("%s\n", expression.as.string_value);
            break;
        case TYPE_BOOL:
            printf("%d\n", expression.as.bool_value);
            break;
        case TYPE_NULL:
            printf("NULL");
            break;
        }

        return makeNullValue();
    }

    if (ast->type == NODE_IF_ELSE)
    {
        Value expression = evaluator(ast->data.ifElse.expression, table, functionTable, result);
        if (result->returned)
        {
            return result->value;
        }

        if (isTruthy(expression) == true)
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

        return makeNullValue();
    }

    if (ast->type == NODE_WHILE)
    {
        Value expression = evaluator(ast->data.whileNode.expression, table, functionTable, result);
        if (result->returned)
        {
            return result->value;
        }

        while (isTruthy(expression) == true)
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

        return makeNullValue();
    }

    if (ast->type == NODE_INCREMENT_DECREMENT)
    {
        Token operator = ast->data.incrementDecrement.operator;
        char name[100];
        strncpy(name, ast->data.incrementDecrement.variable.value.start, ast->data.incrementDecrement.variable.value.length);
        name[ast->data.incrementDecrement.variable.value.length] = '\0';
        Entry *entry = findEntry(table, name);

        if (entry == NULL)
        {
            char errorDescription[100];
            sprintf(errorDescription, "Cannot find entry for variable name: %s\n", name);
            reportError(ast->data.incrementDecrement.variable, errorDescription);
        }

        Value entryValue = entry->value;

        switch (operator.type)
        {
        case TOKEN_INCREMENT:
            entryValue = makeIntValue(entryValue.as.int_value + 1);
            break;
        case TOKEN_DECREMENT:
            entryValue = makeIntValue(entryValue.as.int_value - 1);
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

        return makeNullValue();
    }

    if (ast->type == NODE_FUNCTION_CALL)
    {
        char name[100];
        strncpy(name, ast->data.functionCall.name.value.start, ast->data.functionCall.name.value.length);
        name[ast->data.functionCall.name.value.length] = '\0';
        FunctionEntry *functionEntry = findFunctionEntry(functionTable, name);

        if (functionEntry == NULL)
        {
            char errorDescription[100];
            sprintf(errorDescription, "Cannot find function entry for function name: %s\n", name);
            reportError(ast->data.functionCall.name, errorDescription);
        }

        SymbolTable localTable;
        localTable.totalCount = 0;

        struct AstNode *node = functionEntry->node;

        for (int i = 0; i < ast->data.functionCall.argumentCount; i++)
        {
            Value value = evaluator(ast->data.functionCall.arguments[i], table, functionTable, result);

            char parameterName[100];
            Token parameterToken = node->data.function.parameters[i];
            strncpy(parameterName, parameterToken.value.start, parameterToken.value.length);
            parameterName[parameterToken.value.length] = '\0';

            storeEntry(&localTable, parameterName, value);
        }

        ReturnResult localResult;
        localResult.returned = false;
        localResult.value = makeNullValue();

        evaluator(node->data.function.body, &localTable, functionTable, &localResult);

        for (int i = 0; i < localTable.totalCount; i++)
        {
            if (localResult.value.type == TYPE_STRING && localTable.entries[i].value.type == TYPE_STRING && localTable.entries[i].value.as.string_value == localResult.value.as.string_value)
            {
                continue;
            }
            freeValue(localTable.entries[i].value);
        }

        return localResult.value;
    }
    if (ast->type == NODE_BLOCK)
    {
        for (int i = 0; i < ast->data.nodeBlock.totalCount; i++)
        {
            evaluator(ast->data.nodeBlock.statements[i], table, functionTable, result);

            if (result->returned)
            {
                return result->value;
            }
        }
        return makeNullValue();
    }
    if (ast->type == NODE_RETURN)
    {
        Value expression = evaluator(ast->data.returnNode.expression, table, functionTable, result);
        result->value = expression;
        result->returned = true;
        return expression;
    }

    return makeNullValue();
}
