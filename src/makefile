HEADERS = builtin.h shell_util.h sig_handlers.h util.h pipe_redirect.h
EXE = main.o builtin.o shell_util.o sig_handlers.o util.o pipe_redirect.o

.PHONY: clean 

default: program

program: $(EXE) $(HEADERS)
	gcc -std=c11 $(EXE) -o shell -L.local/lin -lreadline

main.o: main.c
	gcc -c  main.c

shell_util.o: shell_util.c
	gcc -c shell_util.c

builtin.o: builtin.c
	gcc -c builtin.c

sig_handlers.o: sig_handlers.c
	gcc -c sig_handlers.c

util.o: util.c
	gcc -c util.c

pipe_redirect.o: pipe_redirect.c
	gcc -c pipe_redirect.c

clean:
	rm *.o