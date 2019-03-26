CC = gcc
CFLAGS = -c `pkg-config --cflags gtk+-3.0`
OFLAGS = -o
WFLAGS = -pedantic-errors -Wall -Wextra
LIBS=`pkg-config --cflags --libs gtk+-3.0`
RM = rm -f
PRINT = @echo

all: ffw-scanner

ffw-scanner: main.o functions.o
	$(CC) $(LIBS) main.o functions.o $(OFLAGS) ffw-scanner

main.o: main.c
	$(CC) $(LIBS) $(WFLAGS) $(CFLAGS) main.c

functions.o: functions.c functions.h
	$(CC) $(LIBS) $(WFLAGS) $(CFLAGS) functions.c

clean:
	$(RM) *.o ffw-scanner
