#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "shell_util.h" 

int shell_cd(char **args);
int shell_exit(char **args);

static int check_buff(char* buf, int bufLen);

char* builtin_str[] = {
  "cd",
  "exit"
};
// Function pointers.
// Array of function pointers taking array of strings, returning int
int (*builtin_func[]) (char**) = {
  &shell_cd,
  &shell_exit
};

// This wrapper is needed apparently?
int num_builtins() {
  return sizeof(builtin_str) / sizeof(char*);
}

static char prev[PATH_MAX] = {0};
static char tmp[PATH_MAX]  = {0};

int shell_cd(char **args) {
  if (*(args + 1) == NULL) {
    fprintf(stderr, "msg: expected argument to \"cd\"\n");
  } else if (**(args + 1) == '-' && !*(*(args + 1) + 1) && !*(args + 2)) {
    if (check_buff(prev, PATH_MAX)) {
      return 1;
    }
    strcpy(tmp, prev);
    getcwd(prev, PATH_MAX);
    if (chdir(tmp) != 0) {
      perror("msg");
    }
  } else {
    getcwd(prev, PATH_MAX);  
    if (chdir(*(args + 1)) != 0) {
      perror("msg");
    }
  }

  return 1;
}

int shell_exit(char **args) {
  return 0;
}

static int check_buff(char* buf, int bufLen) {
  for (int i = 0; i < bufLen; i++) {
    if ((*(buf + i)) != 0) {
      return 0;
    }
  }
  return 1;
}