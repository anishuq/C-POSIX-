#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <syslog.h>
#include "hello_lib.h"

/**********************************************************************
 * void print_char(int)
 *
 * This function (or signal handler) prints "HELLO WORLD". It first finds the current
 * working directory and then finds "hello_world.c" file. If this file is present
 * then prints "HELLO WORLD".   
 * Input parameter is the SIGNAL. Returns void.
 **********************************************************************/

void print_char(int sig)
{
  char cwd[200];

  struct dirent *de=NULL;
  DIR *d = NULL;

  d = opendir(getcwd(cwd, sizeof(cwd)));
  if(d == NULL)
  {
    perror("Couldn't open directory");
    syslog(LOG_ERR | LOG_USER,"Couldn't open directory. \n");
  }

  /* Loop while not NULL */
  while(1)
  {
    de = readdir(d);
    if(strcmp(de->d_name,"hello_world.c") == 0) /*looking for self*/
    {
      printf("\nHELLO WORLD \n");
      syslog(LOG_INFO,"HELLO WORLD is printed sucessfully.\n");
      break;
    }
   }

  closedir(d);
}


int main()
{
 pid_t pid;
 pid = fork();

 if(pid == -1)
 {
   perror("Can't create process");
   syslog(LOG_ERR | LOG_USER,"Can't create process. \n");
   exit(1);
 }

 if(pid == 0)
 {
   sleep(2);
   kill(getppid(),SIGALRM);
   exit(0);
 }
 /*parent process(start)*/
 (void)signal(SIGALRM,print_char);
 pause();/*parent will now wait until SIGALRM is received by it.*/
 wait(NULL);
 exit(0);

 return 0;
}
