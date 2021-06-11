#pragma once
#include <stdio.h>
#include <errno.h>
#include <sys/epoll.h>


#define MAXEVENTS 1024

class Epoll
{
private:
 int m_epollfd;
 epoll_event m_ev;
 epoll_event m_evs[MAXEVENTS];

public:
 Epoll();
 ~Epoll();

 bool Create();
 bool Add(int fd, int event);
 bool Modify(int fd, int event);
 bool Delete(int fd, int event);
 int Wait(int size = MAXEVENTS, int wait_t = -1);
 epoll_event GetEvent(int index);

};
