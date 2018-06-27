#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <setjmp.h>
#include <signal.h>
#include <readline/readline.h>
#include "builtin.h" 
#include "sig_handlers.h" 

#define INIT_BUFSIZE 128
#define DELIMITER " \t\r\n\a"

static void* d_realloc(void* buf, size_t size) {
  void* newbuf = realloc(buf, size);
  if (!newbuf) {
    free(buf);
  }
  return newbuf;
}

char* shell_readline(char* prompt) {

  char* gotLine = readline(prompt);
  // If readline encounters an EOF while reading the line, and the line is empty at that point, then (char *)NULL is returned
  // http://www.delorie.com/gnu/docs/readline/rlman_24.html
  if (gotLine == NULL) {
    exit(1);
  }
  return gotLine;
}

char** shell_splitline(char *line) {
  int curr_bufsize = INIT_BUFSIZE;
  char **tokens = malloc(curr_bufsize * sizeof(char*));
  char *token;

  if(!tokens) {
    fprintf(stderr, "shell: malloc failure\n");
    exit(1);
  }

  int idx = 0;
  token = strtok(line, DELIMITER);
  while (token != NULL) {
    tokens[idx] = token;
    idx++;

    if (idx >= curr_bufsize) {
      curr_bufsize += INIT_BUFSIZE;
      tokens = d_realloc(tokens, curr_bufsize * sizeof(char*));
      if(!tokens) {
        fprintf(stderr, "malloc failure\n");
        exit(1);
      }
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
    if (execvp(args[0], args) < 0) {
      perror("msg");
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
  if(args[0] == NULL) {
    return 1;
  }
  // checks builtins first
  for(int i = 0; i < num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) {
      return (*builtin_func[i])(args);
    }
  }
  // Asks the system 
  return shell_system(args);
}

