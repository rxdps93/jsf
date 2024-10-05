#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define JSON_WHITESPACE ((const char[]) { ' ', '\t', '\b', '\n', '\r' })

typedef enum
{
    JSON_COMMA,         // ,
    JSON_COLON,         // :
    JSON_OPENBRACKET,   // [
    JSON_CLOSEBRACKET,  // ]
    JSON_OPENBRACE,     // {
    JSON_CLOSEBRACE,    // }
    JSON_QUOTE,         // "
    JSON_STRING,        // Any text wrapped in quotes
    JSON_NUMBER,        // Any digits not wrapped in quotes
    JSON_BOOLEAN,       // The words true or false not wrapped in quotes         
    JSON_NULL           // The word null not wrapped in quotes
} token_type_t;

typedef struct token
{
    token_type_t type;
    char value[8192];
} token_t;

int is_digit(char c)
{
    return (c >= 48) && (c <= 57);
}

int is_whitespace(char c)
{
    for (int i = 0; i < 5; i++)
    {
        if (c == JSON_WHITESPACE[i])
        {
            return 1;
        }
    }
    return 0;
}

int json_lexer(char *pipe, int len)
{
    token_t tokens[65536];
    int t = 0;
    for (int i = 0; i < len; i++)
    {
        char c = pipe[i];
        token_t token;
        switch (c)
        {
            case ',':
                token.type = JSON_COMMA;
                strcpy(token.value, ",");
                break;
            case ':':
                token.type = JSON_COLON;
                strcpy(token.value, ":");
                break;
            case '[':
                token.type = JSON_OPENBRACKET;
                strcpy(token.value, "[");
                break;
            case ']':
                token.type = JSON_CLOSEBRACKET;
                strcpy(token.value, "]");
                break;
            case '{':
                token.type = JSON_OPENBRACE;
                strcpy(token.value, "{");
                break;
            case '}':
                token.type = JSON_CLOSEBRACE;
                strcpy(token.value, "}");
                break;
            case '"':
                token.type = JSON_STRING;
                char str[8192];
                i++;
                int s = 0;
                while (i < len && pipe[i] != '"')
                {
                    str[s++] = pipe[i++];
                }
                str[s] = '\0';
                i++;
                strcpy(token.value, str);
                break;
            default:
                if (is_whitespace(c))
                {
                    i++;
                }
                else if (is_digit(c))
                {
                    char num[8192];
                    int n = 0;
                    while (i < len && is_digit(pipe[i]))
                    {
                        num[n++] = pipe[i++];
                    }
                    num[n] = '\0';
                    token.type = JSON_NUMBER;
                    strcpy(token.value, num);
                }
                else if (c == 't' && (i + 3 < len))
                {
                    char val[] = { c, pipe[i + 1], pipe[i + 2], pipe[i + 3], '\0' };
                    if (strcmp(val, "true") == 0)
                    {
                        token.type = JSON_BOOLEAN;
                        strcpy(token.value, "true");
                        i += 4;
                    }
                }
                else if (c == 'f' && (i + 4 < len))
                {
                    char val[] = { c, pipe[i + 1], pipe[i + 2], pipe[i + 3], pipe[i + 4], '\0' };
                    if (strcmp(val, "false") == 0)
                    {
                        token.type = JSON_BOOLEAN;
                        strcpy(token.value, "false");
                        i += 5;
                    }
                }
                else if (c == 'n' && (i + 3 < len))
                {
                    char val[] = { c, pipe[i + 1], pipe[i + 2], pipe[i + 3], '\0' };
                    if (strcmp(val, "null") == 0)
                    {
                        token.type = JSON_NULL;
                        strcpy(token.value, "null");
                        i += 4;
                    }
                }
                else
                {
                    fprintf(stderr, "Unknown or invalid character at index %d. Aborting.\n", i);
                    return -1;
                }
                break;
        }
        tokens[t++] = token;
    }

    for (int q = 0; q < t; q++)
    {
        printf("%s\n", tokens[q].value);
    }
    return 0;
}

int main(int argc, char **argv)
{
    if (!isatty(fileno(stdin)))
    {
        int i = 0;
        char pipe[65536];

        while ((pipe[i++] = getchar()) != -1);
        pipe[i - 1] = '\0';

        fprintf(stdout, "piped content:\n%s\n", pipe);

        int result = json_lexer(pipe, i);
        if (result == -1)
        {
            return 1;
        }
    }
    else
    {
        printf("wtf r u doin\n");
    }
    return 0;
}
