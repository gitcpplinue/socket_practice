#include "Epoll.h"


Epoll::Epoll()
{
 m_epollfd = 0;
}


Epoll::~Epoll()
{
 if(m_epollfd != 0)
  m_epollfd = 0;
 close(m_epollfd);
}



bool Epoll::Create()
{
 m_epollfd = epoll_create(1);
 if(m_epollfd == -1)
 {
  perror("epoll_create");
  return false;
 }
 return true;
}


bool Epoll::Add(int fd, int event)
{
 m_ev.data.fd = fd;
 m_ev.events = event;
 
 if(epoll_ctl(m_epollfd, EPOLL_CTL_ADD, fd, &m_ev) == -1)
 {
  perror("epoll_ctl");
  return false;
 }
 return true;
}


bool Epoll::Modify(int fd, int event)
{
 m_ev.data.fd = fd;
 m_ev.events = event;

 if(epoll_ctl(m_epollfd, EPOLL_CTL_MOD, fd, &m_ev) == -1)
 {
  perror("epoll_ctl");
  return false;
 }
 return true;
}


bool Epoll::Delete(int fd, int event)
{
 m_ev.data.fd = fd;
 m_ev.events = event;

 if(epoll_ctl(m_epollfd, EPOLL_CTL_DEL, fd, &m_ev) == -1)
 {
  perror("epoll_ctl");
  return false;
 }
 return true;
}


int Epoll::Wait(int size, int wait_t )
{
 if(size <= 0)
  size = 1;
 else if(size > MAXEVENTS)
  size = MAXEVENTS;
 if(wait_t < -1)
  wait_t = -1;

 int ret;
 ret = epoll_wait(m_epollfd, m_evs, size, wait_t);
 if(ret == -1)
 {
  perror("epoll_wait");
  return -1;
 }

 return ret;
}


epoll_event Epoll::GetEvent(int index)
{
 if(index < 0)
  index = 0;
 else if(index > MAXEVENTS)
  index = MAXEVENTS - 1;

 return m_evs[index];

}




