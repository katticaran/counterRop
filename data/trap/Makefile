CC=gcc
CFLAGS=-std=c11 -ggdb -Wall -pedantic -D_POSIX_C_SOURCE=199309L
DEPS=trap.h
OBJ=trap_test.o trap.o
OUTPUT=trap_test

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OUTPUT): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ 

clean:
	@rm -f *~
	@rm -f *.o
	@rm -f $(OUTPUT)
