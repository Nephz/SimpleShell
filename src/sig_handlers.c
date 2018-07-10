#include <signal.h>
#include <setjmp.h>

sigjmp_buf env;

// volatile sig_atomic_t is needed as this runs async.
volatile sig_atomic_t jmp_active = 0;

void SIGINT_handler(int sig) {
  // we use this as we could get sigint before the jump is set which would result in a crash - 
  // Probably a seg-fault?
  if (!jmp_active) {
    return;
  }
  siglongjmp(env, 1);
}

