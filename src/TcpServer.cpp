#include "TcpServer.h"

TcpServer::TcpServer()
{ Init(5000); }


TcpServer::TcpServer(int port)
{ Init(port); }


TcpServer::~TcpServer()
{ 
 close(m_listenfd);
 close(m_clientfd);
}



bool TcpServer::Init(int port)
{
 m_listenfd = socket(AF_INET, SOCK_STREAM, 0);
 if(m_listenfd == -1)
 {
  perror("socket");
  return false;
 }

 sockaddr_in servaddr;
 memset(&servaddr, 0, sizeof(servaddr));
 servaddr.sin_family = AF_INET;
 servaddr.sin_port = htons(port);
 servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

 if(bind(m_listenfd, (sockaddr*)&servaddr, sizeof(servaddr)) != 0)
 {
  perror("bind");
  return false;
 }
 
 if(listen(m_listenfd, 10) != 0)
 {
  perror("listen");
  return false;
 }

 printf("等待客户端连接\n");

 return true;
}


bool TcpServer::Accept()
{
 int socklen = sizeof(sockaddr_in);
 sockaddr_in clientaddr;
 m_clientfd = accept(m_listenfd, (sockaddr*)&clientaddr, (socklen_t*)&socklen);
 if(m_clientfd == -1)
 {
  perror("accept");
  return false;
 }

 printf("客户端(%s)已连接。\n", inet_ntoa(clientaddr.sin_addr));
}


int TcpServer::Send(const void* buf, size_t len, int flags)
{
 return send(m_clientfd, buf, len, flags);
}


int TcpServer::Recv(void* buf, size_t len, int flags)
{
 return recv(m_clientfd, buf, len, flags);
}


