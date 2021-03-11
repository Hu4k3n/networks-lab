#include<pthread.h>
#include <netdb.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/socket.h> 
#include <unistd.h>
#include<sys/select.h>
#include <arpa/inet.h>
    
char name[100];
void listen2msg(int clientSocket)
{
    
    do{
        char buffer[1024]={0};
        int valread=read( clientSocket , buffer, 1024);
        // printf("r:%d\n",valread);
        if (valread <= 0)
            continue;
        printf("%s\n",buffer);
    }while(1);
}
void sendingmsg(int clientSocket)
{
    char buffer[1024];
    do{
    scanf("%[^\n]%*c", buffer);
    int x=send(clientSocket,buffer,1024,0); 
    // printf("s:%d",x);
    }while(strcmp(buffer,"bye")!=0);
    exit(0);   
}

int main()
{
    int clientSocket;
    struct sockaddr_in serverAddr;
    memset(&serverAddr,'\0',sizeof(serverAddr));

    clientSocket=socket(AF_INET,SOCK_STREAM,0);
    serverAddr.sin_family=AF_INET;
    serverAddr.sin_port=htons(4455);
    serverAddr.sin_addr.s_addr=inet_addr("127.0.0.1");
    

    connect(clientSocket,(struct sockaddr*)&serverAddr,sizeof(serverAddr));
    
    printf("Enter name : ");

    scanf("%[^\n]%*c", name);
    int x=send(clientSocket,name,100,0); 

//recive msg
    int clientSocket_recv;
    struct sockaddr_in serverAddr2;
    memset(&serverAddr2,'\0',sizeof(serverAddr2));
    clientSocket_recv=socket(AF_INET,SOCK_STREAM,0);
    serverAddr2.sin_family=AF_INET;
    serverAddr2.sin_port=htons(4455);
    serverAddr2.sin_addr.s_addr=inet_addr("127.0.0.1");
    connect(clientSocket_recv,(struct sockaddr*)&serverAddr2,sizeof(serverAddr2));
    
    
    pthread_t sendmsg;
    pthread_t listen;
    pthread_create(&listen,NULL,listen2msg,clientSocket_recv);
    pthread_create(&sendmsg,NULL,sendingmsg,clientSocket);

    pthread_join(sendmsg,NULL);
}