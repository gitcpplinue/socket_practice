#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>

int main(int argc, char *argv[])
{

  for(int i = 0; i < 100; ++i)
    printf("%d:%s \n",i ,strerror(i) );
  

}
