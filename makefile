dir_tool   = ./tool 
dir_socket = ./socket

tool = $(wildcard ./tool/*.cpp)
# tool = $(wildcard $(dir_tool)/*.cpp) tool被解析为“./tool”。不能在函数中使用变量？？

# 封装的TCP类
Tcps = $(dir_socket)/TcpServer.cpp $(tool)
Tcpc =  $(dir_socket)/TcpClient.cpp

Epoll = $(dir_socket)/epoll_server.cpp $(dir_socket)/Eopll.cpp $(Tcps)


test: test.cpp $(tool) $(Tcpc)
	g++ -g -o test $^ -lpthread -std=c++11
run: test
	./test


net: server client

server: $(dir_socket)/server.cpp $(Tcps) 
	g++ -g -o server $^ -lpthread -std=c++11
epoll_server: $(Epoll)
	g++ -g -o epoll_server $(Epoll) -lpthread -std=c++11

client: $(dir_socket)/client.cpp $(Tcpc)
	g++ -g -o client $^ -lpthread -std=c++11


clean:
	rm -f client server epoll_server test

