#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "commands.h"

/* Store external command names */
char *external_commands[MAX_COMMAND];
/* Built-in commands supported by our shell */
char *builtins[] =
{
    "echo",
    "printf",
    "read",
    "cd",
    "pwd",
    "pushd",
    "popd",
    "dirs",
    "let",
    "eval",
    "set",
    "unset",
    "export",
    "declare",
    "typeset",
    "readonly",
    "getopts",
    "source",
    "exit",
    "exec",
    "shopt",
    "caller",
    "true",
    "type",
    "hash",
    "bind",
    "help",
    "jobs",
    "fg",
    "bg",
    "clear",
    NULL
};

/* Check whether a command is a built-in command */
int check_builtin(char *command)
{
    int i = 0;
    while (builtins[i] != NULL)
    {
        if (strcmp(command, builtins[i]) == 0)
            return 1;
        i++;
    }
    return 0;
}

/* Load external commands from external_command.txt */
void extract_external_commands(void)
{
    FILE *file;
    char line[100];
    int i = 0;

    file = fopen("external_command.txt", "r");
    if (file == NULL)
    {
        perror("external_command.txt");
        return;
    }

    while (fgets(line, sizeof(line), file) != NULL &&
           i < MAX_COMMAND - 1)
    {
        line[strcspn(line, "\n")] = '\0';
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
    fclose(file);
}

/* Identify the command type */
int check_cmd_type(char *command)
{
    int i = 0;
    if (command == NULL || command[0] == '\0')
        return NO_COMMAND;
    if (check_builtin(command))
        return BUILTIN_CMD;
    while (external_commands[i] != NULL)
    {
        if (strcmp(command, external_commands[i]) == 0)
            return EXTERNAL_CMD;
        i++;
    }
    return NO_COMMAND;
}
