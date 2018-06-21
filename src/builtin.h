char *builtin_str[];
int (*builtin_func[]) (char **);
int num_builtins();
int shell_cd(char **args);
int shell_exit(char **args);
