#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define JSON_WHITESPACE ((const char[]) { ' ', '\t', '\b', '\n', '\r' })
#define JSON_WS_SPACE       0
#define JSON_WS_TAB         1
#define JSON_WS_BACKSPACE   2
#define JSON_WS_NEWLINE     3
#define JSON_WS_RETURN      4

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

int is_num_char(char c)
{
    switch (c)
    {
        case '.': // for decimals
        case 'e': // scientific notation
        case 'E': // scientific notation
        case '+': // scientific notation
            return 1;
        default:
            return isdigit(c);
    }
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

int json_lexer(char *pipe, int len, token_t *tokens)
{
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
                else if (isdigit(c))
                {
                    char num[NUMBER_SIZE];
                    int n = 0;
                    while (i < len && is_num_char(pipe[i]))
                    {
                        num[n++] = pipe[i++];
                    }
                    i--;
                    num[n] = '\0';

                    if (num[0] == '0')
                    {
                        fprintf(stderr, "Error: Leading Zero: %s\n", num);
                        return -1;
                    }

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
                        i += 3;
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
                        i += 4;
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
                        i += 3;
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
    return t;
}

int format_from_tokens(token_t *tokens, int token_count, char *json)
{
    int j = 0;
    int v = 0;
    for (int i = 0; i < token_count; i++)
    {
        token_t token = tokens[i];
        switch (token.type)
        {
            case JSON_COMMA:
                json[j++] = ',';
                break;
            case JSON_COLON:
                json[j++] = ':';
                break;
            case JSON_OPENBRACKET:
                json[j++] = '[';
                break;
            case JSON_CLOSEBRACKET:
                json[j++] = ']';
                break;
            case JSON_OPENBRACE:
                json[j++] = '{';
                break;
            case JSON_CLOSEBRACE:
                json[j++] = '}';
                break;
            /*
            case JSON_QUOTE:
                json[j++] = "\"";
                break;
            */
            case JSON_STRING:
                v = 0;
                json[j++] = '\"';
                while (token.value[v] != '\0')
                {
                    json[j++] = token.value[v++];
                }
                json[j++] = '\"';
                break;
            case JSON_NUMBER:
                v = 0;
                while (token.value[v] != '\0')
                {
                    json[j++] = token.value[v++];
                }
                break;
            case JSON_BOOLEAN:
                v = 0;
                while (token.value[v] != '\0')
                {
                    json[j++] = token.value[v++];
                }
                break;
            case JSON_NULL:
                v = 0;
                while (token.value[v] != '\0')
                {
                    json[j++] = token.value[v++];
                }
                break;
            default:
                fprintf(stderr, "Unexpected token at index %d with value %s\n", i, token.value);
                return -1;
        }
    }
    json[j] = '\0';
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

        token_t tokens[TOKENS_SIZE];
        int result = json_lexer(pipe, i - 1, tokens);
        if (result == -1)
        {
            return 1;
        }

        printf("There are a total of %d tokens\n", result);
        char json[PIPE_SIZE];
        result = format_from_tokens(tokens, result, json);

        if (result == -1)
        {
            return 1;
        }

        printf("%s\n", json);
    }
    else
    {
        printf("Only piped/stdin is supported so far.\n");
    }
    return 0;
}
