#ifndef COMMANDS_H
#define COMMANDS_H

#define MAX_COMMAND 200

#define NO_COMMAND 0
#define BUILTIN_CMD 1
#define EXTERNAL_CMD 2

extern char *external_commands[MAX_COMMAND];

void extract_external_commands(void);
int check_cmd_type(char *command);

#endif