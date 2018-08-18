char* shell_readline();
char** shell_splitline(char *line);
int shell_system(char **args);
int shell_command(char **args);
void shell_loop(char* prompt);