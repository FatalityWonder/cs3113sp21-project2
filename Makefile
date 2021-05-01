CC = gcc

all: project2.c
		gcc -g -Wall -o project2 project2.c

clean:
		$(RM) project2