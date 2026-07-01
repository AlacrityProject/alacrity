#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ast.h"

char *source;

char *readFile(char filename[])
{
    FILE *pFile = fopen(filename, "r");
    if (pFile == NULL)
    {
        printf("ERROR OPENING FILE\n");
        return NULL;
    }

    fseek(pFile, 0, SEEK_END);
    long size = ftell(pFile);
    rewind(pFile);
    char *buffer = malloc(size + 1);
    fread(buffer, 1, size, pFile);
    buffer[size] = '\0';
    fclose(pFile);

    return buffer;
}

void printErrorLine(Token token)
{
    // walk backwards from token.start to find line beginning
    // walk forwards from token.start to find line end
    // print the line number
    // print the source line between those two points

    char *lineStart = token.value.start;
    while (lineStart > source && *(lineStart - 1) != '\n')
    {
        lineStart--;
    }

    char *lineEnd = token.value.start;
    while (*lineEnd != '\n' && *lineEnd != '\0')
    {
        lineEnd++;
    }

    fprintf(stderr, "Line %d:\n", token.line);
    fprintf(stderr, "   %.*s\n", (int)(lineEnd - lineStart), lineStart);
}

int tokenize(char source[], Token tokens[])
{
    int tokenCount = 0;
    int currentLine = 1;

    for (int i = 0; source[i] != '\0'; i++)
    {
        switch (source[i])
        {
        case ';':
            tokens[tokenCount].type = TOKEN_SEMICOLON;
            tokens[tokenCount].line = currentLine;
            tokens[tokenCount].value.start = &source[i];
            tokens[tokenCount].value.length = 1;

            tokenCount++;
            break;

        case ',':
            tokens[tokenCount].type = TOKEN_COMMA;
            tokens[tokenCount].line = currentLine;
            tokens[tokenCount].value.start = &source[i];
            tokens[tokenCount].value.length = 1;

            tokenCount++;
            break;

        case '(':
            tokens[tokenCount].type = TOKEN_LEFT_PAREN;
            tokens[tokenCount].line = currentLine;
            tokens[tokenCount].value.start = &source[i];
            tokens[tokenCount].value.length = 1;

            tokenCount++;
            break;

        case ')':
            tokens[tokenCount].type = TOKEN_RIGHT_PAREN;
            tokens[tokenCount].line = currentLine;
            tokens[tokenCount].value.start = &source[i];
            tokens[tokenCount].value.length = 1;

            tokenCount++;
            break;

        case '{':
            tokens[tokenCount].type = TOKEN_LEFT_CURLY_BRACKET;
            tokens[tokenCount].line = currentLine;
            tokens[tokenCount].value.start = &source[i];
            tokens[tokenCount].value.length = 1;

            tokenCount++;
            break;

        case '}':
            tokens[tokenCount].type = TOKEN_RIGHT_CURLY_BRACKET;
            tokens[tokenCount].line = currentLine;
            tokens[tokenCount].value.start = &source[i];
            tokens[tokenCount].value.length = 1;

            tokenCount++;
            break;

        case '=':
            if (source[i + 1] == '=')
            {
                tokens[tokenCount].type = TOKEN_EQUAL_TO;
                tokens[tokenCount].line = currentLine;
                tokens[tokenCount].value.start = &source[i];
                tokens[tokenCount].value.length = 2;
                i++;
            }

            else
            {
                tokens[tokenCount].type = TOKEN_EQUALS;
                tokens[tokenCount].line = currentLine;
                tokens[tokenCount].value.start = &source[i];
                tokens[tokenCount].value.length = 1;
            }

            tokenCount++;
            break;
        case '+':
            if (source[i + 1] == '+')
            {
                tokens[tokenCount].type = TOKEN_INCREMENT;
                tokens[tokenCount].line = currentLine;
                tokens[tokenCount].value.start = &source[i];
                tokens[tokenCount].value.length = 2;
                i++;
            }

            else
            {
                tokens[tokenCount].type = TOKEN_ADD;
                tokens[tokenCount].line = currentLine;
                tokens[tokenCount].value.start = &source[i];
                tokens[tokenCount].value.length = 1;
            }

            tokenCount++;
            break;
        case '-':
            if (source[i + 1] == '-')
            {
                tokens[tokenCount].type = TOKEN_DECREMENT;
                tokens[tokenCount].line = currentLine;
                tokens[tokenCount].value.start = &source[i];
                tokens[tokenCount].value.length = 2;
                i++;
            }

            else if (source[i + 1] == '>')
            {
                tokens[tokenCount].type = TOKEN_ARROW;
                tokens[tokenCount].line = currentLine;
                tokens[tokenCount].value.start = &source[i];
                tokens[tokenCount].value.length = 2;
                i++;
            }

            else
            {
                tokens[tokenCount].type = TOKEN_SUBTRACT;
                tokens[tokenCount].line = currentLine;
                tokens[tokenCount].value.start = &source[i];
                tokens[tokenCount].value.length = 1;
            }

            tokenCount++;
            break;
        case '*':
            if (source[i + 1] == '*')
            {
                tokens[tokenCount].type = TOKEN_DOUBLE_ASTERISK;
                tokens[tokenCount].line = currentLine;
                tokens[tokenCount].value.start = &source[i];
                tokens[tokenCount].value.length = 2;
                i++;
            }

            else
            {
                tokens[tokenCount].type = TOKEN_MULTIPLY;
                tokens[tokenCount].line = currentLine;
                tokens[tokenCount].value.start = &source[i];
                tokens[tokenCount].value.length = 1;
            }

            tokenCount++;
            break;
        case '/':
            if (source[i + 1] == '/')
            {
                while (source[i] != '\n' && source[i] != '\0')
                {
                    i++;
                }
                if (source[i] == '\0')
                {
                    i--;
                }
                break;
            }

            else
            {
                tokens[tokenCount].type = TOKEN_DIVIDE;
                tokens[tokenCount].line = currentLine;
                tokens[tokenCount].value.start = &source[i];
                tokens[tokenCount].value.length = 1;
                tokenCount++;
            }

            break;

        case '<':
            if (source[i + 1] == '=')
            {
                tokens[tokenCount].type = TOKEN_LESS_THAN_EQUAL_TO;
                tokens[tokenCount].line = currentLine;
                tokens[tokenCount].value.start = &source[i];
                tokens[tokenCount].value.length = 2;
                i++;
            }

            else
            {
                tokens[tokenCount].type = TOKEN_LESS_THAN;
                tokens[tokenCount].line = currentLine;
                tokens[tokenCount].value.start = &source[i];
                tokens[tokenCount].value.length = 1;
            }

            tokenCount++;
            break;
        case '>':
            if (source[i + 1] == '=')
            {
                tokens[tokenCount].type = TOKEN_GREATER_THAN_EQUAL_TO;
                tokens[tokenCount].line = currentLine;
                tokens[tokenCount].value.start = &source[i];
                tokens[tokenCount].value.length = 2;
                i++;
            }

            else
            {
                tokens[tokenCount].type = TOKEN_GREATER_THAN;
                tokens[tokenCount].line = currentLine;
                tokens[tokenCount].value.start = &source[i];
                tokens[tokenCount].value.length = 1;
            }

            tokenCount++;
            break;
        case '!':
            if (source[i + 1] == '=')
            {
                tokens[tokenCount].type = TOKEN_NOT_EQUAL_TO;
                tokens[tokenCount].line = currentLine;
                tokens[tokenCount].value.start = &source[i];
                tokens[tokenCount].value.length = 2;
                i++;
            }

            else
            {
                tokens[tokenCount].type = TOKEN_NOT;
                tokens[tokenCount].line = currentLine;
                tokens[tokenCount].value.start = &source[i];
                tokens[tokenCount].value.length = 1;
            }

            tokenCount++;
            break;

        case '"':
            int startIndex = i + 1;
            int closer = startIndex;

            while (source[closer] != '"' && source[closer] != '\0')
                closer++;

            tokens[tokenCount].type = TOKEN_STRING_LITERAL;
            tokens[tokenCount].line = currentLine;
            tokens[tokenCount].value.start = &source[startIndex];
            tokens[tokenCount].value.length = closer - startIndex;

            tokenCount++;
            i = closer;

            break;

        case ' ':
            break;
        case '\n':
            currentLine++;
            break;
        default:
            if (isalpha(source[i]) || source[i] == '_')
            {
                int startIndex = i;
                int closer = startIndex;

                while (isalnum(source[closer]) || source[closer] == '_')
                    closer++;

                tokens[tokenCount].type = TOKEN_IDENTIFIER;
                tokens[tokenCount].line = currentLine;
                tokens[tokenCount].value.start = &source[startIndex];
                tokens[tokenCount].value.length = closer - startIndex;

                tokenCount++;
                i = closer - 1;

                break;
            }

            else if (isdigit(source[i]))
            {
                int startIndex = i;
                int closer = startIndex;
                bool isFloat = false;

                while (isdigit(source[closer]))
                    closer++;

                if (source[closer] == '.')
                {
                    isFloat = true;
                    closer++;
                    while (isdigit(source[closer]))
                        closer++;
                }

                if (isFloat)
                {
                    tokens[tokenCount].type = TOKEN_FLOAT_LITERAL;
                }
                else
                {
                    tokens[tokenCount].type = TOKEN_INTEGER_LITERAL;
                }

                tokens[tokenCount].line = currentLine;
                tokens[tokenCount].value.start = &source[startIndex];
                tokens[tokenCount].value.length = closer - startIndex;

                tokenCount++;
                i = closer - 1;

                break;
            }
            else
            {
                break;
            }
        }
    }
    tokens[tokenCount].type = TOKEN_EOF;
    tokens[tokenCount].value.start = 0;
    tokens[tokenCount].value.length = 0;

    tokenCount++;

    return tokenCount;
}

int determine_identifier(char *tokenStart, int length, Keyword keywords[], int numberOfKeywords)
{
    for (int i = 0; i < numberOfKeywords; i++)
    {
        if (keywords[i].key == NULL)
            break;

        int keywordLength = strlen(keywords[i].key);
        if (keywordLength != length)
        {
            continue;
        }

        int compare = strncmp(tokenStart, keywords[i].key, length);
        if (compare == 0)
        {
            return keywords[i].type;
        }
    }

    return TOKEN_VARIABLE;
}
