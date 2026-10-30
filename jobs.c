
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jobs.h"

/* Head of the job linked list */
Job *job_list=NULL;

/* Generate unique job IDs */
static int next_job_id=1;

/* Add a new background or stopped job */
Job *add_job(pid_t pid,const char *command,int status)
{
    Job *new_job;
    Job *temp;

    /* Allocate memory for new job */
    new_job=malloc(sizeof(Job));

    if(new_job==NULL)
    {
        perror("malloc");
        return NULL;
    }

    /* Store job information */
    new_job->job_id=next_job_id++;
    new_job->pid=pid;
    new_job->status=status;

    strncpy(new_job->command,command,sizeof(new_job->command)-1);
    new_job->command[sizeof(new_job->command)-1]='\0';

    new_job->next=NULL;

    /* Insert job into linked list */
    if(job_list==NULL)
    {
        job_list=new_job;
    }
    else
    {
        temp=job_list;

        while(temp->next!=NULL)
            temp=temp->next;

        temp->next=new_job;
    }

    return new_job;
}

/* Remove a job using its process ID */
void remove_job(pid_t pid)
{
    Job *temp=job_list;
    Job *prev=NULL;

    while(temp!=NULL)
    {
        if(temp->pid==pid)
        {
            if(prev==NULL)
                job_list=temp->next;
            else
                prev->next=temp->next;

            free(temp);
            return;
        }

        prev=temp;
        temp=temp->next;
    }
}

/* Display all available jobs */
void display_jobs(void)
{
    Job *temp=job_list;

    if(temp==NULL)
    {
        printf("No background jobs\n");
        return;
    }

    while(temp!=NULL)
    {
        printf("[%d] %s %d %s\n",
               temp->job_id,
               temp->status==JOB_STOPPED?"Stopped":"Running",
               temp->pid,
               temp->command);

        temp=temp->next;
    }
}

/* Find a job using its job ID */
Job *find_job(int job_id)
{
    Job *temp=job_list;

    while(temp!=NULL)
    {
        if(temp->job_id==job_id)
            return temp;

        temp=temp->next;
    }

    return NULL;
}

/* Update the status of a job */
void update_job_status(pid_t pid,int status)
{
    Job *temp=job_list;

    while(temp!=NULL)
    {
        if(temp->pid==pid)
        {
            temp->status=status;
            return;
        }

        temp=temp->next;
    }
}

/* Find a job using its process ID */
Job *find_job_by_pid(pid_t pid)
{
    Job *temp=job_list;

    while(temp!=NULL)
    {
        if(temp->pid==pid)
            return temp;

        temp=temp->next;
    }

    return NULL;
}
