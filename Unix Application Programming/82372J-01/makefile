morse_code:morse_code.o
	gcc -o morse_code  morse_code.o

morse_code.o:morse_code.c morse_lib.h
	gcc -Wall -pedantic -c morse_code.c

all:morse_code 

clean:
	rm morse_code morse_code.o
