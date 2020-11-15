hello_world:hello_world.o
	gcc -o hello_world  hello_world.o

hello_world.o:hello_world.c hello_lib.h
	gcc -Wall -pedantic -c hello_world.c

all:hello_world 

clean:
	rm hello_world hello_world.o
