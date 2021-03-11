#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>

#define PORT 5000

#define BUFFER_SIZE 1024
#define MAX_CONN 100
#define CHUNK_SIZE 512

int main()
{
    int server_fd, conn_socket, recv_len;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\nError creating socket\n");
        return -1;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("couldn't bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((conn_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        printf("Error accepting connection!!!\n");
        return -1;
    }

    while (1)
    {
        int len = recv(conn_socket, buffer, BUFFER_SIZE, 0);
        printf("Message received: %s\n", buffer);
        if (strcmp(buffer, "Bye") == 0)
        {
            break;
        }

        if (strcmp(buffer, "GivemeyourVideo") == 0)
        {
            printf("Staring file transfer....\n");
            FILE *file = fopen("send.bin", "rb");
            fseek(file, 0L, SEEK_END);
            int size = ftell(file);
            fseek(file, 0, SEEK_SET);
            printf("Size : %d Bytes\n", size);

            char chunk[CHUNK_SIZE];
            sprintf(buffer, "%016d", size);
            send(conn_socket, buffer, 16, 0);
            int total = 0;
            while (total < size)
            {
                int chunk_size = (size - total < CHUNK_SIZE) ? size - total : CHUNK_SIZE;
                int r = fread(chunk, sizeof(char), chunk_size, file);

                int sent = send(conn_socket, chunk, chunk_size, 0);
                total += sent;
                printf("Sent: %d B\tTotal : %d KB\r", sent, total / 1000);
                fflush(stdout);
                fseek(file, sent - r, SEEK_CUR);
            }
            fclose(file);
            printf("\n");
            fflush(stdout);
            continue;
        }

        sprintf(buffer, "message");
        send(conn_socket, buffer, BUFFER_SIZE, 0);
    }
    close(conn_socket);
    close(server_fd);
    return 0;
}