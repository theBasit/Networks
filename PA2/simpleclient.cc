#include <sys/socket.h>
#include <vector>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <iostream> 
#include <dirent.h>
#include <fstream>


int main(int argc, char *argv[])
{
    int sockfd = 0; 
    int n = 0;
 	int s = 0;
    char buff[1024*1024*3];
    char command[(1024*1024)*3];
    struct sockaddr_in server_information; 
    hostent* host_info = NULL;
    in_addr** addr_list;
    DIR *c_dir = NULL;
    struct dirent *c_file = NULL;
    

    if(argc != 3)
    {
        printf("\n Usage: %s <Domain name of server> <port number> \n",argv[0]);
        return 1;
    } 

    memset(buff, '0',sizeof(buff));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if(sockfd < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    } 

    memset(&server_information, '0', sizeof(server_information)); 

	    
    const char* temp = argv[2];

    if (atoi(temp) < 0 || atoi(temp) > 65535)
    {
    	printf("\n Error : Invalid port number %i", atoi(temp));
    	return 1;
    }

    // std::cout<<short(atoi(temp))<<std::endl;

    server_information.sin_family = AF_INET;
    server_information.sin_port = htons(short(atoi(temp))); 
    host_info = gethostbyname(argv[1]);

    std::vector<std::string> cflist;
    int alread_present = 0;
    c_dir = opendir("./");
    if (c_dir != NULL)
    {
        while (c_file = readdir(c_dir))
        {
            alread_present = 0;
            if (strcmp(c_file->d_name, ".") != 0 && strcmp(c_file->d_name, "..") != 0 )
                {   
                    for (int i = 0; i < cflist.size(); i++)
                        {
                            if (cflist[i] == c_file->d_name)    
                                {
                                    alread_present = 1;
                                }
                        }
                    if (alread_present == 0)
                    {
                        cflist.push_back(c_file->d_name);          
                    }
                }                        
        }
        closedir(c_dir);
    }

    if (host_info == NULL)
        {
            printf("%s\n", "This address does not exist\n");
            return 1;
        }

    addr_list =(in_addr**) host_info->h_addr_list;
    const char* IP = inet_ntoa(*addr_list[0]);


    if(inet_pton(AF_INET, IP, &server_information.sin_addr)<=0)
    {
        printf("\n inet_pton error occured\n");
        return 1;
    } 

    if(connect(sockfd, (struct sockaddr *)&server_information, sizeof(server_information)) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return 1;
    } 

    n = recv(sockfd, &buff, sizeof(buff)-1,0);
    for (int i = 0; i < n; i++)
    {   
        std::cout<<buff[i];
    }
    std::cout<<std::endl<<"c : ";
    
    while ( n > 0)
    {   
        // std::cout<<"start of while"<<std::endl;
        std::string response = "";
        std::string input = "";
        std::getline(std::cin,input,'\n');
        const char* for_strcnmp = input.c_str();
        // std::cout<<*for_strcnmp<<std::endl;
        for(int i = 0; i < input.size(); i++)
        {
            command[i] = input[i];
        }

        command[input.size()] = 0xd;
        command[input.size()+1] = 0xA;
	    
        s = send(sockfd, &command, input.size()+2,0);
        // std::cout<<"sent somethin 1"<<std::endl;
	    if (s < 0)
	    {
	    	printf("Error : Sending Failed \n");
	    }
        
        n = recv(sockfd, &buff, sizeof(buff)-1,0);

        for (int i = 0; i < n; i++)
        {   
            std::cout<<buff[i];
            if (buff[i] != 0xd && buff[i] != 0xa)
            response = response + buff[i];
        }
        std::cout<<std::endl;
        memset(buff, '0',sizeof(buff));


        if (input == "exit")
        {
            shutdown(sockfd,2);
            std::cout<<"Connection Closed"<<std::endl;
            return 1;
        }
        if (strncmp(for_strcnmp,"SEND ",4) == 0)
        {
            int exist = 0;
            std::string file_name = "";
            for (int i = 5; i < input.size();i++)
            {
                file_name = file_name + input[i];
            }
            for (int i = 0; i < cflist.size(); i++)
            {
                if (cflist[i] == file_name)
                {
                    exist = 1;
                }
            }
                std::string check = (file_name + " already exist on the server");
            if (response.compare(check) == 0)
            {
                exist = 2;
            }
            std::string str;
            std::cout<<exist<<std::endl;
            if (exist == 1)  
            {
                const char* name = file_name.c_str();
                std::ifstream to_send(name);

                to_send.seekg(0, std::ios::end);   
                str.reserve(to_send.tellg());
                to_send.seekg(0, std::ios::beg);

                str.assign((std::istreambuf_iterator<char>(to_send)),
                             std::istreambuf_iterator<char>());  
                
                for (int i = 0; i < str.size(); i++)
                {
                    command[i] = str[i];
                }
                command[str.size()] = 0xd;
                command[str.size() + 1] = 0xA;

                s = send(sockfd, &command, str.size()+2,0);
                n = recv(sockfd, &buff, sizeof(buff)-1,0);
                    
            }
            else if (!exist)
            {   
                std::cout<<"Client prompt: no such file "<<exist<<std::endl;
                input = "No such file";
                for(int i = 0; i < input.size(); i++)
                {
                    command[i] = input[i];
                }

                command[input.size()] = 0xd;
                command[input.size()+1] = 0xA;
                
                s = send(sockfd, &command, input.size()+2,0);
                n = recv(sockfd, &buff, sizeof(buff)-1,0);
            }
        }
        else if (strncmp(for_strcnmp,"RECEIVE ",7)==0)
        {
            int exist = 0;
            std::string file_name = "";
            for (int i = 8; i < input.size();i++)
            {
                file_name = file_name + input[i];
            }
            for (int i = 0; i < cflist.size(); i++)
            {
                if (cflist[i] == file_name)
                {
                    exist = 1;
                }
            }

            if(response.compare("No such file on the server.") == 0)
            {
                exist = 1;
            }

            if(!exist)
            {   
                std::cout<<"Begining transfer.."<<std::endl;
                std::cout<<"waiting to recv"<<std::endl;
                n = recv(sockfd, &buff, sizeof(buff)-1,0);
                response = "";
                for (int i = 0; i < n; i++)
                {   
                    std::cout<<buff[i];
                    if (buff[i] != 0xd && buff[i] != 0xa)
                    response = response + buff[i];
                } 
                memset(buff, '0',sizeof(buff));
                
                const char* temp = file_name.c_str();
                std::ofstream out(temp);
                out<<response;
                out.close();
                cflist.push_back(file_name);
            }
            if (exist)
            {
                std::string to_do = "";
                std::cout<<"The file already exist. \n\tDo you want to replace the existing file? (y/n) ";
                std::cin>> to_do;
                std::cin.clear();
                std::cin.ignore(10000, '\n');

                if (to_do.compare("n") == 0)
                {
                    n = recv(sockfd, &buff, sizeof(buff)-1,0);
                    std::cout<<"Did not replace file.."<<std::endl;
                }
                else
                {
                    std::cout<<"Begining transfer.."<<std::endl;
                    std::cout<<"waiting to recv"<<std::endl;
                    n = recv(sockfd, &buff, sizeof(buff)-1,0);
                    response = "";
                    for (int i = 0; i < n; i++)
                    {   
                        std::cout<<buff[i];
                        if (buff[i] != 0xd && buff[i] != 0xa)
                        response = response + buff[i];
                    } 
                    memset(buff, '0',sizeof(buff));
                    
                    const char* temp = file_name.c_str();
                    std::ofstream out(temp);
                    out<<response;
                    out.close();    
                }

            }
        }


        std::cout<<std::endl<<"c : ";

        memset(buff, '0',sizeof(buff));
        memset(command, '0',sizeof(command)); 
    }   
   
    if(n < 0)
    {
        printf("\n Recv error \n");
    } 
    // std::cout<<"here"<<std::endl;
    return 0;
}    