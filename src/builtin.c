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
// Array of function pointers, which takes array of strings and returns int
int (*builtin_func[]) (char**) = {
  &shell_cd,
  &shell_exit
};

// This wrapper is needed apparently?
int num_builtins() {
  return sizeof(builtin_str) / sizeof(char*);
}

// TODO: make another util file with helper functions like this.
void nullchecker(void *p, char* msg) {
  if (!p) {
    fprintf(stderr, "%s" ,msg);
    exit(1);
  }
}

int shell_cd(char **args) {
  // Maybe we should use the feature of getcwd to allocate dynamically
  char cur_pwd[PATH_MAX + 1];
  
  // if no argument or if argument is '~' 
  if (*(args + 1) == NULL || (**(args + 1) == '~' && !*(*(args + 1) + 1) && !*(args + 2))) {
    setenv("OLDPWD", getenv("PWD"), 1);
    char *home = getenv("HOME");
    if (chdir(home) != 0) {
      perror("msg");
      exit(1);
    }
    getcwd(cur_pwd, PATH_MAX);
    setenv("PWD", cur_pwd, 1);
  } else if (**(args + 1) == '-' && !*(*(args + 1) + 1) && !*(args + 2)) {
    char new_old_pwd[PATH_MAX] = {0};
    char *p = getenv("OLDPWD");

    nullchecker(p, "Failure when getting env variable: OLDPWD");

    strcpy(new_old_pwd, p);

    setenv("OLDPWD", getenv("PWD"), 1);

    if (chdir(new_old_pwd) != 0) {
      perror("msg");
      exit(1);
    }

    getcwd(cur_pwd, PATH_MAX);
    setenv("PWD", cur_pwd, 1);

  } else {
    setenv("OLDPWD", getenv("PWD"), 1); 
    if (chdir(*(args + 1)) != 0) {
      perror("msg");
      exit(1);
    }
    getcwd(cur_pwd, PATH_MAX);
    setenv("PWD", cur_pwd, 1);
  }
  return 1;
}

int shell_exit(char **args) {
  return 0;
}