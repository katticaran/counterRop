CC=g++
CFLAGS=-std=c++11 -Wall -pedantic -ggdb
DEPS=trie.h
OBJ=trieTest.o trie.o
OUTPUT=trieTest

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OUTPUT): $(OBJ)
	gcc $(CFLAGS) -o $@ $^

clean:
	@rm -f *~
	@rm -f *.o
	@rm -f $(OUTPUT)
	@rm -f names
