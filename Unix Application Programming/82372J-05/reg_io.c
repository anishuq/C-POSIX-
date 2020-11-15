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
  struct stat st;
  long k;
  
  stat(file_name, &st);
  k = st.st_size;

  return k;
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
  int file_descriptor,n;
  long file_size;
  FILE *fp;
  int k = 0;
  int length;

  char file_buf[100];
  char* file_string;
  char* file_name = "input.txt";
  char* changed_string;

  file_size = find_size(file_name);
  file_string = (char*)malloc(file_size+1); 

  if( ( file_descriptor = open(file_name,O_RDWR) ) < 0)
  {
    syslog(LOG_ERR | LOG_USER,"Error opening file. \n"); 
    perror("Error opening file");
  }

  while( (n = read(file_descriptor,file_buf,sizeof(file_buf))) > 0)
  {
    strcat(file_string,file_buf);
  }

  file_string[file_size] = '\0';
  changed_string = find_replace_string(file_string, "Branch", "Anisul Huq");
  puts(changed_string);
  length = strlen(changed_string);  
  
  /*Writing to file in the good old fashined way!!!!*/
  
  fp = fopen("filex.txt","w");

  if(!fp)
  {
    syslog(LOG_ERR | LOG_USER,"Cannot open file. \n"); 
    printf("\nCannot open file.");
  }
  else
  {
    while(k < length)
    {
      if( isalpha( changed_string[k] ) || (changed_string[k] == ' ') || (changed_string[k] == '\n') || (changed_string[k] == '\t'))
      {
        fputc(changed_string[k],fp);
      } 
      k++;
    }
  } 
  return 0;	
}



