
so:
	g++ -O3 -fpic -c -I./include src/Message.cc src/Client.cc
	g++ -shared -o lib739kv.so  Client.o Message.o

test:
	g++ -O3 src/test.cpp -I./include -L. -l739kv