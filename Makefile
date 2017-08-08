DEPS = ./data/list/list.c ./data/hashtable/hashtable.c ./data/hashtable/jhash.c ./data/trap/trap.c ./data/boundTree/boundTree.c

all:  libaddr.so basic

basic: basic.o
	gcc -o basic basic.c -ggdb -Wall -pedantic

libaddr.so: ./elf/libaddr.cc $(DEPS)
	clang++ ./elf/libaddr.cc $(DEPS)  -ggdb --std=c++11 -rdynamic -shared -fPIC -o libaddr.so -ldl

clean:
	@rm -f *~
	@rm -f \#*\#
	@rm -f *.o
	@rm -f *.so
