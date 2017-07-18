CC=gcc
CFLAGS=-std=c11 -Wall -pedantic -ggdb
DEPS=list.h
OBJ=test_list.o list.o
OUTPUT=test_list

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OUTPUT): $(OBJ)
	gcc $(CFLAGS) -o $@ $^

clean:
	@rm -f *~
	@rm -f *.o
	@rm -f $(OUTPUT)
	@rm -f names
