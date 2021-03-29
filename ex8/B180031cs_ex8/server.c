#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h> 
#include <pthread.h>

#include "transfer.c"
#include "checks.c"

#define PORT 8080

typedef struct Acc
{
    char username[50];
    char password[50];
}Acc;

typedef struct Client
{
    char *username;
    char *password;
    int sock_fd;

    Acc *acc;
    int total;
    struct Client *head;
    struct Client *next;
}Client;

//new client is used to make the node for a new client
Client *new_client(int sockfd, Client *head, Acc *acc, int total)
{
    Client *client = (Client*)malloc(sizeof(Client));
    client-> username=NULL;
    client -> password=NULL;
    client->sock_fd= sockfd;

    client->total=total;
    client->acc = acc;

    client->head=head;
    client->next = NULL;
    return client;

}

Client *init_list()
{
    Client *head=new_client(0,NULL,NULL,0);
    return head;
}
void add_client(Client *head,Client *client)
{
    Client *curr=head;
    while(curr->next)
        curr=curr->next;
    curr->next = client;
}
int remove_client(Client *head,Client *client)
{
    Client *curr = head;
    while(curr)
    {
        if(curr->next ==  client)
        {
            curr->next = client->next;
            free(client);
            return 1;
        }
        curr=curr->next;
    }
    return 0;
}
void get_time(char *time_string)
{
    time_t curr_time;
    struct tm *timeinfo;

    curr_time=time(NULL);
    timeinfo = gmtime(&curr_time);
    strftime(time_string, 100, "%a, %d %b %Y %I:%M:%S IST", timeinfo);
}
void *client_handle(void *arg)
{
    Client *client = (Client*)arg;
    printf("new client\n");
    char *username,*password;
    if(recv_packet(client->sock_fd,&username)==-1)
    {
        printf("not recieved");
        close(client->sock_fd);
        remove_client(client->head,client);
        return NULL;
    }
    if(recv_packet(client->sock_fd,&password)==-1)
    {
        printf("not recieved");
        close(client->sock_fd);
        remove_client(client->head,client);
        return NULL;
    }

    int flag=0;
    for(int i=0;i<client->total;i++)
    {
        if(strcmp(client->acc[i].username,username) == 0)
        {
            if(strcmp(client->acc[i].password,password) != 0)
            {
                send_packet(client->sock_fd,"Invalid Password",strlen("Invalid Password"));
                close(client->sock_fd);
                remove_client(client->head,client);
                return NULL;
            }
            else
            {
                flag=1;
                client->username=username;
                client->password=password;
                break;
            }
        }
    }
    printf("Flag : %d\n",flag);
    if(flag==0)
    {
        send_packet(client->sock_fd,"Invalid User",strlen("Invalid User"));
        close(client->sock_fd);
        remove_client(client->head,client);
        return NULL;
    }

    send_packet(client->sock_fd,"Success",strlen("Success"));
    printf("Client authenticated\n");

    while(1)
    {
        char *data;
        char buffer[1024];
        char send_username[50];
        int len = recv_packet(client->sock_fd,&data);
        // printf("%s\n",data);
        int i=0;
        if(len==-1)
        {
            printf("Not recieved\n");
            break;
        }
        if(strcmp(data,"EXIT") == 0)
        {
            break;
        }
        printf("New Mail !");

        char from[50],to[50], subject[50], body[1024];

        get_field(data, "From", from);
        get_field(data, "To", to);
        get_field(data, "Subject", subject);
        get_field(data, "Body", body);

        // printf("%s %s %s %s",from,to,subject,body);
    
        if(!verify_email(from))
        {
            printf("Invalid From\n");
            send_packet(client->sock_fd,"Invalid",strlen("Invalid"));
            continue;
        }

        if(!verify_email(to))
        {
            printf("Invalid To\n");
            send_packet(client->sock_fd,"Invalid",strlen("Invalid"));
            continue;
        }

        int p = has_char(to,'@');
        int rec_flag = 0;
        strncpy(send_username,to,p);
        send_username[p]=0;
        printf("%s",send_username);

        for(int i=0;i<client->total;i++)
        {
            if(strcmp(client->acc[i].username, username)==0)
                rec_flag=1;
        }

         if (!rec_flag){
            printf("To wrong %s \n", to);
            send_packet(client->sock_fd, "Invalid", strlen("Invalid"));
            continue;
        }
        printf("File creating\n");
        char mailbox_fname[100];
        sprintf(mailbox_fname, "%s/mymailbox.mail", send_username);
        FILE *mailbox = fopen(mailbox_fname, "a");

        char time_str[50];
        get_time(time_str);

        fprintf(mailbox, "From: %s\n", from);
        fprintf(mailbox, "To: %s\n", to);
        fprintf(mailbox, "Subject: %s\n", subject);
        fprintf(mailbox, "Received: %s\n", time_str);
        fprintf(mailbox, "%s", body);

        fclose(mailbox);
        printf("File created\n");

        char *succ = "EMAIL SENT";
        printf(" %s -> %s.\n", from, to);
        
        send_packet(client->sock_fd, succ, strlen(succ));

        free(data);

    }

    printf("Client %s Disconnnected\n", client->username);

    close(client->sock_fd);
    remove_client(client->head, client);
}

int main(int argc, char *argv[])
{
    int my_port=8080;
    if (argc == 2)
    {
        if (sscanf(argv[1], "%d", &my_port) != 1)
        {
            printf("Invalid PORT number\n");
            return -1;
        }
    }
    else if (argc == 1)
    {
        printf("PORT not specified\n");
    }
    else
    {
        printf("Invalid arguments\n");
        return -1;
    }

    int server_fd,conn_socket,recv_len;
    struct sockaddr_in address;
    int opt=1;
    int addrlen = sizeof(address);
    char messages[256];
    char buffer[1024]={0};
     printf("Starting SMTP server...\n");

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\nError creating socket\n");
        return -1;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        printf("Error setting socket options!\n");
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(my_port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        printf("Error binding to my_port!\n");
        return -1;
    }

    if (listen(server_fd, 3) < 0)
    {
        printf("Error while listening for connections!\n");
        return -1;
    }

    printf("Server started!\n");
    printf("Listening on 0.0.0.0:%d\n", my_port);

    //Login Checking 

    char fusername[50], fpassword[50],username[50],password[50];

    FILE *cred_file = fopen("logincred.txt", "r");
    int total=0;


    while (fscanf(cred_file, "%[^,]%*c %[^\n]%*c", fusername, fpassword) != EOF)
        total++;
    
    Acc *creditials = calloc(total,sizeof(Acc));
    fseek(cred_file, 0 ,SEEK_SET);
    int i=0;
    while (fscanf(cred_file, "%[^,]%*c %[^\n]%*c", fusername, fpassword) != EOF)
    {
        // printf("%s %s %d\n",fusername,fpassword,total);
        strcpy(creditials[i].username, fusername);
        strcpy(creditials[i].password, fpassword);
        i++;
    }

    Client *head = init_list();

    while(1)
    {
        if((conn_socket = accept(server_fd,(struct sockaddr*)&address,(socklen_t*)&addrlen))<0)
        {
            printf("Error : accept");
            continue ;
        }
        fflush(stdout);

        Client *client = new_client(conn_socket,head,creditials,total);
        add_client(head,client);
        pthread_t thread_id;
        pthread_create(&thread_id, NULL, client_handle,client);
    }
    return 0;
}