#include <vector>
#include <sys/socket.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
#include <iostream>
#include <fstream>
#include <dirent.h>

using namespace std;

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        printf("\n Usage: %s <port number> \n",argv[0]);
        return 1;
    } 

    int serverfd = 0;
    int clientfd = 0;
    int n = 0;
    int closed = 0;
    struct sockaddr_in serv_addr; 
    string output = "\n\n\t************************************\n\rHello! You have connected to the simple server.";
    char buff[1024*1024*3];
    char recvbuff[1024*1024*3];
    struct sockaddr_in client_address;
    DIR *serv_dir = NULL;
    DIR *client_dir = NULL;
    struct dirent *serv_file = NULL;
    struct dirent *client_file = NULL;
    vector<string> sflist,cflist;


    cout<<".. Creating Local listner socket"<<endl;
    serverfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if(serverfd < 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    const char* temp = argv[1];

    if (atoi(temp) < 0 || atoi(temp) > 65535)
    {
        printf("\n Error : Invalid port number %i", atoi(temp));
        return 1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(buff, '0', sizeof(buff)); 

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(short(atoi(temp))); 

    cout<<".. Bindin socket to port: "<<atoi(temp)<<endl;
    int b = bind(serverfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    if (b < 0)
    {
        cout<<"Binding Error: Error in binding to port: "<<atoi(temp)<<endl<<"Try again in a few seconds."<<endl;
        return 1;
    }

    cout<<".. Starting listening at the port"<<endl;
    int l = listen(serverfd, 1);

    if (l < 0)
    {
        cout<<"XX...Couldn't get socket to listen"<<endl;
    }

    int alread_present = 0;
    serv_dir = opendir("./");
    if (serv_dir != NULL)
    {
        while (serv_file = readdir(serv_dir))
        {
            alread_present = 0;
            if (strcmp(serv_file->d_name, ".") != 0 && strcmp(serv_file->d_name, "..") != 0 )
                {   
                    for (int i = 0; i < sflist.size(); i++)
                        {
                            if (sflist[i] == serv_file->d_name)    
                                {
                                    alread_present = 1;
                                }
                        }
                    if (alread_present == 0)
                    {
                        sflist.push_back(serv_file->d_name);          
                    }
                }                        
        }
        closedir(serv_dir);
    }

                int c_already_present = 0;
                client_dir = opendir("/home/students/cs471f16/n1810035/PA-2/Client");
    if (client_dir != NULL)
    {
        while (client_file = readdir(client_dir))
        {
            c_already_present = 0; 
            if (strcmp(client_file->d_name, ".") != 0 && strcmp(client_file->d_name, "..") != 0 )
                {
                    for (int i = 0; i < cflist.size(); i++)
                    {
                        if (cflist[i] == client_file->d_name)
                        {
                            c_already_present = 1;
                        }                                   
                    }
                    if (c_already_present == 0)
                    {
                        cflist.push_back(client_file->d_name);
                    }
                }                        
            // cout<<serv_file->d_name<<endl;
        }
        closedir(client_dir);
    }    
    while(1)
    {  

        if (clientfd == 0)
        {
            cout<<".. Waiting for connection"<<endl;
            clientfd = accept(serverfd, (struct sockaddr*)NULL, NULL);
            socklen_t client_addr_size = sizeof(struct sockaddr_in);
            int res = getpeername(clientfd, (struct sockaddr *)&client_address, &client_addr_size);
            cout<<"client connected to server through ip: "<<inet_ntoa(client_address.sin_addr);
            cout<<" and port: "<<ntohs(client_address.sin_port)<<endl;
            output = "\n\n\t************************************\n\rHello! You have connected to the simple server.";
            for (int i = 0; i < output.size(); i++)
            {   
                // cout<<output<<endl;
                buff[i] = output[i];
            }

            buff[output.size()] = 0xd;
            buff[output.size()+1] = 0xA;    
            write(clientfd, buff, output.size()+2); 
            memset(buff, '0', sizeof(buff));
            output = "";
        } 
        else if (clientfd != 0)
        {
            // cout<<"start of while"<<endl;
            memset(recvbuff, '0', sizeof(recvbuff));
            n = recv(clientfd, &recvbuff, sizeof(recvbuff)-1,0);
            // cout<<"recv 1"<<endl;
            string cli_mssg = "";

            for (int i = 0; recvbuff[i] != 0xd;i++)
            {
                cli_mssg = cli_mssg + recvbuff[i];
            }
            // cout<<cli_mssg<<endl;
            cout<<endl<<endl<<"\tclient said: "<<cli_mssg<<endl;
            const char* for_strncmp = cli_mssg.c_str();

            if(cli_mssg == "exit")
            {
                closed = 1;
                cout<<"\n\nClient:\n\t "<<inet_ntoa(client_address.sin_addr)<<endl<<"on port:\n\t"<<ntohs(client_address.sin_port)<<endl;
                cout<<"has disconnected!"<<endl;
                output = "Alright! See you agin soon..";
                cli_mssg = "";
            }
            else if (cli_mssg == "help")
            {
                output = "\nAll instructions are case sensitive!\n\texit: close connection\n\tList <client/server>: list files in client/server directory";
                output = output + " \n\tCREATE server <filename.ext>";
                output = output + "\n\tSEND <filename>";
                output = output + "\n\tRECEIVE <filename>";    
                cli_mssg = "";
            }
            else if (cli_mssg == "List server")
            {
                int alread_present = 0;
                serv_dir = opendir("./");
                if (serv_dir != NULL)
                {
                    while (serv_file = readdir(serv_dir))
                    {
                        alread_present = 0;
                        if (strcmp(serv_file->d_name, ".") != 0 && strcmp(serv_file->d_name, "..") != 0 )
                            {   
                                for (int i = 0; i < sflist.size(); i++)
                                    {
                                        if (sflist[i] == serv_file->d_name)    
                                            {
                                                alread_present = 1;
                                            }
                                    }
                                if (alread_present == 0)
                                {
                                    sflist.push_back(serv_file->d_name);          
                                }
                            }                        
                    }
                    closedir(serv_dir);
                    output = "\nServer File List: \n";
                    for (int i = 0; i < sflist.size(); i++)
                    {
                        output = output + sflist[i] + "\n";   
                    }
                }
                else
                {
                    perror("Error");
                    output = "Error accessing directory";
                }

            }
            else if (cli_mssg == "List client")
            {   
                int c_already_present = 0;
                client_dir = opendir("/home/students/cs471f16/n1810035/PA-2/Client");
                if (client_dir != NULL)
                {
                    while (client_file = readdir(client_dir))
                    {
                        c_already_present = 0; 
                        if (strcmp(client_file->d_name, ".") != 0 && strcmp(client_file->d_name, "..") != 0 )
                            {
                                for (int i = 0; i < cflist.size(); i++)
                                {
                                    if (cflist[i] == client_file->d_name)
                                    {
                                        c_already_present = 1;
                                    }                                   
                                }
                                if (c_already_present == 0)
                                {
                                    cflist.push_back(client_file->d_name);
                                }
                            }                        
                        // cout<<serv_file->d_name<<endl;
                    }
                    closedir(client_dir);
                    output = "\nClient File List: \n";
                    for (int i = 0; i < cflist.size(); i++)
                    {
                        output = output + cflist[i] + "\n";   
                    }
                }
                else
                {
                    perror("Error");
                    output = "Error accessing directory";
                }
            }
            else if (strncmp(for_strncmp,"CREATE server",13 ) == 0)
            {   
                string file_name = "";
                string storage_name = "";
                for (int i = 14; i < cli_mssg.size();i++)
                {
                    file_name = file_name + cli_mssg[i];
                    storage_name = storage_name + cli_mssg[i];
                }

                int exist = 0;

                for (int j = 0; j < sflist.size(); j++)
                {
                    if (sflist[j] == file_name)
                    {
                        output = "Error : file " + file_name + " Already exists";
                        exist = 1;
                    }
                }

                file_name = "./" + file_name;
                const char* tmp = file_name.c_str();
                ofstream new_file;
                if (!exist)
                {
                    new_file.open(tmp);
                }

                if (new_file.is_open())
                {   
                    // cout<<"condition met"<<endl;
                    cout<<"Successfully created " + file_name + " on the server"<<endl;
                    output = "Successfully created " + file_name + " on the server";
                    sflist.push_back(storage_name);
                    new_file.close();
                }
                else if (!exist)
                {
                    cout<<"Error : Could not make file.."<<endl;
                    output = "Error : Could not make file..";
                }
            }
            else if (strncmp(for_strncmp,"CREATE client",13) == 0)
            {
                string file_name = "";
                string storage_name = "";
                for (int i = 14; i < cli_mssg.size();i++)
                {
                    file_name = file_name + cli_mssg[i];
                    storage_name = storage_name + cli_mssg[i];
                }

                int exist = 0;

                for (int j = 0; j < cflist.size(); j++)
                {
                    if (cflist[j] == file_name)
                    {
                        output = "Error : file " + file_name + " Already exists";
                        exist = 1;
                    }
                }

                file_name = "/home/students/cs471f16/n1810035/PA-2/Client/" + file_name;
                const char* tmp = file_name.c_str();
                // mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
                // int fd = 0;
                ofstream new_file;
                if (!exist)
                {
                    // fd = creat(temp, mode);
                    new_file.open(tmp);
                }

                if (new_file.is_open())
                {   
                    // cout<<"condition met"<<endl;
                    cout<<"Successfully created " + file_name + " on client"<<endl;
                    output = "Successfully created " + file_name + " on client";
                    cflist.push_back(storage_name);
                    // close(fd);
                    new_file.close();
                }
                else if (!exist)
                {
                    cout<<"Error : Could not make file.."<<endl;
                    output = "Error : Could not make file..";
                }                
            }
            else if (strncmp(for_strncmp,"SEND ",4) == 0)
            {
                int existS = 0;
                string file_name = "";
                for (int i = 5; i < cli_mssg.size();i++)
                {
                    file_name = file_name + cli_mssg[i];
                    // storage_name = storage_name + cli_mssg[i];
                }
                for (int i = 0; i < sflist.size(); i++)
                {
                    if (sflist[i] == file_name)
                    {
                        existS = 1;
                        output = file_name + " already exist on the server";
                        cout<<"File already exist"<<endl;
                    } 
                }
                cout<<!(existS)<<" "<<existS<<endl;
                if (!existS)
                {   
                    output = "Requesting file..";
                    for (int i = 0; i < output.size(); i++)
                    {   
                        buff[i] = output[i];
                        // cout<<output[i]<<endl;
                    }

                    buff[output.size()] = 0xd;
                    buff[output.size()+1] = 0xA;
                    write(clientfd, buff, output.size()+2); 
                    memset(buff, '0', sizeof(buff));
                    output = "";
            
                    memset(recvbuff, '0', sizeof(recvbuff));
                    n = recv(clientfd, &recvbuff, sizeof(recvbuff)-1,0);
                    // cout<<"recv 2"<<endl;
                    string cli_mssg = "";
                    // cout<<"recved"<<endl;
                    for (int i = 0; i < n;i++)
                    {
                        cli_mssg = cli_mssg + recvbuff[i];
                    }
                    // cout<<"client message: "<<cli_mssg<<endl;
                    cout<<endl<<endl<<"\tclient said: "<<cli_mssg<<endl;

                    if(cli_mssg.compare("No such file"))
                    {
                        const char* temp = file_name.c_str();

                        ofstream out(temp);
                        out<<cli_mssg;
                        out.close();
                        output = file_name + " Sent to the server..";
                        sflist.push_back(file_name);
                    }
                    else
                    {
                        cout<<"Operation aborted.."<<endl;
                        output = output + "Aborting file transfer..";
                    }
                }

            }
            else if(strncmp(for_strncmp,"RECEIVE ",7) == 0)
            {
                int existS = 0;
                string file_name = "";
                for (int i = 8; i < cli_mssg.size();i++)
                {
                    file_name = file_name + cli_mssg[i];
                }
                cout<<"file name "<<file_name<<endl;

                for (int i = 0; i < sflist.size(); i++)
                {
                    if (sflist[i] == file_name)
                    {
                        existS = 1;
                    } 
                }
                std::string str;

                if (existS)
                {
                    output = output + "File found on server..sending";
                    for (int i = 0; i < output.size(); i++)
                    {   
                        buff[i] = output[i];
                    }

                    buff[output.size()] = 0xd;
                    buff[output.size()+1] = 0xA;
                    write(clientfd, buff, output.size()+2); 
                    memset(buff, '0', sizeof(buff));
                    output = "";

                    const char* name = file_name.c_str();
                    std::ifstream to_send(name);

                    to_send.seekg(0, std::ios::end);   
                    str.reserve(to_send.tellg());
                    to_send.seekg(0, std::ios::beg);

                    str.assign((std::istreambuf_iterator<char>(to_send)),
                             std::istreambuf_iterator<char>());

                    for (int i = 0; i < str.size(); i++)
                    {   
                        buff[i] = str[i];
                    }

                    buff[str.size()] = 0xd;
                    buff[str.size()+1] = 0xA;
                    write(clientfd, buff, str.size()+2); 
                    memset(buff, '0', sizeof(buff));
                    output = "";
                }
                else
                {
                    output = output + "No such file on the server.";

                    for (int i = 0; i < output.size(); i++)
                    {   
                        buff[i] = output[i];
                        // cout<<output[i]<<endl;
                    }

                    buff[output.size()] = 0xd;
                    buff[output.size()+1] = 0xA;

                    write(clientfd, buff, output.size()+2); 
                    memset(buff, '0', sizeof(buff));
                    output = "";

                }
            }
            else
            {
                output = "\nOOPS! Seems like you entered an invaid command. Type \"help\" to see legal commands\n";
            }

            if (n < 0)
            {
                cout<<"Error in recieving message."<<endl;
                return 1;
            }
        
            for (int i = 0; i < output.size(); i++)
            {   
                buff[i] = output[i];
                // cout<<output[i]<<endl;
            }

            buff[output.size()] = 0xd;
            buff[output.size()+1] = 0xA;
            
            if (closed == 1)
            {   
                closed = 0;
                write(clientfd,buff,output.size()+2);
                close(clientfd);
                clientfd = 0;
            }
            else if(strncmp(for_strncmp,"RECEIVE ",7) != 0)
            {
                write(clientfd, buff, output.size()+2); 
                // cout<<"waiting to write at the end"<<endl;
                memset(buff, '0', sizeof(buff));
                // cout<<"nope"<<endl;
                output = "";
            }
        }
        cout<<"looped"<<endl;
        // sleep(1);
     }
}