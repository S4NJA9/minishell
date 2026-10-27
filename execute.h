
#ifndef EXECUTE_H
#define EXECUTE_H

int execute_command(char **args);
int execute_internal_cmd(char **args);
void execute_external_cmd(char **args);

#endif
