CC=gcc
CFLAGS = -std=c99 -Wall -Werror -Wextra -Wsign-conversion -Wconversion -Wshadow -Wdouble-promotion -pedantic -g
DEPS=lclient.h laccount.h
OBJ=lclient.o laccount.o menu.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

banking-app: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
