#ifndef EXECUTE_H
#define EXECUTE_H

int execute_command(char **args, int last_status);
int execute_internal_cmd(char **args, int last_status);
void execute_external_cmd(char **args);
#endif
