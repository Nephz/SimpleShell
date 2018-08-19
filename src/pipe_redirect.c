#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#define INPUT_END 0
#define OUTPUT_END 1

#define IS_PIPE(c) (*c == '|' || *c == '=')

// number of pipes
// number of string entries
// The array with strings
// The array (which in find_replace_pipes(), will be modified.)
typedef struct pip {
  int n_pipes;
  int n_str;
} pip;


int pipe_check(char **p) {
  for (int i = 0; *(p + i) != NULL; i++) {
    char *chr = *(p + i);
    // two kinds of pipes (| or =)
    if (strlen(chr) == 1 && IS_PIPE(chr)) {
      return 1;
    }
  }
  return 0;
}

// Should only be called if pipe_check evals to true.
pip* find_replace_pipes(char **p) {
  pip* ret = malloc(sizeof(pip));
  for (int i = 0; *(p + i) != NULL; i++) {
    char *chr = *(p + i);
    if (IS_PIPE(chr)) {
      ret->n_pipes++;
      *(p + i) = NULL; 
    }
    ret->n_str++; // Last "null" is not counted.
  }
  return ret;
}

void pipe_stuff(char **p) {
  int status;
  pid_t w;
  pid_t pid;
  int fd[2];

  pip* info = find_replace_pipes(p);
  pipe(fd);

  pid = fork();

  if (pid == 0){
    close(fd[INPUT_END]);
    if (dup2(fd[OUTPUT_END], STDOUT_FILENO) == -1) {
      fprintf(stderr, "dup2 fail\n ");
      exit(EXIT_FAILURE);
    }
    close(fd[OUTPUT_END]);

    if (execvp(*p, p) < 0) {
      fprintf(stderr, "msg: Execution failed\n ");
    }
    
    exit(EXIT_SUCCESS);
  } else {
    pid = fork();

    if (pid == 0) {
      close(fd[OUTPUT_END]);

      if (dup2(fd[INPUT_END], STDIN_FILENO) == -1) {
        fprintf(stderr, "dup2 fail\n ");
        exit(EXIT_FAILURE);
      }
      close(fd[INPUT_END]);

      if (execvp(*(p + 2), p + 2) < 0) {
        fprintf(stderr, "msg: Execution failed\n ");
      }

      exit(EXIT_SUCCESS);
    } else {
      close(fd[OUTPUT_END]);
      close(fd[INPUT_END]);
      do {
        w = waitpid(pid, &status, WUNTRACED);
        if (w == -1) {
          perror("waitpid");
          exit(EXIT_FAILURE);
        }
      } while(!WIFEXITED(status) && !WIFSIGNALED(status));
    }
  }
  
  free(info);
}