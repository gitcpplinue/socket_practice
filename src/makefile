all:test 

run: test
	./test

test: test.cpp 
	g++ -g -o test test.cpp 
	



net: server client

server: server.cpp 
	g++ -g -o server server.cpp

client: client.cpp
	g++ -g -o client client.cpp 



delete:
	rm -f test

