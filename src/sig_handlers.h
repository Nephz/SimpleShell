extern sigjmp_buf env;
extern volatile sig_atomic_t jmp_active;

void SIGINT_handler(int sig);