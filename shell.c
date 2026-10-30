
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#include "shell.h"
#include "input.h"
#include "commands.h"
#include "execute.h"
#include "jobs.h"

/* Default shell prompt */
char prompt_string[50]="msh>";

/* Flag used to check child process changes */
volatile sig_atomic_t child_changed=0;

/* SIGCHLD signal handler */
void child_signal_handler(int signum)
{
    (void)signum;
    child_changed=1;
}

/* Initialize shell signal handling */
void init_shell(void)
{
    signal(SIGINT,SIG_IGN);
    signal(SIGTSTP,SIG_IGN);
    signal(SIGCHLD,child_signal_handler);
}

/* Display shell prompt */
void display_prompt(void)
{
    printf("%s",prompt_string);
    fflush(stdout);
}

/* Check and remove completed background processes */
void check_background_jobs(void)
{
    int status;
    pid_t pid;

    if(!child_changed)
        return;

    child_changed=0;

    while(1)
    {
        pid=waitpid(-1,&status,WNOHANG);

        if(pid>0)
        {
            if(WIFEXITED(status))
            {
                printf("\n[Process %d completed with exit status %d]\n",
                       pid,WEXITSTATUS(status));
            }
            else if(WIFSIGNALED(status))
            {
                printf("\n[Process %d terminated by signal %d]\n",
                       pid,WTERMSIG(status));
            }

            remove_job(pid);
        }
        else if(pid==-1 && errno==EINTR)
        {
            continue;
        }
        else
        {
            break;
        }
    }
}

/* Get the latest job from the linked list */
Job *get_latest_job(void)
{
    Job *temp=job_list;

    if(temp==NULL)
        return NULL;

    while(temp->next!=NULL)
        temp=temp->next;

    return temp;
}

/* Get job using job ID or process ID */
Job *get_selected_job(char *argument)
{
    if(argument==NULL)
        return get_latest_job();

    if(argument[0]=='%')
        return find_job(atoi(argument+1));

    return find_job_by_pid((pid_t)atoi(argument));
}

/* Continue a stopped job in background */
void background_job(char **args)
{
    Job *job;

    job=get_selected_job(args[1]);

    if(job==NULL)
    {
        printf("Job not found\n");
        return;
    }

    if(kill(job->pid,SIGCONT)==-1)
    {
        perror("bg");
        return;
    }

    job->status=JOB_RUNNING;

    printf("[%d] %s &\n",job->job_id,job->command);
}

/* Bring a background job to foreground */
void foreground_job(char **args)
{
    int status;
    Job *job;
    pid_t pid;

    job=get_selected_job(args[1]);

    if(job==NULL)
    {
        printf("Job not found\n");
        return;
    }

    pid=job->pid;

    if(kill(pid,SIGCONT)==-1)
    {
        perror("fg");
        return;
    }

    job->status=JOB_RUNNING;

    printf("%s\n",job->command);

    while(1)
    {
        if(waitpid(pid,&status,WUNTRACED)!=-1)
            break;

        if(errno==EINTR)
            continue;

        perror("waitpid");
        return;
    }

    if(WIFSTOPPED(status))
    {
        job->status=JOB_STOPPED;

        printf("[%d] Stopped %s\n",job->job_id,job->command);
    }
    else
    {
        remove_job(pid);
    }
}

/* Execute an external command in background */
int execute_background(char **args)
{
    pid_t pid;
    int i;
    Job *new_job;

    /* Remove the ampersand symbol */
    for(i=0;args[i]!=NULL;i++)
    {
        if(strcmp(args[i],"&")==0)
        {
            args[i]=NULL;
            break;
        }
    }

    if(args[0]==NULL)
        return 0;

    /* Check whether command is external */
    if(check_cmd_type(args[0])!=EXTERNAL_CMD)
    {
        printf("Background execution currently supports external commands only\n");
        return 1;
    }

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
        signal(SIGCHLD,SIG_DFL);

        execvp(args[0],args);

        perror(args[0]);
        _exit(127);
    }

    /* Store background process in job list */
    new_job=add_job(pid,args[0],JOB_RUNNING);

    if(new_job!=NULL)
        printf("[%d] %d\n",new_job->job_id,pid);
    else
        printf("[%d] Started (job tracking failed)\n",pid);

    return 0;
}

/* Main shell execution loop */
void start_shell(void)
{
    char input_string[100];
    char *args[MAX_ARGS];
    int last_status=0;
    int i;
    int background;

    init_shell();

    extract_external_commands();

    while(1)
    {
        /* Check completed background jobs */
        check_background_jobs();

        display_prompt();

        /* Read user input */
        if(fgets(input_string,sizeof(input_string),stdin)==NULL)
        {
            printf("\n");
            break;
        }

        input_string[strcspn(input_string,"\n")]='\0';

        /* Ignore empty input */
        if(input_string[0]=='\0')
            continue;

        /* Handle PS1 prompt assignment */
        if(strncmp(input_string,"PS1=",4)==0)
        {
            int valid=1;

            if(input_string[4]=='\0')
                valid=0;

            for(i=0;input_string[i]!='\0';i++)
            {
                if(isspace((unsigned char)input_string[i]))
                {
                    valid=0;
                    break;
                }
            }

            if(valid && strlen(input_string+4)<sizeof(prompt_string))
            {
                strcpy(prompt_string,input_string+4);
                last_status=0;
            }
            else
            {
                printf("Invalid PS1 assignment\n");
                last_status=1;
            }

            continue;
        }

        /* Split input into arguments */
        split_input(input_string,args);

        if(args[0]==NULL)
            continue;

        /* Check for background execution */
        background=0;

        for(i=0;args[i]!=NULL;i++)
        {
            if(strcmp(args[i],"&")==0)
            {
                background=1;
                break;
            }
        }

        if(background)
        {
            last_status=execute_background(args);
            continue;
        }

        /* Handle jobs command */
        if(strcmp(args[0],"jobs")==0)
        {
            display_jobs();
            last_status=0;
            continue;
        }

        /* Handle bg command */
        if(strcmp(args[0],"bg")==0)
        {
            background_job(args);
            last_status=0;
            continue;
        }

        /* Handle fg command */
        if(strcmp(args[0],"fg")==0)
        {
            foreground_job(args);
            last_status=0;
            continue;
        }

        /* Execute normal command */
        last_status=execute_command(args,last_status);
    }
}
