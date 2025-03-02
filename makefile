/* makefile */
CC=gcc
CFLAGS=-Wall -Wextra -std=c99

all: myShell

myShell: myShell.o myFunctionsShell.o
	$(CC) $(CFLAGS) -o myShell myShell.o myFunctionsShell.o

myShell.o: myShell.c myShell.h myFunctionsShell.h
	$(CC) $(CFLAGS) -c myShell.c

myFunctionsShell.o: myFunctionsShell.c myFunctionsShell.h
	$(CC) $(CFLAGS) -c myFunctionsShell.c

clean:
	rm -f myShell myShell.o myFunctionsShell.o
