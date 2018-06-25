#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "builtin.h" 

#define INIT_BUFSIZE 128
#define DELIMITER " \t\r\n\a"

static void* d_realloc(void* buf, size_t size) {
  void *newbuf = realloc(buf, size);
  if (!newbuf) {
    free(buf);
  }
  return newbuf;
}

char* shell_readline() {
  int curr_bufsize = INIT_BUFSIZE;
  char *buffer = malloc(sizeof(char) * curr_bufsize);

  if (!curr_bufsize) {
    fprintf(stderr, "shell: malloc failure\n");
    exit(1);
  }

  int c;
  int idx = 0;
  while(1) {
    c = getchar();

    if (c == EOF) {
      exit(1);
    } else if (c == '\n') {
      buffer[idx] = '\0';
      return buffer;
    } else {
      buffer[idx] = c;
    }
    idx++;

    if (idx >= curr_bufsize) {
      curr_bufsize += INIT_BUFSIZE;
      buffer = d_realloc(buffer, curr_bufsize);
      if (!buffer) {
        fprintf(stderr, "shell: malloc failure\n");
        exit(1);
      }
    }
  }
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

  pid_t child = fork();
  int status;


  struct sigaction sig;
  sig.sa_handler = SIGINT_handler;
  sigemptyset(&sig.sa_mask);
  // system calls will be restarted after the signalhandler has finished its execution
  sig.sa_flags = SA_RESTART;
  sigaction(SIGINT, &sig, NULL);

  if (child == 0) {
    if (execvp(args[0], args) < 0) {
      perror("msg");
    }
    exit(1);
  } else if (child < 0) {
    perror("msg");
  } else {
    // https://linux.die.net/man/2/waitpid example given at the bottom

    struct sigaction sig;
    sig.sa_handler = SIGINT_handler; // set to SIG_DFL?
    sigemptyset(&sig.sa_mask);
    // system calls will be restarted after the signalhandler has finished its execution
    sig.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sig, NULL);

    do {
      waitpid(child, &status, WUNTRACED);
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

