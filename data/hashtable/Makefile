CC=gcc
CFLAGS=-std=c11 -Wall -pedantic -ggdb
DEPS=hashtable.h jhash.h 
OBJ=hashtest.o hashtable.o jhash.o
OUTPUT=hashtest

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OUTPUT): $(OBJ)
	gcc $(CFLAGS) -o $@ $^

clean:
	@rm -f *~
	@rm -f *.o
	@rm -f \#*
	@rm -f $(OUTPUT)
	@rm -f names
