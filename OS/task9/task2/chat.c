#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <poll.h>


#define MAX_QUEUE 5
#define N 100
#define MAX_CLIENTS 5


int main(int argc, char** argv)
{
    if(argc != 2)
    {
        fprintf(stderr,"Use: %s port_num\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(port);

    bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress));

    listen(serverSocket, MAX_QUEUE);

    int clients = 0;

    struct pollfd* fds = malloc(sizeof(struct pollfd) * (MAX_CLIENTS + 1));

    fds[0].fd = serverSocket;
    fds[0].events = POLLIN | POLLERR | POLLPRI | POLLOUT;
    fds[0].revents = 0;

    for (int i = 1; i <= MAX_CLIENTS; ++i)
    {
        fds[i].fd = -1;
    }

    while (1)
    {
        int activity = poll(fds, MAX_CLIENTS + 1, -1);

        printf("Events = %d\n", activity);

        if(fds[0].revents)
        {
            int temp_socket = accept(serverSocket, NULL, NULL);

            clients++;

            fds[clients].fd = temp_socket;
            fds[clients].events = POLLIN | POLLERR | POLLPRI | POLLHUP;
            fds[clients].revents = 0;

            char welcome[N];
            sprintf(welcome, "Your id %d\n", clients);

            write(temp_socket, welcome, strlen(welcome));
            //shutdown(temp_socket, SHUT_WR);
            fds[0].revents=0;

            sprintf(welcome, "Client %d have joined\n", clients);
            printf("%s", welcome);
            for (int i = 1; i < MAX_CLIENTS; i++)
            {
                if (fds[i].fd != -1)
                {
                    send(fds[i].fd, welcome, strlen(welcome), 0);
                }
            }
            
        }

        for(int i = 1; i <= clients; i++)
        {
            if(fds[i].revents)
            {
                char buf[N];
                int n_read = read(fds[i].fd, buf, N);

                if(n_read == 0)
                {
                    char mes[N];
                    sprintf(mes, "Client %d have left the chat\n", i);
                    printf("%s", mes);
                    for (int j = 1; j < MAX_CLIENTS; j++)
                    {
                        if (fds[j].fd != -1)
                        {
                            send(fds[j].fd, mes, strlen(mes), 0);
                        }
                    }

                    shutdown(fds[i].fd, 2);
                    close(fds[i].fd);
                    fds[i].fd = -1;
                }

                // if(n_read == -1){
                //     fprintf(stderr, "%s (%d): Ошибка при чтении из сокета: %s\n",
                //             __FILE__, __LINE__ - 2,  strerror(errno));
                //     close(fds[i].fd);
                //     fds[i].fd = -1;
                // }

                if(n_read > 0)
                {
                    buf[n_read]='\0';

                    char mes[N];
                    mes[0] = '\0';

                    if (strncmp(buf, "\\users", strlen("\\users")) == 0)
                    {
                        char tmp[N];
                        sprintf(tmp, "Online: ");
                        strcat(mes, tmp);
                        for (int j = 1; j < MAX_CLIENTS; j++)
                        {
                            if (fds[j].fd != -1)
                            {
                                sprintf(tmp, "Client %d ", j);
                                strcat(mes, tmp);
                            }
                        }
                        strcat(mes, "\n");

                        send(fds[i].fd, mes, strlen(mes), 0);
                    }
                    else if (strncmp(buf, "\\quit ", strlen("\\quit ")) == 0)
                    {
                        shutdown(fds[i].fd, 2);
                        close(fds[i].fd);
                        fds[i].fd = -1;

                        char tmp[N];

                        sprintf(tmp, "Last message from Client %d:", i);
                        strcat(mes, tmp);
                        strcat(mes, buf+strlen("\\quit "));

                        printf("%s", mes);
                        for (int j = 1; j < MAX_CLIENTS; j++)
                        {
                            if (fds[j].fd != -1)
                            {
                                send(fds[j].fd, mes, strlen(mes), 0);
                            }
                        }

                        //client left
                        sprintf(mes, "Client %d have left the chat\n", i);
                        printf("%s", mes);
                        for (int j = 1; j < MAX_CLIENTS; j++)
                        {
                            if (fds[j].fd != -1)
                            {
                                send(fds[j].fd, mes, strlen(mes), 0);
                            }
                        }

                        shutdown(fds[i].fd, 2);
                        close(fds[i].fd);
                        fds[i].fd = -1;
                    }
                    else
                    {
                        sprintf(mes, "Client %d:", i);
                        strcat(mes, buf);

                        printf("%s", mes);
                        for (int j = 1; j < MAX_CLIENTS; j++)
                        {
                            if (fds[j].fd != -1)
                            {
                                send(fds[j].fd, mes, strlen(mes), 0);
                            }
                        }
                    }
                }
            }
            fds[i].revents = 0;
        }
    }

    shutdown(serverSocket, 2);
    close(serverSocket);

    return 0;
}
