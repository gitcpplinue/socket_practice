tool = $(tool.h) $(tool.cpp)
tool.cpp = Log.cpp Timer.cpp
tool.h = Log.h Timer.h


test: test.cpp $(tool)
	g++ -g -o test test.cpp $(tool.cpp) TcpClient.cpp -lpthread -std=c++11
	
run: test
	./test



net: server client

server: server.cpp TcpServer.cpp TcpServer.h $(tool)
	g++ -g -o server server.cpp TcpServer.cpp $(tool.cpp) -lpthread -std=c++11

epoll_server: epoll_server.cpp Epoll.h Epoll.cpp TcpServer.cpp TcpServer.h 
	g++ -g -o epoll_server Epoll.cpp epoll_server.cpp TcpServer.cpp -lpthread -std=c++11


client: client.cpp TcpClient.cpp TcpClient.h
	g++ -g -o client client.cpp TcpClient.cpp -lpthread -std=c++11



delete:
	rm -f test

