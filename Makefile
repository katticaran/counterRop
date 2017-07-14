all:  libaddr.so basic

basic: basic.o
	gcc -o basic basic.c -ggdb

libaddr.so: ./elf/libaddr.cc
	clang++ ./elf/libaddr.cc ./list/list.c -ggdb --std=c++11 -rdynamic -shared -fPIC -o libaddr.so -ldl

clean:
	@rm -f *~
	@rm -f \#*\#
	@rm -f *.o
	@rm -f *.so
