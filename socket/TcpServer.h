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
 int m_listenfd; // 监听套接字
 int m_clientfd; // 客户端套接字

public:
 TcpServer();
 ~TcpServer();

 bool Init(int port); // 完成监听套接字的初始化和绑定、监听操作
 bool Accept(); // 调用accept等待客户端的连接
 int Send(const void *buf, size_t len, int flags); // 间接调用send函数 
 int Send(int sockfd, const void *buf, size_t len, int flags); // 为多线程服务端重载的函数
 int Recv(void *buf, size_t len, int flags);  // 间接调用recv函数
 int Recv(int sockfd, void *buf, size_t len, int flags); // 为多线程服务端重载的函数 


 // 获取、设置套接字
 int GetListen() { return m_listenfd; }
 bool SetListen(int var) { m_listenfd = var; }
 int GetClient() { return m_clientfd; }
 bool SetClient(int var) { m_clientfd = var; }

};



