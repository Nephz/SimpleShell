#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "shell_util.h" 
#include "sig_handlers.h"


int main(int argc, char **argv) {
  char *line;
  char **args;
  int status;

  struct sigaction sig;
  sig.sa_handler = SIGINT_handler;
  sigemptyset(&sig.sa_mask);
  // system calls will be restarted after the signalhandler has finished its execution
  sig.sa_flags = SA_RESTART;
  sigaction(SIGINT, &sig, NULL);

  do {
    if (sigsetjmp(env, 1) == 1) {
      printf("\n")
      continue;
    }

    jump_active = 1;

    printf("sh> ");
    line = shell_readline();
    args = shell_splitline(line);
    status = shell_command(args);

    free(line);
    free(args);
  } while(status);

  return 0;
}

setup