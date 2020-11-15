#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <dirent.h>
#include <aio.h>
#include <fcntl.h>
#include <syslog.h>

/**********************************************************************
 * long find_size(char*)
 *
 * This function returns the size of the input file.
 * Input parameter is the file name. 
 * Returns file size in long format.
 **********************************************************************/

long find_size(char* file_name)
{
  long k;
  struct stat st;
  stat(file_name, &st);
  k = st.st_size;

  return k;
}

/**********************************************************************
 * char* read_file(long,char*,char*)
 *
 * This function reads file asynchronously and stores it in a string.
 * Input parameter are the file string to which the data is read and the 
 * name of the input file.  
 * Returns the read data in string format.
 **********************************************************************/

char* read_file(long file_size,char* file_str, char* file_nm)
{
  struct aiocb cb;
  int numBytes;

  int fo = open(file_nm, O_RDONLY, 0);
	
  if (fo == -1)
  {
    syslog(LOG_ERR | LOG_USER,"Unable to open file!");
    printf("Unable to open file!");
    exit(0);
  }

  /*create the control block structure*/
	
  memset(&cb, 0, sizeof(struct aiocb));
 
  cb.aio_nbytes = file_size + 1;
  cb.aio_fildes = fo;
  cb.aio_offset = 0;
  cb.aio_buf = file_str;

  if (aio_read(&cb) == -1)
  {
    syslog(LOG_ERR | LOG_USER,"Unable to create request.");
    perror("Unable to create request ");
    close(fo);
  }
	
  syslog(LOG_INFO,"Request queued!.");	
  printf("\n Request queued!");
  
  /*wait until the request has finished*/
  while(aio_error(&cb) == EINPROGRESS)
  {
    printf("\n Working...");
  }
	
  /*success?*/
  numBytes = aio_return(&cb);

  if (numBytes != -1)
  {
    syslog(LOG_INFO,"Success!.");
    printf("\n Success!\n");
  }
  else
  {
    syslog(LOG_ERR | LOG_USER,"Error!");
    perror("Error!");
  }
 
  close(fo);

  return file_str;
}

/**********************************************************************
 * char* write_file(long,char*,char*)
 *
 * This function writes data asynchronously from a string to a file.
 * Input parameter are the file string to which the data is read and the 
 * name of the input file.  
 * Returns void.
 **********************************************************************/

void write_file(long file_size, char* file_str, char* file_nm)
{
  struct aiocb cb;
  int numBytes;

  int fo = open(file_nm, O_WRONLY, S_IWRITE);
	
  if (fo == -1)
  {
    syslog(LOG_ERR | LOG_USER,"Unable to open file!");
    printf("Unable to open file!");
    exit(0);
  }

  
  memset(&cb, 0, sizeof(struct aiocb));
 
  cb.aio_nbytes = file_size + 1;
  cb.aio_fildes = fo;
  cb.aio_offset = 0;
  cb.aio_buf = file_str;

  if (aio_write(&cb) == -1)
  {
    syslog(LOG_ERR | LOG_USER,"Unable to create request.");
    perror("Unable to create request. ");
    close(fo);
  }

  syslog(LOG_INFO,"Request queued!.");	
  printf("\n Request queued!");
  
  
  while(aio_error(&cb) == EINPROGRESS)
  {
    printf("\n Working...");
  }

  numBytes = aio_return(&cb);

  if (numBytes != -1)
  {
    syslog(LOG_INFO,"Success!.");	
    printf("\n Success!\n");
  }
  else
  {
    syslog(LOG_ERR | LOG_USER,"Error!");
    perror("Error!");
  }
 
  close(fo);

}

/**********************************************************************
 * char* find_replace_string(char*,char*,char*)
 *
 * This function finds the the desired string and replaces it.
 * Input parameters are the main string, the string to replace and lastly
 * the replacemant string. 
 * Returns the changed string.
 **********************************************************************/

char* find_replace_string(char* main_str,char* sub_str,char* rep_str)
{
  char* first_part;
  char* last_part;
  char* sub_string_pos;
  char* changed_string;

  sub_string_pos = strstr(main_str, sub_str);
  
  first_part = (char*)malloc( (sub_string_pos - main_str) + 1 );

  strncpy(first_part, main_str,( sub_string_pos - main_str ) );
  /*first_part now contains the string from start till match is found.*/

  first_part[sub_string_pos - main_str] = '\0';
  
  strcat(first_part,rep_str);

  last_part = sub_string_pos + strlen(sub_str);
  
  strcat(first_part, last_part);  

  changed_string = first_part;

  return changed_string;
}



int main(int argc, char* argv[])
{
  long file_size;
  char* file_string;
  char* file_name = "input.txt";
  char* changed_string;
  char* read_string;

  file_size = find_size(file_name);
  file_string = (char*)malloc(file_size+1); 
  
  read_string = read_file(file_size, file_string, file_name);
  read_string[file_size] = '\0';
  changed_string = find_replace_string(read_string, "Branch", "Anisul Huq");
  puts(changed_string); 
  write_file(file_size, changed_string, "asyn_ouput.txt");
  /*puts(changed_string);*/ 
  /*puts(read_string);*/

  return 0;	
}



