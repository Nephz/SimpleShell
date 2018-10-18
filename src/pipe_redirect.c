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
#define IS_REDIRECT(c) (*c == '<' || *c == '>')

typedef enum symbol {
  sym_pipe,
  sym_redirect,
} symbol;

typedef struct pip {
  int n_sym;
  int n_str;
  symbol* syms;
} pip;


// Returns the number of symbols found (see the enum)
int count_symbols(char **p) {
  int acc = 0;
  for (int i = 0; *(p + i) != NULL; i++) {
    char *chr = *(p + i);
    // two kinds of pipes (| or =)
    if (strlen(chr) == 1 && (IS_PIPE(chr))) {
      acc++;
    } else if (IS_REDIRECT(chr)) {
      fprintf(stderr, "redirection is not implemented yet");
      exit(EXIT_FAILURE);
    }
  }
  return acc;
}

// Should only be called if pipe_check evals to true.
static pip* find_replace_symbols(char **p, int n_symbols) {
  pip* ret = malloc(sizeof(pip));
  ret->syms = malloc(sizeof(symbol) * n_symbols);
  ret->n_sym = 0;
  ret->n_str = 0;
  //printf("ååå %lu \n", sizeof(symbol *) * n_symbols);
  symbol *tmp = ret->syms;

  for (int i = 0, k = 0; *(p + i) != NULL; i++) {
    char *chr = *(p + i);
    if (IS_PIPE(chr)) {
      ret->n_sym++;
      *(p + i) = NULL;
      tmp[k] = sym_pipe;
      k++;
    } else if (IS_REDIRECT(chr)) {
      ret->n_sym++;
      *(p + i) = NULL;
      tmp[k] = sym_redirect;
      k++;
    } else {
      ret->n_str++;
    }
  }

  if (ret->n_sym >= ret->n_str) {
    fprintf(stderr, "msg: Can't determine the pipeline\n");
  }

  return ret;
}

void pipeline_execute(char **p, pip* info) { 
  int status = 0;
  pid_t w;
  pid_t pid;

  symbol *ptr = info->syms;
  int len = info->n_str + info->n_sym;
  int n_nulls = info->n_sym + 1;

  int fd_pipes[2*info->n_sym];

  // Create pipes
  for (int i = 0; i < info->n_sym; i++) {
    if (pipe(fd_pipes + i*2) < 0) {
      perror("msg");
    }
  }

  int cmds[info->n_str];
  int k = 0;
  for (int i = 0; i < len; i++) {
    if (p[i]) {
      cmds[k] = i;
      k++;
      i++;
      while (p[i]) {
        i++;
      }
    }
  }
  cmds[k] = -1;

  int cmds_len = k;

  for (int idx = 0, cmd = 0; idx < k; idx++, cmd++) {
    pid = fork();

    if (pid == 0) {
      // first command does not take input from other process
      if (cmd != 0) {
        if (dup2(fd_pipes[(cmd-1)*2], 0) == -1) {
          fprintf(stderr, "input dup2 failed at cmd=%d \n", cmd);
          perror("input");
          exit(EXIT_FAILURE);
        }
      }
      // last command ouputs to stdout (or internally used fd).
      if (cmd < cmds_len-1) {
        if (dup2(fd_pipes[cmd*2+1], 1) == -1) {
          fprintf(stderr, "output dup2 failed at cmd=%d \n", cmd);
          perror("output");
          exit(EXIT_FAILURE);
        }
      }

      for (int i = 0; i < 2*info->n_sym; i++) {
        close(fd_pipes[i]);
      }

      if (execvp(p[cmds[cmd]], &p[cmds[cmd]]) < 0) {
        fprintf(stderr, "msg: Execution failed\n ");
      }

      exit(EXIT_SUCCESS);

    } else if (pid < 0) {
      perror("msg");
      exit(EXIT_FAILURE);
    }
  }

  for (int i = 0; i < info->n_sym*2; i++) {
    close(fd_pipes[i]);
  }

  while ((w = wait(&status)) > 0);
}

void pipe_stuff(char **p, int n_symbols) {
  pip *info = find_replace_symbols(p, n_symbols);
  pipeline_execute(p, info);

  free(info->syms);
  free(info);
}
