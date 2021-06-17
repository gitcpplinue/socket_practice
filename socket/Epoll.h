#pragma once
#include <stdio.h>
#include <errno.h>
#include <sys/epoll.h>


#define MAXEVENTS 1024

class Epoll
{
private:
 int m_epollfd; // epoll标识符
 epoll_event m_ev; // 
 epoll_event m_evs[MAXEVENTS]; // 

public:
 Epoll();
 ~Epoll();

 bool Create(); // 创建epoll

 // 添加、修改、删除。3者操作基本相同
 bool Add(int fd, int event); 
 bool Modify(int fd, int event);
 bool Delete(int fd, int event);

 int Wait(int size = MAXEVENTS, int wait_t = -1); // 间接调用epoll_wait
 epoll_event GetEvent(int index); // 获取m_evs数组中下标为index的元素

};
