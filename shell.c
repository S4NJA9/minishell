
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>

#include "execute.h"
#include "shell.h"
#include "input.h"
#include "commands.h"

char prompt_string[50] = "minishell$:";

/* Display the Matrix-style prompt */
void display_prompt(int last_status)
{
    char cwd[1024];
    char time_string[20];
    time_t now;
    struct tm *time_info;

    if (getcwd(cwd, sizeof(cwd)) == NULL)
        strcpy(cwd, "?");

    time(&now);
    time_info = localtime(&now);

    if (time_info != NULL)
        strftime(time_string, sizeof(time_string),
                 "%H:%M:%S", time_info);
    else
        strcpy(time_string, "--:--:--");

    /* Display previous command status */
    if (last_status == 0)
        printf("\033[1;32m[✓ %d] \033[0m", last_status);
    else
        printf("\033[1;31m[✗ %d] \033[0m", last_status);

    /* Display time, directory and custom prompt */
    printf("\033[1;32m[%s] \033[0m", time_string);
    printf("\033[0;32m%s\033[0m ", cwd);
    printf("\033[1;32m%s \033[0m", prompt_string);

    fflush(stdout);
}

/* Start the shell */
void start_shell(void)
{
    char input_string[100];
    char *args[20];
    int last_status = 0;

    system("clear");

    printf("\033[1;32m");
    printf("========================================\n");
    printf("          MATRIX MINI SHELL             \n");
    printf("========================================\n");
    printf("  Welcome to your custom C shell!\n");
    printf("========================================\n");
    printf("\033[0m");

    extract_external_commands();

    while (1)
    {
        display_prompt(last_status);

        if (fgets(input_string, sizeof(input_string), stdin) == NULL)
            break;

        input_string[strcspn(input_string, "\n")] = '\0';

        /* Handle PS1 prompt customization */
        if (strstr(input_string, "PS1=") != NULL)
        {
            int i = 0;
            int space_found = 0;

            if (strncmp(input_string, "PS1=", 4) != 0)
            {
                printf("Invalid input\n");
                continue;
            }

            while (input_string[i] != '\0')
            {
                if (isspace((unsigned char)input_string[i]))
                {
                    space_found = 1;
                    break;
                }
                i++;
            }

            if (space_found == 1)
            {
                printf("Invalid input\n");
            }
            else if (input_string[4] == '\0')
            {
                printf("Invalid input: prompt is empty\n");
            }
            else if (strlen(input_string + 4) <
                     sizeof(prompt_string))
            {
                strcpy(prompt_string, input_string + 4);
            }
            else
            {
                printf("Prompt is too long\n");
            }

            continue;
        }

        split_input(input_string, args);
        last_status = execute_command(args);
    }
}
