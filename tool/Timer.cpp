#include "Timer.h"

Timer::Timer()
{
 m_begin = m_end = m_count = 0;
 m_state = STOP;
}

Timer::~Timer()
{}


double Timer::Start()
{
 if(m_state != RUN)  
 {
  timeval begin;

  gettimeofday(&begin, 0); 
  m_begin = Time2Double(begin);
  m_state = RUN;

  return 0;
 }
 else
  return -1;
}

double Timer::Pause()
{
 if(m_state == RUN)  
 { 
  timeval end;

  gettimeofday(&end, 0); 
  m_end = Time2Double(end);
  m_count += m_end - m_begin;
  m_state = PAUSE;

  return m_count;
 }
 else
  return -1;
}

double Timer::Stop()
{
 if(m_state == STOP)  
  return -1;

 if(m_state == RUN)
 { 
  timeval end;

  gettimeofday(&end, 0); 
  m_end = Time2Double(end);
  m_count += m_end - m_begin;
  m_state = STOP;
 }

  double tmp = m_count;
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




string Date()
{
 time_t cur_t;
 tm *tmp;
 char buf[50];

 memset(buf, 0, sizeof(buf));

 time(&cur_t);
 tmp = localtime(&cur_t);
 strftime(buf, 50, "%F", tmp); 
 
 string ret = buf;
 return ret;
}





string Time()
{
 time_t cur_t;
 tm *tmp;
 char buf[50];

 memset(buf, 0, sizeof(buf));

 time(&cur_t);
 tmp = localtime(&cur_t);
 strftime(buf, 50, "%X", tmp); 
 
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


