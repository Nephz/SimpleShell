#include <stdio.h>
#include <stdlib.h>
#include "shell_util.h" 

int main(int argc, char **argv) {

  char *line;
  char **args;
  int status;

  do {
    printf("unixsh> ");
    line = shell_readline();
    args = shell_splitline(line);
    status = shell_command(args);

    free(line);
    free(args);
  } while(status);

  return 0;
}