#ifndef COMMANDS_H
#define COMMANDS_H

/* Maximum number of external commands */
#define MAX_COMMAND 200

/* Command types */
#define NO_COMMAND 0
#define BUILTIN_CMD 1
#define EXTERNAL_CMD 2

/* External command list */
extern char *external_commands[MAX_COMMAND];

/* Function declarations */
void extract_external_commands(void);
int check_cmd_type(char *command);
int check_builtin(char *command);

#endif
