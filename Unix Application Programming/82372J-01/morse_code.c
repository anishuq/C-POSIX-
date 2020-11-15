#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <dirent.h>
#include <syslog.h>
#include "morse_lib.h"

/**********************************************************************
 * void file_name_missing()
 *
 * this signal is handled if the first file name is missing.
 * Input parameter is the SIGNAL. Returns void.
 **********************************************************************/
void file_name_missing(int sig)
{
  printf("\nChild cannot find file!! \n");
  syslog(LOG_ERR | LOG_USER,"Child cannot find file!! \n"); 
  exit(1);
}



/**********************************************************************
 * void do_child(int[],int[],char*) 
 *
 * this function converts each character into equivalent morse code and
 * then child sends morse code to parent using pipes.
 *
 * Input parameter are two pipes; one for data and another for 
 * transferring the size of the data. The last param is the input file name.
 * Returns void.
 **********************************************************************/

void do_child(int data_pipe[],int size_of_data_pipe[],char* file_name) 
{
  char ch,buf[100];
  int num = 0;
  int found = 0;
  int j;

  char* err_code = "········ ";
  struct morse_code
  {
    char letter, morse[10];
  };
    
  struct morse_code morse_code_array[] =
                                       {
                                         { 'A', ".- "   },{ 'B', "-... " },{ 'C', "-.-. " },{ 'D', "-.. "  },
                                         { 'E', ". "    },{ 'F', "..-. " },{ 'G', "--. "  },{ 'H', ".... " },
                                         { 'I', ".. "   },{ 'J', ".--- " },{ 'K', ".-.- " },{ 'L', ".-.. " },
                                         { 'M', "-- "   },{ 'N', "-. "   },{ 'O', "--- "  },{ 'P', ".--. " },
                                         { 'Q', "--.- " },{ 'R', ".-. "  },{ 'S', "... "  },{ 'T', "- "    },
                                         { 'U', "..- "  },{ 'V', "...- " },{ 'W', ".-- "  },{ 'X', "-..- " },
                                         { 'Y', "-.-- " },{ 'Z', "--.. " },{ ' ', "  "    },{ '\n', "--..---" },
                                        /*here "." means 0 and "-" means 1.
                                          every character is encoded to morse individually. Thats why there is a space after
                                          every encoded character. Actual space is represented by double space.*/
                                       };


  FILE *fp;
  fp = fopen(file_name,"r");

  if(fp == NULL)
  {
    kill(getppid(),SIGALRM);
  }
  else
  {
    close(data_pipe[0]);/*close the reading end of the pipes.*/
    close(size_of_data_pipe[0]);

    while(1)/*go through the whole file for each character*/
    {/*1*/
      ch = fgetc(fp);
      if(ch == EOF)
      {
        break;
      }
      
      for ( j = 0; j < (int)(sizeof(morse_code_array) / sizeof(struct morse_code)); j++ )/* this for loop through the whole array for every character.*/
      {/*3*/
        if ( toupper(ch) == morse_code_array[j].letter )
        {
          /*printf("%c",ch);*/
          write(data_pipe[1],morse_code_array[j].morse,strlen(morse_code_array[j].morse)); 
          num = num + strlen(morse_code_array[j].morse);   
          found = 1;
          break;
        }
      }/*3*/
        
      if(found == 0)
      {
        /*printf("%c",ch);*/
        write(data_pipe[1],err_code,strlen(err_code)); 
        num = num + strlen(err_code);   
      }
      found = 0; 
    }/*1*/

    sprintf(buf, "%d", num );/*num to string conversion.*/
    /*printf("\n size in child: %s \n",buf);*/
    write(size_of_data_pipe[1],buf,strlen(buf));   
    close(size_of_data_pipe[1]);

    close(data_pipe[1]);
   }
}

/**********************************************************************
 * void do_parent(int[],int[],char*, char*) 
 *
 * This function reads the data sent by pipes from child and writes it 
 * to a file.
 *
 * Input parameter are two pipes; one for data and another for 
 * transferring the size of the data. The last param is the output file name.
 * Returns void.
 **********************************************************************/

void do_parent(int data_pipe[],int size_of_data_pipe[],char* input_file_name, char* output_file_name)
{
  int num = 0;
  char test[100];
  char* str_p;
  /*char ch;*/
  FILE *fp, *fo;
  fp = fopen(input_file_name,"r");

  close(size_of_data_pipe[1]);
  close(data_pipe[1]);/*close the writing end of the file.*/

  read(size_of_data_pipe[0], test, 100);
  num = atoi(test);
  /* printf("\n size in parent: %d \n",num);*/
  close(size_of_data_pipe[0]);
  
  str_p = (char*)malloc(num+1);
  read(data_pipe[0], str_p, num+1);
  /*printf("\nhe he  %s \n",str_p);*/
   
  fo = fopen(output_file_name,"w");
  fprintf(fo,"%s",str_p);
  close(data_pipe[0]);
}

/* and the main function. */
int main(int argc, char* argv[])
{
    int data_pipe[2]; /* an array to store the file descriptors of the pipe. */
    int size_of_data_pipe[2];

    int pid;       /* pid of child process, or 0, as returned via fork.    */

    int data_pipe_status;        
    int size_of_data_pipe_status;

    /* first, create pipes. */
    data_pipe_status = pipe(data_pipe);

    if (data_pipe_status == -1) 
    {
	perror("pipe");
        syslog(LOG_ERR | LOG_USER,"Error in creating pipe. \n");  
	exit(1);
    }

    size_of_data_pipe_status = pipe(size_of_data_pipe);

    if (size_of_data_pipe_status == -1) 
    {
	perror("pipe");
        syslog(LOG_ERR | LOG_USER,"Error in creating pipe. \n");   
	exit(1);
    }


    /* now fork off a child process, and set their handling routines. */
    pid = fork();

    switch (pid) 
    {
      case -1:	/* fork failed. */
	       perror("fork");
               syslog(LOG_ERR | LOG_USER,"Error in process creation. \n");
	       exit(1);
      case 0:		/* inside child process.  */
              sleep(1);   /* so that parent can register signal first. */
              do_child(data_pipe,size_of_data_pipe,argv[1]);
              exit(0);
	      
      default:	/* inside parent process. */
	       /*printf("\n first we register signal and then we sleep!! \n");*/ 
               (void)signal(SIGALRM,file_name_missing);
            
               sleep(5);   /* so that child process starts executing. */

               do_parent(data_pipe,size_of_data_pipe,argv[1],argv[2]);
               wait(NULL);
               exit(EXIT_SUCCESS);
    }

    return 0;	
}


