
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#include "execute.h"
#include "commands.h"
#include "input.h"
#include "jobs.h"

/* Store shell process ID and path */
static pid_t shell_pid=0;
static char shell_path[1024];

/* Execute built-in commands */
int execute_internal_cmd(char **args,int last_status)
{
    int i;

    /* echo command */
    if(strcmp(args[0],"echo")==0)
    {
        for(i=1;args[i]!=NULL;i++)
        {
            if(strcmp(args[i],"$?")==0)
                printf("%d",last_status);
            else if(strcmp(args[i],"$$")==0)
                printf("%d",shell_pid);
            else if(strcmp(args[i],"$SHELL")==0)
                printf("%s",shell_path);
            else
                printf("%s",args[i]);

            if(args[i+1]!=NULL)
                printf(" ");
        }

        printf("\n");
    }

    /* pwd command */
    else if(strcmp(args[0],"pwd")==0)
    {
        char cwd[1024];

        if(getcwd(cwd,sizeof(cwd))==NULL)
        {
            perror("pwd");
            return 1;
        }

        printf("%s\n",cwd);
    }

    /* cd command */
    else if(strcmp(args[0],"cd")==0)
    {
        char *path;

        if(args[1]==NULL)
            path=getenv("HOME");
        else
            path=args[1];

        if(path==NULL || chdir(path)==-1)
        {
            perror("cd");
            return 1;
        }
    }

    /* clear command */
    else if(strcmp(args[0],"clear")==0)
    {
        system("clear");
    }

    /* jobs command */
    else if(strcmp(args[0],"jobs")==0)
    {
        display_jobs();
    }

    /* exit command */
    else if(strcmp(args[0],"exit")==0)
    {
        exit(0);
    }

    else
    {
        printf("Unsupported built-in command: %s\n",args[0]);
        return 1;
    }

    return 0;
}

/* Execute external commands */
void execute_external_cmd(char **args)
{
    execvp(args[0],args);

    perror(args[0]);
    _exit(127);
}

/* Execute multiple commands connected using pipes */
static int execute_pipeline(char **args,int last_status)
{
    char ***commands;
    pid_t *pids;
    int command_count=1;
    int current_command=0;
    int arg_count=0;
    int i;
    int pipe_fd[2];
    int previous_read=-1;
    pid_t pid;
    int status;
    int final_status=0;

    /* Count pipeline commands */
    for(i=0;args[i]!=NULL;i++)
    {
        if(strcmp(args[i],"|")==0)
            command_count++;
    }

    /* Count arguments before changing the array */
    while(args[arg_count]!=NULL)
        arg_count++;

    /* Allocate memory for commands and process IDs */
    commands=malloc(command_count*sizeof(char **));
    pids=malloc(command_count*sizeof(pid_t));

    if(commands==NULL || pids==NULL)
    {
        perror("malloc");
        free(commands);
        free(pids);
        return 1;
    }

    /* Divide input into separate commands */
    commands[0]=args;

    for(i=0;i<arg_count;i++)
    {
        if(strcmp(args[i],"|")==0)
        {
            args[i]=NULL;
            current_command++;
            commands[current_command]=&args[i+1];
        }
    }

    /* Create pipes and child processes */
    for(i=0;i<command_count;i++)
    {
        if(i<command_count-1)
        {
            if(pipe(pipe_fd)==-1)
            {
                perror("pipe");

                if(previous_read!=-1)
                    close(previous_read);

                for(int j=0;j<i;j++)
                    waitpid(pids[j],NULL,0);

                free(commands);
                free(pids);
                return 1;
            }
        }

        pid=fork();

        if(pid==-1)
        {
            perror("fork");

            if(i<command_count-1)
            {
                close(pipe_fd[0]);
                close(pipe_fd[1]);
            }

            if(previous_read!=-1)
                close(previous_read);

            for(int j=0;j<i;j++)
                waitpid(pids[j],NULL,0);

            free(commands);
            free(pids);
            return 1;
        }

        if(pid==0)
        {
            signal(SIGINT,SIG_DFL);
            signal(SIGTSTP,SIG_DFL);

            /* Connect previous pipe to input */
            if(previous_read!=-1)
            {
                dup2(previous_read,STDIN_FILENO);
                close(previous_read);
            }

            /* Connect output to next pipe */
            if(i<command_count-1)
            {
                close(pipe_fd[0]);
                dup2(pipe_fd[1],STDOUT_FILENO);
                close(pipe_fd[1]);
            }

            if(commands[i][0]==NULL)
                _exit(0);

            /* Execute built-in command in child */
            if(check_cmd_type(commands[i][0])==BUILTIN_CMD)
            {
                execute_internal_cmd(commands[i],last_status);
                _exit(0);
            }

            execute_external_cmd(commands[i]);
        }

        pids[i]=pid;

        /* Close unused pipe ends in parent */
        if(previous_read!=-1)
            close(previous_read);

        if(i<command_count-1)
        {
            close(pipe_fd[1]);
            previous_read=pipe_fd[0];
        }
    }

    /* Wait for all pipeline processes */
    for(i=0;i<command_count;i++)
    {
        if(waitpid(pids[i],&status,0)==-1)
        {
            perror("waitpid");
            final_status=1;
        }
        else if(i==command_count-1)
        {
            if(WIFEXITED(status))
                final_status=WEXITSTATUS(status);
            else if(WIFSIGNALED(status))
                final_status=128+WTERMSIG(status);
        }
    }

    free(commands);
    free(pids);

    return final_status;
}

/* Decide how to execute the given command */
int execute_command(char **args,int last_status)
{
    int command_type;
    int i;

    if(args==NULL || args[0]==NULL)
        return 0;

    /* Get shell PID and executable path */
    if(shell_pid==0)
    {
        ssize_t length;

        shell_pid=getpid();

        length=readlink("/proc/self/exe",shell_path,sizeof(shell_path)-1);

        if(length!=-1)
            shell_path[length]='\0';
        else
            strcpy(shell_path,"minishell");
    }

    /* Check whether command contains a pipe */
    for(i=0;args[i]!=NULL;i++)
    {
        if(strcmp(args[i],"|")==0)
            return execute_pipeline(args,last_status);
    }

    command_type=check_cmd_type(args[0]);

    /* Execute built-in command */
    if(command_type==BUILTIN_CMD)
        return execute_internal_cmd(args,last_status);

    /* Execute external command */
    if(command_type==EXTERNAL_CMD)
    {
        pid_t pid;
        int status;

        pid=fork();

        if(pid==-1)
        {
            perror("fork");
            return 1;
        }

        if(pid==0)
        {
            signal(SIGINT,SIG_DFL);
            signal(SIGTSTP,SIG_DFL);

            execute_external_cmd(args);
        }

        /* Wait for child process */
        if(waitpid(pid,&status,WUNTRACED)==-1)
        {
            perror("waitpid");
            return 1;
        }

        /* Handle stopped process */
        if(WIFSTOPPED(status))
        {
            add_job(pid,args[0],JOB_STOPPED);

            printf("\n[%d] Stopped %s\n",pid,args[0]);

            return 128+WSTOPSIG(status);
        }

        /* Return command exit status */
        if(WIFEXITED(status))
            return WEXITSTATUS(status);

        if(WIFSIGNALED(status))
            return 128+WTERMSIG(status);

        return 1;
    }

    /* Command not found */
    printf("Command not found: %s\n",args[0]);

    return 127;
}
