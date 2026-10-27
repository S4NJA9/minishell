#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commands.h"

char *external_commands[MAX_COMMAND];

void extract_external_commands(void)
{
    FILE *fp;
    char line[100];
    int i = 0;

    fp = fopen("external_command.txt", "r");

    if (fp == NULL)
    {
        perror("external_command.txt");
        return;
    }

    while (fgets(line, sizeof(line), fp) != NULL &&
           i < MAX_COMMAND - 1)
    {
        line[strcspn(line, "\r\n")] = '\0';

        if (line[0] == '\0')
            continue;

        external_commands[i] = malloc(strlen(line) + 1);

        if (external_commands[i] == NULL)
        {
            perror("malloc");
            break;
        }

        strcpy(external_commands[i], line);
        i++;
    }

    external_commands[i] = NULL;

    fclose(fp);
}

int check_cmd_type(char *command)
{
    int i;

    char *builtins[] = {
        "echo", "printf", "read", "cd", "pwd",
        "pushd", "popd", "dirs", "let", "eval",
        "set", "unset", "export", "declare", "typeset",
        "readonly", "getopts", "source", "exit", "exec",
        "shopt", "caller", "true", "type", "hash",
        "bind", "help", "clear", NULL
    };

    if (command == NULL || command[0] == '\0')
        return NO_COMMAND;

    for (i = 0; builtins[i] != NULL; i++)
    {
        if (strcmp(command, builtins[i]) == 0)
            return BUILTIN_CMD;
    }

    for (i = 0; external_commands[i] != NULL; i++)
    {
        if (strcmp(command, external_commands[i]) == 0)
            return EXTERNAL_CMD;
    }

    return NO_COMMAND;
}