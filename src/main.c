#include <stdio.h>
#include "shell_util.h" 

int main(int argc, char **argv) {

  // write it as " $ " (remember the spaces and qutation marks)
  shell_loop(argc >= 1 ? argv[1] : " >>> ");

  return 0;
}