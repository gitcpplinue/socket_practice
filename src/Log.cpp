#include "Log.h"
#include "Timer.h"

Log::Log()
{
 m_file = 0;
}


Log::~Log()
{
 Log::Close();
}

bool Log::Open(const char *filename, const char *mode)
{
 Close();
 
 string filedir;
 string fname = filename;

 char *p = strrchr(filename, '/');
 if(p != NULL) // 如果文件名使用了绝对路径或相对路径
 {
  *(p + 1) = '\0';
  filedir = filename; // 存储文件的前置目录
 
  MKDIR(filedir.c_str(), 0755); // 递归创建前置目录
 }

 m_file = fopen(fname, mode);
 if(m_file == NULL)
 {
  perror("fopen");
  return false;
 }

 return true;
}

bool Log::Write(const char *fmt, ...)
{
 string date;

 va_list va;
 va_start(va, fmt);

 date = LocalTime();

 fprintf(m_file, "%s ", date.c_str());
 vfprintf(m_file, fmt, va);

 va_end(va);
}

void Log::Close()
{
 if(m_file != 0)
 { 
  fclose(m_file);
  m_file = 0;
 }
}

int MKDIR(const char *pathname, mode_t mode)
{
 string tmp;
 string path = pathname;
 int length = strlen(pathname);

 for(int i = 1; i < length; ++i)
 {
  if(path[i] != '/')
   continue;
  
  tmp = path.substr(0, i);

  if(access(tmp.c_str(), F_OK) == 0)
   continue;
     
  if(mkdir(tmp, mode) == -1)
  {
   perror("mkdir");
   return -1;
  }
 }
 
 return 0;
}


