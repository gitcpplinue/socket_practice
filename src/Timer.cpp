#include "Timer.h"

Timer::Timer()
{
 memset(&m_sec, 0, sizeof(m_sec));
 memset(&m_begin, 0, sizeof(m_begin));
 memset(&m_end, 0, sizeof(m_end));
 m_state = stop;
}

Timer::~Timer
{}


int Timer::Start()
{
 if(m_state != running)  
 {
  timeval begin;

  gettimeofday(&begin, 0); 
  m_begin = Time2Double(begin);
  m_state = running;

  return 0;
 }
 else
  return -1;
}

int Timer::Pause()
{
 if(m_state == running)  
 { 
  timeval end;

  gettimeofday(&end, 0); 
  m_end = Time2Double(end);
  m_count += m_end - m_begin;
  m_state = pause;

  return m_count;
 }
 else
  return -1;
}

int Timer::Stop()
{
 if(m_state == stop)  
  return -1;

 if(m_state == running)
 { 
  timeval end;

  gettimeofday(&end, 0); 
  m_end = Time2Double(end);
  m_count += m_end - m_begin;
  m_state = stop;
 }

  doube tmp = m_count;
  m_count = 0;
  return tmp;
}



string LocalTime()
{
 time_t cur_t;
 tm *tmp;
 char buf[50];

 memset(buf, 0, sizeof(buf));

 time(&cur_t);
 tmp = localtime(&cur_t);
 strftime(buf, 50, "[%F %X]", tmp); 
 
 string ret = buf;
 return ret;
}



double Timer::Time2Double(timeval time)
{
  char tmp[20]; 
  memset(tmp, 0, 20);
  sprintf(tmp, "%ld.%ld", time.tv_sec, time.tv_usec);
  return atof(tmp);
}


