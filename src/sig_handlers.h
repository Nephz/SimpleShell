
extern volatile sig_atomic_t jmp_active;
extern sigjmp_buf env;

void SIGINT_handler(int sig);