#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "shell_util.h" 

int shell_cd(char **args);
int shell_exit(char **args);

char* builtin_str[] = {
  "cd",
  "exit"
};

// Function pointers.
// Array of function pointers taking array of strings, returning int
int (*builtin_func[]) (char **) = {
  &shell_cd,
  &shell_exit
};

// This wrapper is needed apparently?
int num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

static char prev[1024];

int shell_cd(char **args) {
  if (*(args + 1) == NULL) {
    fprintf(stderr, "msg: expected argument to \"cd\"\n");
  } else if (**(args + 1) == '-') {
    char tmp[1024];
    strcpy(tmp, prev);
    getcwd(prev, sizeof(prev));
    if (chdir(tmp) != 0) {
      perror("msg");
    }
  } else {
    getcwd(prev, sizeof(prev));  
    if (chdir(*(args + 1)) != 0) {
      perror("msg");
    }
  }
  return 1;
}

int shell_exit(char **args) {
  return 0;
}