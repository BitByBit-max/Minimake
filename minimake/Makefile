CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -Werror -pedantic -Wvla -g
LDFLAGS=-fsanitize=address
SRCNOMAIN=src/parsing.c src/print.c src/building.c
SRC=src/minimake.c $(SRCNOMAIN)
OBJ=${SRC:.c=.o}
TEST=tests/first_test.c
TESTOBJ=${TEST:.c=.o}

all: minimake

minimake: $(OBJ)
	$(CC) -o minimake $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $< $(LDFLAGS)

check: $(TEST) $(SRCNOMAIN)
	$(CC) $(CFLAGS) -lcriterion $^ -o testsuite $(LDFLAGS)
	./testsuite

clean:
	$(RM) minimake $(OBJ) $(TESTOBJ) testsuite
