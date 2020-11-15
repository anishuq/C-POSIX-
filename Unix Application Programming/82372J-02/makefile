file_clean:file_clean.o
	gcc -o file_clean  file_clean.o

file_clean.o:file_clean.c file_clean_lib.h
	gcc -Wall -pedantic -c file_clean.c

all:file_clean 

clean:
	rm file_clean file_clean.o
