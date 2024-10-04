#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define JSON_WHITESPACE ((const char[]) { ' ', '\t', '\b', '\n', '\r' })

typedef enum
{
    JSON_COMMA,
    JSON_COLON,
    JSON_OPENBRACKET,
    JSON_CLOSEBRACKET,
    JSON_OPENBRACE,
    JSON_CLOSEBRACE,
    JSON_QUOTE,
    JSON_STRING,
} token_type_t;

typedef struct token
{
    token_type_t type;
    char value[8192];
} token_t;

void json_lexer(char *pipe, int len)
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
                break;
        }
        tokens[t++] = token;
    }
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

        json_lexer(pipe, i);
    }
    else
    {
        printf("wtf r u doin\n");
    }
    return 0;
}
