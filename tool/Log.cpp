#include "Log.h"

Log::Log()
{
 m_file = 0;
 int err = pthread_mutex_init(&m_mutex, 0);
 if(err != 0)
  printf("mutex_init(%d):%s \n", err, strerror(err));
}


Log::~Log()
{
 Close();
 pthread_mutex_destroy(&m_mutex);
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
 
  MkDir(fileDir.c_str(), 0755); // 递归创建前置目录
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
 pthread_mutex_lock(&m_mutex);

 string date;

 va_list va;
 va_start(va, fmt);

 date = LocalTime();
 // 先写入时间信息，再写入数据
 fprintf(m_file, "%s ", date.c_str());

 vfprintf(m_file, fmt, va);

 va_end(va);

 fflush(m_file);

 pthread_mutex_unlock(&m_mutex);
}

void Log::Close()
{
 if(m_file != 0)
 { 
  fclose(m_file);
  m_file = 0;
 }

}



int MkDir(const char *pathname, mode_t mode)
{
 string tmp;
 string path = pathname;
 int length = strlen(pathname);

 for(int i = 1; i < length; ++i)
 {
  if(path[i] != '/')
   continue;
   
  tmp = path.substr(0, i);

  // 如果目录存在，跳过；否则创建该目录
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


