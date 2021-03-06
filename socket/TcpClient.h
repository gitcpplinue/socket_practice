#pragma once

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>


class TcpClient
{
private:
 int m_sockfd; // 客户端的套接字


public:
 TcpClient();
 ~TcpClient();

 bool Init(); // 初始化套接字
 bool Connect(const char *addr, int port); // 连接到服务端
 int Send(const void *buf, size_t len, int flags); // 调用send向连接的另一端发送数据
 int Recv(void *buf, size_t len, int flags); // 调用recv接收连接另一端的数据

 // 获取、设置套接字
 int GetSocket() { return m_sockfd; }
 bool SetSocket(int var) { m_sockfd = var; }

};



