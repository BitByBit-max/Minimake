CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -Werror -pedantic -Wvla -g
LDFLAGS=-fsanitize=address
SRC=src/micromake.c
OBJ=${SRC:.c=.o}

all: micromake

micromake: $(OBJ)
	$(CC) $(CFLAGS) -o micromake $(OBJ) $(LDFLAGS)

clean:
	$(RM) micromake $(OBJ)
