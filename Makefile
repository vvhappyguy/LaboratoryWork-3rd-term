build:
	g++ -c addrbook.cpp -o addrbook.o
	g++ addrbook.o -o ssdb.out
	rm addrbook.o