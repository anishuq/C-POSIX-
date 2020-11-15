#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdint.h>
#include <string.h>
#include <syslog.h>
#define LINELENGTH 200

/**********************************************************************
 * int empty_line_detect(char* )
 *
 * This function finds detects blank line while reading from file.
 * Input parameter is a line from the input file. Returns Boolean.
 **********************************************************************/

int empty_line_detect(char* str)
{
  int i;
  int len;

  len = strlen(str);

  for(i=0;i<len;i++)
  {
     if((str[i] != ' ') && (str[i] != '\n') && (str[i] != '\t'))
     { 
       return 1; 
     }
  }
  return 0;
}


/**********************************************************************
 * int file_size(char*)
 *
 * This function finds the size of the input file using system call.
 * Input parameter is the file name. Returns file size.
 **********************************************************************/

int file_size(char* file_name)
{
  int k;
  struct stat st;
  stat(file_name, &st);
  k = (int)st.st_size;

  return k;
}

/**********************************************************************
 * char* memory_allocation(int)
 *
 * This function allocates memory dynamically for the input string
 * based on the file size. Input param is the file size.
 * Returns a string.
 **********************************************************************/

char* memory_allocation(int l)
{
  char* str_p;
  if ( ( str_p = (char*)malloc( l + 1 ) ) == NULL)
  {
    puts("Memory allocation error!");
    syslog(LOG_ERR | LOG_USER,"Memory allocation error! \n");
    exit(0);
  }

  return str_p;
}

/**********************************************************************
 * char* file_to_string(char*,char*)
 *
 * This function takes the contents of the file into the string.
 * Input param is the file name and input string.
 * Returns a string.
 **********************************************************************/

char* file_to_string(char* file_name, char* strp)
{
  char line[LINELENGTH];
  FILE *fp;
  fp = fopen(file_name, "r");  /* open file for input */

  if(!fp)
  {
    printf("An error occurred while opening the file.\n");
    syslog(LOG_ERR | LOG_USER,"An error occurred while opening the file. \n");
  }

  else
  {
    while ( fgets ( line, sizeof(line), fp ) != NULL ) /* read a line */
    {
      int k = empty_line_detect(line);
  
      if(k == 1)
      {
        strcat(strp,line);
      }
    }  
  }
  
  /*close(fp);*/

  return strp;
}



/**********************************************************************
 * void clean_code(int,char*,char*);
 *
 * This function cleans the code and writes to file.
 * Input param is the file name and input string.
 * Returns a string.
 **********************************************************************/

void clean_code(int size_of_file,char* file_name,char* str)
{
  int i,j,initial_position,final_position;
  int multi_line_comment = -1;
  int single_line_comment = -1;
  
  /* comment detection (start)*/
  for(i = 0; i < strlen(str); i++)
  {

    if( (str[i] == '/')  &&  (str[i + 1] == '*') )
    {
      initial_position = i;
      multi_line_comment = 0;
    }

    if( ( (str[i] == '*')  &&  (str[i + 1] == '/') ) &&  (multi_line_comment == 0 ) )
    {
       final_position = i + 1;
       multi_line_comment = 1;
    }

    if( (str[i] == '/')  &&  (str[i + 1] == '/') )
    {
      initial_position = i;
      single_line_comment = 0;
    }

    if(  (str[i] == '\n')  &&  ( single_line_comment == 0 ) )
    {
       final_position = i;
       single_line_comment = 1;
    }

   if( (multi_line_comment == 1) || (single_line_comment == 1) )
   {
     for(j = initial_position; j <= final_position; j++)
     {
       str[j] = ' ';
     }

     multi_line_comment = -1;
     single_line_comment = -1;
   }

  }
  /* comment detection (end)*/
  printf("%s",str);
}

void write_to_clean_file(char*file_name, char* str)
{
  char* file_to_write = strcat(file_name,".clean");
  FILE *fp;

  fp = fopen(file_to_write, "w");  /* open file for input */

  if(fp)
  {
    fprintf(fp,"%s",str);
  }

  else
  {
    printf("An error occurred while writing to the cleaned file.\n");
    syslog(LOG_ERR | LOG_USER,"An error occurred while writing to the cleaned file. \n");
  }
}


int main(int argc, char* argv[])
{
  int i;
  pid_t pid;
  int fs;
  char* str_pointer;
  
  for(i = 1; i < argc; i++)
  {
    pid = fork();
    wait(NULL);

    if( pid == 0 )
    {
      fs =  file_size(argv[i]);
      str_pointer = memory_allocation(fs);
      str_pointer = file_to_string(argv[i], str_pointer);
      clean_code(fs,argv[i],str_pointer);
      write_to_clean_file(argv[i],str_pointer);
      exit(i);
    }
 }


 return 0;
}

