#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#include "shell_util.h" 
#include "sig_handlers.h"

int main(int argc, char **argv) {
  char *line;
  char **args;
  int status;
  int i = 0;

  struct sigaction sig;
  sig.sa_handler = SIGINT_handler;
  sigemptyset(&sig.sa_mask);
  // system calls will be restarted after the signalhandler has finished its execution
  sig.sa_flags = SA_RESTART;
  sigaction(SIGINT, &sig, NULL);

  do {
    // We set the savesigs flag to nonzero (1 here), to save it, and make sure to restore it, when the signal handler is done - so we can get the signal again.
    if (sigsetjmp(env, 1) == 1) {
      printf("\n");
      status = 1;
      continue;
    }

    jmp_active = 1;
    
    line = shell_readline("> ");
    args = shell_splitline(line);
    status = shell_command(args);

    free(line);
    free(args);
  } while(status);

  return 0;
}