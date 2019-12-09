#include <stdio.h> // basic I/O
#include <stdlib.h>
#include <sys/types.h> // standard system types
#include <netinet/in.h> // Internet address structures
#include <sys/socket.h> // socket API
#include <arpa/inet.h>
#include <netdb.h> // host to IP resolution
#include <string.h>
#include <unistd.h>
#include <iostream>
#define HOSTNAMELEN 40 // maximal host name length; can make it variable if you want
#define BUFLEN 1024 // maximum response size; can make it variable if you want

int main(int argc, char *argv[])
{
	//std::cout<<"testing"<<std::endl;	
  // define your variables here
  hostent* host_info = NULL;
  in_addr** addr_list;
  // check that there are enough parameters
  if (argc != 2)
    {
      fprintf(stderr, "Usage: mydns <hostname>\n");
      exit(-1);
    }
   // printf("%s\n",argv[1]);

  // Address resolution stage by using gethostbyname()
  host_info = gethostbyname(argv[1]);
  // Write your code here!
  if (host_info == NULL)
  {
    printf("%s\n", "This address does not exist\n");
  }
  else
  {
   printf("\t%s\n","The address is valid");
   addr_list =(in_addr**) host_info->h_addr_list;
          
   for (int i = 0; addr_list[i] != NULL ;i++)
   {
   	printf("%s%i\n","IP address ",i+1);
   	printf("\t%s\n",inet_ntoa(*addr_list[i]));
   }
  }
  // Print to standard output

  return 0;
}
