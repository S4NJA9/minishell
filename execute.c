
#include "execute.h"
#include "commands.h"
#include "input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/* Decide how to execute the command */
int execute_command(char **args)
{
    int type;
    int status;
    pid_t pid;

    if (args == NULL || args[0] == NULL)
        return 0;

    type = check_cmd_type(args[0]);

    if (type == NO_COMMAND)
    {
        printf("No command found: %s\n", args[0]);
        return 127;
    }
    else if (type == BUILTIN_CMD)
    {
        return execute_internal_cmd(args);
    }
    else if (type == EXTERNAL_CMD)
    {
        pid = fork();

        if (pid < 0)
        {
            perror("fork");
            return 1;
        }
        else if (pid == 0)
        {
            /* Child process */
            execute_external_cmd(args);

            /* Reached only if execution returns */
            _exit(127);
        }
        else
        {
            /* Parent waits for the child */
            if (waitpid(pid, &status, 0) == -1)
            {
                perror("waitpid");
                return 1;
            }

            if (WIFEXITED(status))
                return WEXITSTATUS(status);

            if (WIFSIGNALED(status))
                return 128 + WTERMSIG(status);

            return 1;
        }
    }

    return 1;
}

/* Execute built-in commands */
int execute_internal_cmd(char **args)
{
    char cwd[1024];
    int i;

    if (strcmp(args[0], "cd") == 0)
    {
        if (args[1] == NULL)
        {
            char *home = getenv("HOME");

            if (home == NULL)
            {
                fprintf(stderr, "cd: HOME not set\n");
                return 1;
            }

            if (chdir(home) == -1)
            {
                perror("cd");
                return 1;
            }
        }
        else if (chdir(args[1]) == -1)
        {
            perror("cd");
            return 1;
        }

        return 0;
    }
    else if (strcmp(args[0], "pwd") == 0)
    {
        if (getcwd(cwd, sizeof(cwd)) != NULL)
        {
            printf("%s\n", cwd);
            return 0;
        }

        perror("pwd");
        return 1;
    }
    else if (strcmp(args[0], "exit") == 0)
    {
        exit(0);
    }
    else if (strcmp(args[0], "echo") == 0)
    {
        for (i = 1; args[i] != NULL; i++)
        {
            if (i > 1)
                printf(" ");

            printf("%s", args[i]);
        }

        printf("\n");
        return 0;
    }

    return 1;
}

/* Execute external commands and pipelines */
void execute_external_cmd(char **args)
{
    char *pipeline[MAX_ARGS][MAX_ARGS];
    pid_t pids[MAX_ARGS];

    int command_count = 1;
    int arg_count = 0;
    int has_pipe = 0;
    int i, j;

    /* Check whether the command contains a pipe */
    for (i = 0; args[i] != NULL; i++)
    {
        if (strcmp(args[i], "|") == 0)
        {
            has_pipe = 1;
            command_count++;
        }
    }

    /* No pipe: execute the external command directly */
    if (!has_pipe)
    {
        execvp(args[0], args);
        perror(args[0]);
        _exit(127);
    }

    /* Initialize the pipeline array */
    for (i = 0; i < MAX_ARGS; i++)
    {
        for (j = 0; j < MAX_ARGS; j++)
        {
            pipeline[i][j] = NULL;
        }
    }

    command_count = 0;
    arg_count = 0;

    /* Divide arguments at each pipe symbol */
    for (i = 0; args[i] != NULL; i++)
    {
        if (strcmp(args[i], "|") == 0)
        {
            if (arg_count == 0 || command_count >= MAX_ARGS - 1)
            {
                fprintf(stderr, "Invalid pipe usage\n");
                _exit(1);
            }

            pipeline[command_count][arg_count] = NULL;
            command_count++;
            arg_count = 0;
        }
        else
        {
            if (arg_count >= MAX_ARGS - 1)
            {
                fprintf(stderr, "Too many arguments\n");
                _exit(1);
            }

            pipeline[command_count][arg_count] = args[i];
            arg_count++;
        }
    }

    if (arg_count == 0)
    {
        fprintf(stderr, "Invalid pipe usage\n");
        _exit(1);
    }

    pipeline[command_count][arg_count] = NULL;
    command_count++;

    /* Create and connect each process in the pipeline */
    int previous_read = -1;

    for (i = 0; i < command_count; i++)
    {
        int fd[2] = {-1, -1};

        if (i < command_count - 1)
        {
            if (pipe(fd) == -1)
            {
                perror("pipe");
                _exit(1);
            }
        }

        pids[i] = fork();

        if (pids[i] < 0)
        {
            perror("fork");
            _exit(1);
        }

        if (pids[i] == 0)
        {
            /* Read from the previous command */
            if (previous_read != -1)
            {
                dup2(previous_read, STDIN_FILENO);
                close(previous_read);
            }

            /* Write to the next command */
            if (i < command_count - 1)
            {
                close(fd[0]);
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]);
            }

            /* Run a built-in inside the pipeline child */
            if (check_cmd_type(pipeline[i][0]) == BUILTIN_CMD)
            {
                int builtin_status = execute_internal_cmd(pipeline[i]);
                fflush(NULL);
                _exit(builtin_status);
            }

            execvp(pipeline[i][0], pipeline[i]);

            perror(pipeline[i][0]);
            _exit(127);
        }

        /* Parent closes descriptors it no longer needs */
        if (previous_read != -1)
            close(previous_read);

        if (i < command_count - 1)
        {
            close(fd[1]);
            previous_read = fd[0];
        }
    }

    if (previous_read != -1)
        close(previous_read);

    /* Wait for every process in the pipeline */
    for (i = 0; i < command_count; i++)
    {
        waitpid(pids[i], NULL, 0);
    }
}
