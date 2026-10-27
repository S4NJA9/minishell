#include <stddef.h>
#include "input.h"

void split_input(char *input_string, char **args)
{
    int i = 0;
    char *p = input_string;
    static char pipe_symbol[] = "|";

    while (*p != '\0' && i < MAX_ARGS - 1)
    {
        while (*p == ' ' || *p == '\t')
            p++;

        if (*p == '\0')
            break;

        if (*p == '|')
        {
            args[i] = pipe_symbol;
            i++;
            p++;
            continue;
        }

        args[i] = p;
        i++;

        while (*p != '\0' && *p != ' ' &&
               *p != '\t' && *p != '|')
        {
            p++;
        }

        if (*p == '|')
        {
            *p = '\0';
            p++;

            if (i < MAX_ARGS - 1)
            {
                args[i] = pipe_symbol;
                i++;
            }
        }
        else if (*p != '\0')
        {
            *p = '\0';
            p++;
        }
    }

    args[i] = NULL;
}
