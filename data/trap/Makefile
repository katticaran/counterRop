CC=clang
CFLAGS=-std=c11 -ggdb -Wall -pedantic
DEPS=trap.h
OBJ=test_trap.o trap.o
OUTPUT=test_trap

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OUTPUT): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ 

clean:
	@rm -f *~
	@rm -f *.o
	@rm -f $(OUTPUT)
