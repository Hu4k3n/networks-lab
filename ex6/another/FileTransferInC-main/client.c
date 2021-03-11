#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>

#define PORT 5000
#define BUFFER_SIZE 1024
#define CHUNK_SIZE 512
#define TIME_RANGE 2

int main()
{
    int sock_fd = 0, recv_len;
    struct sockaddr_in server_address;
    char buffer[1024] = {0};

    char input[100];

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Error creating socket!!\n");
        return -1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0)
    {
        printf("Invalid address\\Address not supported\n");
        return -1;
    }

    if (connect(sock_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        printf("Connection failed!\n");
        return -1;
    }

    while (1)
    {
        scanf("%s%*c", input);
        send(sock_fd, input, BUFFER_SIZE, 0);

        if (strcmp(input, "Bye") == 0)
            break;

        if (strcmp(input, "GivemeyourVideo") == 0)
        {
            int r = recv(sock_fd, buffer, 16, 0);
            int size;
            sscanf(buffer, "%d", &size);
            printf("Size : %d\n", size);
            int total = 0;
            FILE *file = fopen("recv.bin", "wb");
            FILE *plot = fopen("plot.dat", "w");
            FILE *plotter = popen("gnuplot -persistent > /dev/null", "w");

            struct timeval prev_time, curr_time, start_time;

            gettimeofday(&prev_time, NULL);
            gettimeofday(&start_time, NULL);
            float speed = 0;
            int sample = 0;
            while (total < size)
            {
                int chunk_size = size - total < CHUNK_SIZE ? size - total : CHUNK_SIZE;
                r = recv(sock_fd, buffer, chunk_size, 0);
                total += r;
                fwrite(buffer, sizeof(char), r, file);
                gettimeofday(&curr_time, NULL);

                int seconds = curr_time.tv_sec - prev_time.tv_sec;
                int usec = curr_time.tv_usec - prev_time.tv_usec;

                float time = seconds + ((float)usec) / 1000000;
                float c_time = (curr_time.tv_sec - start_time.tv_sec) + (float)(curr_time.tv_usec - start_time.tv_usec) / 1000000;

                sample += r;

                if (time >= 0.1)
                {
                    speed = sample / (time * 1000);
                    prev_time.tv_sec = curr_time.tv_sec;
                    prev_time.tv_usec = curr_time.tv_usec;
                    sample = 0;
                    fprintf(plot, "%f %f\n", c_time, speed);
                    fflush(plot);

                    fprintf(plotter, "set title \"Transfer Speed\"\n");
                    fprintf(plotter, "set xlabel \"Time in seconds\"\n");
                    fprintf(plotter, "set ylabel \"Speed in KB/s\"\n");
                    fprintf(plotter, "set xrange [%3.3f:%3.3f]\n", c_time - TIME_RANGE, c_time);
                    fprintf(plotter, "plot \"plot.dat\" with lines\n");
                    fflush(plotter);
                }

                printf("Packet: %4d bytes | Total: %9d KB | Speed: %6.2f KB/s\r", r, total / 1000, speed);
                fflush(stdout);
            }
            fclose(file);
            fclose(plot);
            fclose(plotter);
            printf("\n");
            fflush(stdout);
            continue;
        }

        int len = recv(sock_fd, buffer, BUFFER_SIZE, 0);
        buffer[len] = 0;
        printf("Message Received: %s\n", buffer);
    }

    close(sock_fd);
}