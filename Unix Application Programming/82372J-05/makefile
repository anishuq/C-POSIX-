all: asyn_io reg_io
asyn_io:asyn_io.o
	gcc -o asyn_io -lrt asyn_io.o
reg_io:reg_io.o
	gcc -o reg_io  reg_io.o
asyn_io.o:asyn_io.c 
	gcc -Wall -pedantic -c asyn_io.c
reg_io.o:reg_io.c 
	gcc -Wall -pedantic -c reg_io.c

clean:
	rm asyn_io asyn_io.o reg_io reg_io.o
