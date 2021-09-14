#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>

#include "../tool/Log.h"
#include "../tool/Timer.h"

#define ISspace(x) isspace((int)(x))
#define SERVER_STRING "Server: jdbhttpd/0.1.0\r\n"


class Http
{
private:
 int m_client;
 char m_buf[1024];
 char m_method[255];     
 char m_url[255];       
 char m_path[512];     
 int m_cgi;      
 char *m_query_string;

 Log *m_log;
 char m_intput[1024];
 char m_output[1024];
 Timer *m_timer;


public:
 Http();
 ~Http();

 void accept_request(int);
 void bad_request(int);
 void cat(int, FILE *);
 void cannot_execute(int);
 void error_die(const char *);
 void execute_cgi(int, const char *, const char *, const char *);
 int get_line(int, char *, int);
 void headers(int, const char *);
 void not_found(int);
 void serve_file(int, const char *);
 int startup(u_short *);
 void unimplemented(int);
};
