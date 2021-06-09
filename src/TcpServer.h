#pragma once

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>


class TcpServer
{
private:
 int m_listenfd;
 int m_clientfd;

public:
 TcpServer();
 TcpServer(int port);
 ~TcpServer();

 bool Init(int port); // 完成监听套接字的初始化和绑定、监听操作
 bool Accept(); // 调用accept等待客户端的连接
 int Send(const void *buf, size_t len, int flags); // 调用send向连接的另一端发送数据
 int Recv(void *buf, size_t len, int flags); // 调用recv接收连接另一端的数据


 int GetListen() { return m_listenfd; }
 bool SetListen(int var) { m_listenfd = var; }
 int GetClient() { return m_clientfd; }
 bool SetClient(int var) { m_clientfd = var; }

};



