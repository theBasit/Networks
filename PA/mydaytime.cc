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
#define PORT 13 // port of daytime server

int main(int argc, char *argv[])
{
  // define your variables here
  hostent* host_info = NULL;
  // check that there are enough parameters
  if (argc != 2)
    {
      fprintf(stderr, "Usage: mydaytime <hostname>\n");
      exit(-1);
    }

  // Write your code here

  host_info = gethostbyname(argv[1]);

  if (host_info == NULL)
  {
  	std::cout<<"\n\n\tThe desired serve does not exit"<<std::endl;
  }
  else
  {
  	struct sockaddr_in sa;
  	int n,sockfd;
	int len;
	char buff[BUFLEN];
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	
	if(sockfd<0)
	{
		printf("\nError in Socket\n");
		exit(0);
	}
	else
	{
		 printf("\nSocket is Opened\n");
	}

	bzero(&sa,sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(PORT);
	bcopy(host_info->h_addr,&sa.sin_addr.s_addr,host_info->h_length);
	int check = (connect(sockfd,(sockaddr*)  &sa,sizeof(sa)));
	if (check < 0)
	{
		printf("\nError in connection failed\n");
		exit(0);
	}
	else
	{
		printf("\nconnected successfully\n");
	}
	if(read(sockfd,buff,BUFLEN)<0)
	{
		printf("\nError in Reading\n");
		exit(0);
	}
	else
	{	
		printf("\nMessage Read:\n \tTime on host server: %s\n",buff);
		close(sockfd);
	}
  }
  return 0;
}
