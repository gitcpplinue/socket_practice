#include "Log.h"

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
 
 string fileDir;
 string fileName = filename;

 size_t locate = fileName.find_last_of('/');
 if(locate != -1) // 如果文件名使用了绝对路径或相对路径
 {
  // 保存路径名，保留最后的'/'
  fileDir = fileName.substr(0, locate + 1);
 
  MKDIR(fileDir.c_str(), 0755); // 递归创建前置目录
 }

 m_file = fopen(filename, mode);
 if(m_file == NULL)
 {
  perror("fopen");
  return false;
 }
 
// printf("open log file \"%s\" \n", filename);
 return true;
}

bool Log::Write(const char *fmt, ...)
{
 string date;

 va_list va;
 va_start(va, fmt);

 date = LocalTime();
 
 // 先写入时间信息，再写入数据
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
     
  if(mkdir(tmp.c_str(), mode) == -1)
  {
   perror("mkdir");
   return -1;
  }
 }
 
 return 0;
}


