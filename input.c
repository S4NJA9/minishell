
#include <string.h>
#include "input.h"

/* Split input into command arguments */
void split_input(char *input_string,char **args)
{
    int i=0;
    char *p=input_string;
    static char pipe_symbol[]="|";
    static char background_symbol[]="&";

    while(*p!='\0' && i<MAX_ARGS-1)
    {
        /* Skip spaces and tabs */
        while(*p==' ' || *p=='\t')
            p++;

        if(*p=='\0')
            break;

        /* Handle pipe symbol */
        if(*p=='|')
        {
            args[i++]=pipe_symbol;
            p++;
            continue;
        }

        /* Handle background symbol */
        if(*p=='&')
        {
            args[i++]=background_symbol;
            p++;
            continue;
        }

        /* Store the beginning of an argument */
        args[i++]=p;

        /* Move until a delimiter is found */
        while(*p!='\0' && *p!=' ' && *p!='\t' && *p!='|' && *p!='&')
            p++;

        /* Separate the argument from the next token */
        if(*p=='|')
        {
            *p='\0';
            p++;

            if(i<MAX_ARGS-1)
                args[i++]=pipe_symbol;
        }
        else if(*p=='&')
        {
            *p='\0';
            p++;

            if(i<MAX_ARGS-1)
                args[i++]=background_symbol;
        }
        else if(*p!='\0')
        {
            *p='\0';
            p++;
        }
    }

    /* Mark the end of arguments */
    args[i]=NULL;
}
