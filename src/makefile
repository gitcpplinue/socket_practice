all:test 

run: test
	./test

test: test.cpp 
	g++ -g -o test test.cpp -lpthread -std=c++11
	



net: server client

server: server.cpp TcpServer.cpp TcpServer.h 
	g++ -g -o server server.cpp TcpServer.cpp -lpthread -std=c++11

client: client.cpp TcpClient.cpp TcpClient.h
	g++ -g -o client client.cpp TcpClient.cpp -lpthread -std=c++11



delete:
	rm -f test

