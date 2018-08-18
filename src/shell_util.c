#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <signal.h>
#include <limits.h>
#include <readline/readline.h>
#include "builtin.h" 
#include "sig_handlers.h" 
#include "util.h"

#define INIT_BUFSIZE 128
#define DELIMITER " \t\r\n\a"

static char* get_nice_prompt(char* end_of_prompt) {

  char pwd[PATH_MAX+1];
  char *home;
  char *ret_prompt;
  
  // Pointer to the HOME string
  home = getenv("HOME");
  if (!home) {
    home = "";
  }  

  // Copies the path to pwd
  getcwd(pwd, PATH_MAX);
  char *offset = strstr(pwd, home);
  int pwd_len = strlen(pwd);
  int end_of_prompt_len = strlen(end_of_prompt);
  
  // Dunno if this works.
  if (!offset) {
    ret_prompt = malloc(pwd_len + end_of_prompt_len + 1);

    memcpy(ret_prompt, pwd, pwd_len);
    strcpy(ret_prompt + pwd_len, end_of_prompt);
    return ret_prompt;
  }
  // else
  int home_len = strlen(home);
  if (home_len > PATH_MAX) {
    exit(1);
  }

  // +2 = 1 (for NULL terminator) + 1 ('~' char)
  ret_prompt = malloc(pwd_len - home_len + end_of_prompt_len + 2);

  memcpy(ret_prompt, "~", 1);
  memcpy(ret_prompt + 1, offset + home_len, pwd_len - home_len);
  strcpy(ret_prompt + 1 + pwd_len - home_len, end_of_prompt);

  return ret_prompt;
}

char* shell_readline(char *append_prompt) {

  char* prompt = get_nice_prompt(append_prompt);
  char* gotLine = readline(prompt);
  // If readline encounters an EOF while reading the line, and the line is empty at that point, then (char *)NULL is returned
  // http://www.delorie.com/gnu/docs/readline/rlman_24.html
  free(prompt);
  nullchecker(gotLine, "", Nothing); // print nothing, just exit if NULL

  return gotLine;
}

char** shell_splitline(char *line) {
  int curr_bufsize = INIT_BUFSIZE;
  char **tokens = malloc(curr_bufsize * sizeof(char*));
  char *token;

  nullchecker(tokens, "Malloc failure\n", Msg);

  int idx = 0;
  token = strtok(line, DELIMITER);
  while (token) {
    tokens[idx] = token;
    idx++;

    if (idx >= curr_bufsize) {
      curr_bufsize += INIT_BUFSIZE;
      tokens = d_realloc(tokens, curr_bufsize * sizeof(char*));
    }
    token = strtok(NULL, DELIMITER);
  }
  tokens[idx] = NULL;
  return tokens;
}

int shell_system(char **args) {
  int status;
  pid_t w;
  pid_t child = fork();

  if (child == 0) {
    if (execvp(*args, args) < 0) {
      fprintf(stderr, "msg: Execution failed\n ");
    }
    exit(1);
  } else if (child < 0) {
    perror("msg");
  } else {
    // https://linux.die.net/man/2/waitpid example given at the bottom
    do {
      w = waitpid(child, &status, WUNTRACED);
      if (w == -1) {
        perror("waitpid");
        exit(1);
      }
    } while(!WIFEXITED(status) && !WIFSIGNALED(status));
  }
  return 1;
}

int shell_command(char **args) {
  if(!(*args)) {
    return 1;
  }
  // checks builtins first
  for(int i = 0; i < num_builtins(); i++) {
    if (strcmp(*args, builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }
  // Asks the system 
  return shell_system(args);
}

void shell_loop() {
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
    // We set the savesigs flag to nonzero (1 here), to save it, and make sure to restore it, when the signal handler is done - so we can get the signal again.
    if (sigsetjmp(env, 1) == 1) {
      printf("\n");
      status = 1;
      continue;
    }

    jmp_active = 1;
    
    line = shell_readline(" >>> ");
    args = shell_splitline(line);
    status = shell_command(args);

    free(line);
    free(args);
  } while(status);
}

