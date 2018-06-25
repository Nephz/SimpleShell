#include <signal.h>
#include <setjmp.h>

static sigjmp_buf env;
// volatile sig_atomic_t is needed as this runs async.
volatile sig_atomic_t jump_active = 0;

void SIGINT_handler(int sig) [
  if (!jump_active) {
    return;
  }
  siglongjmp(env, 1);
]

