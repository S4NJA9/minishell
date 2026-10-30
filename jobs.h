
#ifndef JOBS_H
#define JOBS_H

#include <sys/types.h>

#define JOB_RUNNING 1
#define JOB_STOPPED 2

typedef struct Job
{
    int job_id;
    pid_t pid;
    char command[100];
    int status;

    struct Job *next;

} Job;

extern Job *job_list;


void remove_job(pid_t pid);
void display_jobs(void);
Job *find_job(int job_id);
void update_job_status(pid_t pid, int status);
Job *find_job_by_pid(pid_t pid);
Job *add_job(pid_t pid, const char *command, int status);
#endif
