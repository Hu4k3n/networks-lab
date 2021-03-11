#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <pthread.h>

char broadcaster[1024];
struct argu{
        int new_socket;
        int i;
        char name[100];
    }x[10];

int listen2client( struct argu *x1 )
{
    
    char buffer[1024] = {0}; 
    while(strcmp(buffer,"bye")!=0 )
    {

        int valread = read( x1->new_socket , buffer, 1024); 
        printf("%s : %s\n",x1->name,buffer);
        
        if(strlen(buffer)!=0)
        {    char buffer2[1024]={0};
            strcat(buffer2,x1->name );
            strcat(buffer2," : ");
            strcat(buffer2,buffer);
            strcpy(broadcaster,buffer2);
            broadcastmsg(x1);
            // printf("::%s::\n",broadcaster);
        }
        printf("<Listening for incoming messages ...> \n",x1->name);
    }
    close(x1->new_socket);
    printf("< Socket closed : %s >",x1->name);
    return 0;
}

void broadcastmsg(struct argu *x1)
{
        if(strcmp(broadcaster,"?NULL?")!=0)
        {
            for(int l=1;l<10;l+=2)
            {
                if(l-1==x1->i)
                    continue ;
                int broadcastmsg=send(x[l].new_socket,broadcaster,1024,0);
                // printf("BC %d :::: %s    %d \n",l, broadcaster,broadcastmsg);
            }
            strcpy(broadcaster,"?NULL?");
        }
    printf("<msg broadcasted>\n");
}
void broadcastmsg2(struct argu *x1)
{
        if(strcmp(broadcaster,"?NULL?")!=0)
        {
            for(int l=1;l<10;l+=2)
            {
                int broadcastmsg=send(x[l].new_socket,broadcaster,1024,0);
                // printf("BC %d :::: %s    %d \n",l, broadcaster,broadcastmsg);
            }
            strcpy(broadcaster,"?NULL?");
        }
    printf("<msg broadcasted>\n");
}

int main(int argc, char const *argv[]) 
{ 
    int server_fd, new_socket, valread; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
       
    // Creating socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                  &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( 4455 ); 
       
    if (bind(server_fd, (struct sockaddr *)&address,  
                                 sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (listen(server_fd, 10) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
    pthread_t listenclient[10];
    int i=0;

    do{

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
                       (socklen_t*)&addrlen))<0) 
    { 
        perror("accept"); 
        exit(EXIT_FAILURE); 
    }
    
    x[i].new_socket=new_socket;
    x[i].i=i;

    char name[100];
    char psswd[100];
    int nameread;
    int psswdread;
    if(i%2==0)
    {
        nameread = read( x[i].new_socket , name, 100);
        psswdread = read(x[i].new_socket,psswd,100);
        if (!(strcmp(psswd,"234513")==0))
            continue;
        strcpy(x[i].name,name);
        printf("User %s connected \n", x[i].name);
        char bc[1024]="User ";
        strcat(bc,x[i].name);
        strcat(bc," is connected ");
        strcpy(broadcaster,bc);
        broadcastmsg(&x[i]);

        char welcome[1000]="Welcome to Project COBRA \nThe Co-PIs connected are : \n";
        strcpy(broadcaster,welcome);
        broadcastmsg2(&x[i]);
        char clientnamelist[1000]={0};
        int k=1;
        for(int j=0;j<=i;j+=2)
        {

            // printf("%s\n",x[j].name);
            char no[100];
            sprintf(no, "%d", k);
            strcat(clientnamelist,no);
            strcat(clientnamelist," ");
            strcat(clientnamelist,x[j].name);
            strcat(clientnamelist,"\n");
            k++;
        }
        printf("%s",clientnamelist);
        strcpy(broadcaster,clientnamelist);
        printf("%s",broadcaster);
        // send(x[i].new_socket,clientnamelist,1000,0);
        broadcastmsg2(&x[i]);

        pthread_create(&listenclient[i], NULL, listen2client, &x[i]);
    }   
    i++;
    }while(i!=10);
    pthread_join(listenclient[i],NULL);
    return 0; 
}