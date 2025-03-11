CC = gcc
Flags = -Wall -g

all: myShell

myShell: myShell.o myFunctionsShell.o
	$(CC) $(Flags) -o myShell myShell.o myFunctionsShell.o

myShell.o: myShell.c myShell.h
	$(CC) $(Flags) -c myShell.c

myFunctionsShell.o: myFunctionsShell.c myFunctionsShell.h
	$(CC) $(Flags) -c myFunctionsShell.c

clean:
	rm -f *.o myShell
