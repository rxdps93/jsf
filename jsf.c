#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define JSON_WHITESPACE ((const char[]) { ' ', '\t', '\b', '\n', '\r' })
#define PIPE_SIZE 256
#define TOKEN_VALUE_SIZE 64
#define TOKENS_SIZE 128
#define STRING_SIZE 64
#define NUMBER_SIZE 8

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
    char value[TOKEN_VALUE_SIZE];
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
    token_t tokens[TOKENS_SIZE];
    int t = 0;
    for (int i = 0; i < len; i++)
    {
        char c = pipe[i];
        printf("beginning with index %d and char %c [%d]\n", i, c, c);
        token_t token;
        switch (c)
        {
            case ',':
                token.type = JSON_COMMA;
                strcpy(token.value, ",");
                printf("\tcomma token added\n");
                break;
            case ':':
                token.type = JSON_COLON;
                strcpy(token.value, ":");
                printf("\tcolon token added\n");
                break;
            case '[':
                token.type = JSON_OPENBRACKET;
                strcpy(token.value, "[");
                printf("\topen bracket token added\n");
                break;
            case ']':
                token.type = JSON_CLOSEBRACKET;
                strcpy(token.value, "]");
                printf("\tclose bracket token added\n");
                break;
            case '{':
                token.type = JSON_OPENBRACE;
                strcpy(token.value, "{");
                printf("\topen brace token added\n");
                break;
            case '}':
                token.type = JSON_CLOSEBRACE;
                strcpy(token.value, "}");
                printf("\tclose brace token added\n");
                break;
            case '\"':
                token.type = JSON_STRING;
                char str[STRING_SIZE];
                i++;
                int s = 0;
                while (i < len && pipe[i] != '\"')
                {
                    str[s++] = pipe[i++];
                }
                str[s] = '\0';
                strcpy(token.value, str);
                printf("\tstring token added: %s\n", str);
                break;
            default:
                if (is_whitespace(c))
                {
                    printf("\tfound whitespace\n");
                    continue;
                }
                else if (is_digit(c))
                {
                    char num[NUMBER_SIZE];
                    int n = 0;
                    while (i < len && is_digit(pipe[i]))
                    {
                        num[n++] = pipe[i++];
                    }
                    num[n] = '\0';
                    token.type = JSON_NUMBER;
                    strcpy(token.value, num);
                    printf("\tnumber token added: %s\n", num);
                }
                else if (c == 't' && (i + 3 < len))
                {
                    char val[] = { c, pipe[i + 1], pipe[i + 2], pipe[i + 3], '\0' };
                    if (strcmp(val, "true") == 0)
                    {
                        token.type = JSON_BOOLEAN;
                        strcpy(token.value, "true");
                        i += 4;
                        printf("\tboolean token added: %s\n", val);
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
                        printf("\tboolean token added: %s\n", val);
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
                        printf("\tnull token added: %s\n", val);
                    }
                }
                else
                {
                    fprintf(stderr, "Unknown or invalid character %c [%d] found at index %d. Aborting.\n", c, c, i);
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
        char pipe[PIPE_SIZE];

        while ((pipe[i++] = getchar()) != -1);
        pipe[i - 1] = '\0';

        fprintf(stdout, "piped content:\n%s\nlen: %d\n", pipe, i - 1);

        int result = json_lexer(pipe, i - 1);
        if (result == -1)
        {
            return 1;
        }
    }
    else
    {
        printf("Only piped/stdin is supported so far.\n");
    }
    return 0;
}
