#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>


#define MAX_QUEUE 5
#define N 20


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

    while (1)
    {
        int localSocket = accept(serverSocket, NULL, NULL);

        if (!fork())
		{
			close(serverSocket);

            int number, pval = 1;
            char buf[N];

            while (1)
            {
                recv(localSocket, buf, N, 0);

                if (buf[0] == '\\')
                {
                    if (buf[1] == '+')
                    {
                        pval = atoi(buf+3);
                        strcpy(buf, "OK\n");
                        send(localSocket, buf, (int)strlen(buf), 0);
                    }
                    else if (buf[1] == '?')
                    {
                        sprintf(buf, "%d\n", pval);
                        send(localSocket, buf, (int)strlen(buf), 0);
                    }
                    else if (buf[1] == '-')
                    {
                        break;
                    }
                }
                else
                {
                    number = atoi(buf);
                    number += pval;
                    sprintf(buf, "%d\n", number);
                    send(localSocket, buf, (int)strlen(buf), 0);
                }
            }

			shutdown(localSocket, 2);
			close(localSocket);

			return 0;
		}

        close(localSocket);
    }

    return 0;
}
