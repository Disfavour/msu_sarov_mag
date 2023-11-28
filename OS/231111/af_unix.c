#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define SADDRESS "mysocket"
#define CADDRESS "clientsocket"
#define BUFLEN 40


int main(int argc, char** argv)
{
	struct sockaddr_un party_addr, own_addr;
	int sockfd;
	int is_server;
	char buf[BUFLEN];
	int party_len;
	int quitting;

	if (argc != 2)
	{
		printf("Usage: %s client|server\n", argv[0]);
		return 1;
	}

	quitting = 1;
	is_server = !strcmp(argv[1], "server");

	memset(&own_addr, 0, sizeof(own_addr));

	own_addr.sun_family = AF_UNIX;

	strcpy(own_addr.sun_path, is_server ? SADDRESS : CADDRESS);

	if ((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0)
	{
		perror("Can't create socket\n");
		return 2;
	}

	unlink(own_addr.sun_path);

	if (bind(sockfd, (struct sockaddr*) &own_addr, sizeof(own_addr.sun_family) + strlen(own_addr.sun_path)) < 0)
	{
		perror("Can't bind socket\n");
		return 3;
	}

	// client
	if (!is_server)
	{
		memset(&party_addr, 0, sizeof(party_addr));
		party_addr.sun_family = AF_UNIX;
		strcpy(party_addr.sun_path, SADDRESS);
		printf("Type the string: ");

		while (gets(buf))
		{
			quitting = !strcmp(buf, "quit");

			if (sendto(sockfd, buf, strlen(buf) + 1, 0, (struct sockaddr*)&party_addr, sizeof(party_addr.sun_family) + strlen(SADDRESS)) != strlen(buf) + 1)
			{
				perror("Client: error writing socket\n");
				return 4;
			}

			if (recvfrom(sockfd, buf, BUFLEN, 0, NULL, 0) < 0)
			{
				perror("Client: error reading socket\n");
				return 5;
			}

			printf("Client: server answered: %s\n", buf);

			if (quitting)
			{
				break;
			}

			printf("Type the string: ");
		}
		close(sockfd);
		return 0;
	}

	// server
	while (1)
	{
		party_len = sizeof(party_addr);

		if (recvfrom(sockfd, buf, BUFLEN, 0, (struct sockaddr*)&party_addr, &party_len) < 0)
		{
			perror("Server: error reading socket\n");
			return 6;
		}

		printf("Server: received from client: %s\n", buf);

		quitting = !strcmp(buf, "quit");

		if (quitting)
		{
			strcpy(buf, "quitting now!");
		}
		else
		{
			if (!strcmp(buf, "ping!"))
			{
				strcpy(buf, "pong!");
			}
			else
			{
				strcpy(buf, "wrong string!");
			}
		}

		if (sendto(sockfd, buf, strlen(buf) + 1, 0, (struct sockaddr*)&party_addr, party_len) != strlen(buf) + 1)
		{
			perror("Server: error writing socket\n");
			return 7;
		}

		if (quitting)
		{
			break;
		}
	}

	close(sockfd);

	return 0;
}
