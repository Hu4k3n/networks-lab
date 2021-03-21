#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define PORT 8080

int main(int argc, char * argv[]){
    

    int sockfd, newsockfd, portno, n;
    char buffer[255];
    long valread;
    char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 30\n\nWelcome to Networks Lab!\n";

    struct sockaddr_in serv_addr, cli_addr; //Gives internet address 
    socklen_t clilen; //32-bit datatype

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0){
        printf("Error opening Socket");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr)); //Clears the text

    portno = PORT;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno); //Host to network short

    if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
        printf("Binding Failed.");
    }
    
    printf("\nListening on Port %d\n\n",PORT);
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

   
    int i;

    while(1){
        if ((newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t*)&clilen))<0){
            perror("In accept");
            exit(EXIT_FAILURE);
        }
        printf("\n[+] New Client Request has been received.\n");
        char buffer[30000] = {0};
        valread = read( newsockfd , buffer, 30000);
        printf("%s\n",buffer );
        write(newsockfd , hello , strlen(hello));
        printf("The Message has been sent to the client.\n");
        close(newsockfd);
    }
    close(sockfd);
    return 0;
}
