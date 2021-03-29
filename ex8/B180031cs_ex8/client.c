#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h> 
#include <sys/socket.h>
#include <pthread.h>

#include "transfer.c"
#include "checks.c"

int main(int argc, char *argv[])
{
    int port = 8080;
    char *address="127.0.0.1";

    if (argc == 2)
    {
        if (sscanf(argv[1], "%d", &port) != 1)
        {
            printf("Invalid PORT number\n");
            return -1;
        }
    }
    else if(argc == 1)
    {
        printf("Invalid PORT number\n");
        return -1;
    }


    //Socket
    int sock_fd = 0, recv_len;
    struct sockaddr_in server_address;

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Error creating socket!!\n");
        return -1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);

    if (inet_pton(AF_INET, address, &server_address.sin_addr) <= 0)
    {
        printf("Invalid address\\Address not supported\n");
        return -1;
    }

    printf("Connecting...\n");

    if (connect(sock_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        printf("Connection failed!\n");
        return -1;
    }

    printf("Successfully connected to %s:%d\n", address, port);
    
    char username[50];
    char password[50];

    printf("Login\n");
    printf("Username : ");
    scanf("%s",username);
    printf("\nPassword : ");
    scanf("%s",password);

    int length ;
    char *response;

    int x=send_packet(sock_fd,username,strlen(username));
    int y=send_packet(sock_fd,password,strlen(password));

    printf("%d %d\n",x,y);

    length=recv_packet(sock_fd,&response);
    if(strcmp(response,"Success")!=0)
    {
        printf("User not authenticated\n");
        printf("%s\n",response);
        close(sock_fd);
        return -1;
    }
    free(response);

    printf("User Authenticated");
    int choice =0;

    while(1)
    {
        printf("\n\nMail\n");
        printf("1. Send Mail\n");
        printf("2. Quit\n");
        printf("Option : ");
        scanf("%d%*c", &choice);
        printf("\n");
        switch(choice)
        {
            case 1 : 
            
            printf("Enter Details \nFor Example : \n\nFrom: stallings@localhost\nTo: andrew@localhost\nSubject: SMTP Experiment\nNext evaluation will be on 22 March 2021.\nPlease be ready.\n.\n\n");

            char from[50], to[50], subject[50], body[81];
            char buffer[80]={0};
            int count =0;

            printf("From: ");
            scanf("%s%*c", from);
            printf("To: ");
            scanf("%s%*c", to);
            printf("Subject: ");
            scanf("%[^\n]%*c", subject);

            if(!verify_email(from) || !verify_email(to))
            {
                printf("Invalid Email \n");
                continue;
            }

            while (strcmp(buffer, ".") != 0 && count < 81)
            {
                scanf("%[^\n]%*c", buffer);
                count += sprintf(body + count, "%s\n", buffer);
            }

            char data[1024];

            count = 0;
            count += sprintf(data + count, "From: %s\n", from);
            count += sprintf(data + count, "To: %s\n", to);
            count += sprintf(data + count, "Subject: %s\n", subject);
            count += sprintf(data + count, "%s\n", body);

            int x=send_packet(sock_fd, data,count);

            recv_packet(sock_fd, &response);

            if (strcmp("EMAIL SENT", response) == 0){
                printf("Email sent succesffully!\n");
                continue;
            }
            else{
                printf("Error sending email!\n");
                printf("Error: %s\n", response);
            }

            free(response);
            break;
        
            case 2:
            send_packet(sock_fd,"EXIT",strlen("EXIT"));
            close(sock_fd);
            return 1;

            default : 
            printf("Invalid");
            
        }    
    }
    close(sock_fd);
    return 0; 
}