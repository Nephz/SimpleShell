#include <stdio.h>
#include <stdlib.h>
#include "shell_util.h"
#include "util.h"

// Idk why i made this
void nullchecker(void *p, char* msg, errnoflag option) {
  if (!p) {
    switch(option) {
      case Nothing:
        exit(EXIT_FAILURE);
        break;
      case Msg:
        fprintf(stderr, "msg: %s" ,msg);
        exit(EXIT_FAILURE);
        break;
      case Error:
        perror(msg);
        exit(EXIT_FAILURE);
        break;
      default:
        fprintf(stderr, "Idk what just happened");
        break;
    }
  }
}

void* d_realloc(void* buf, size_t size) {
  void* newbuf = realloc(buf, size);
  if (!newbuf) {
    free(buf);
    fprintf(stderr, "msg: Memory allocation error when realloc\n");
    exit(EXIT_FAILURE);
  }
  return newbuf;
}

