#include <iostream>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <time.h>
#include <dirent.h>
#include <string>

#include "./socket/TcpClient.h"
#include "./tool/SIG.h"
#include "./tool/Log.h"

using namespace std;

#define MAX 50000

void* threadfoo(void* arg);

TcpClient g_tcpc[MAX];
int main()
{
 pthread_t tids[MAX];
 
 for(int i = 0; i < MAX; ++i)
 {
  pthread_create(&tids[i], NULL, threadfoo, (void*)(long)i);
 }

 for(int i = 0; i < MAX; ++i)
  pthread_join(tids[i], 0);

}

void* threadfoo(void* arg)
{
 int seq = (long)arg;

 g_tcpc[seq].Init();
 g_tcpc[seq].Connect("127.0.0.1", 5000);
/*
 char buffer[100];
 for(int i = 0; i < seq + 1; ++i) 
 {
  sleep(1);

  int iret;
  memset(buffer, 0, sizeof(buffer));
  sprintf(buffer, "第%d条消息，编号%03d。", i + 1 ,i + 1);

  iret = g_tcpc[seq].Send(buffer, strlen(buffer), 0);
  if(iret <= 0)
  {
   perror("send");
   break;
  }
  printf("发送：%s \n", buffer);


  memset(buffer, 0, sizeof(buffer));
  iret = g_tcpc[seq].Recv(buffer, sizeof(buffer), 0);
  if(iret <= 0)
  {
   printf("iret = %d \n", iret);
   break;
  }

//  printf("接收：%s \n", buffer);
//  printf("--------------------------\n");
 }*/
 pthread_exit(0);
}
