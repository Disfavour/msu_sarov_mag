#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORTNUM 8080
#define BACKLOG 5
#define BUFLEN 80
#define FNFSTR "404 Error File NotFound"
#define BRSTR "Bad Request"


int main(int argc, char** argv)
{
	struct sockaddr_in own_addr, party_addr;
	int sockfd, newsockfd, filefd;
	int party_len;
	char buf[BUFLEN];
	int len;
	int i;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("Can't create socket\n");
		return 1;
	}

	memset(&own_addr, 0, sizeof(own_addr));
	own_addr.sin_family = AF_INET;
	own_addr.sin_addr.s_addr = INADDR_ANY;
	own_addr.sin_port = htons(PORTNUM);

	if (bind(sockfd, (struct sockaddr*)&own_addr, sizeof(own_addr)) < 0)
	{
		perror("Can't bind socket\n");
		return 2;
	}

	if (listen(sockfd, BACKLOG) < 0)
	{
		perror("Can't listen socket\n");
		return 3;
	}

	while (1)
	{
		memset(&party_addr, 0, sizeof(party_addr));
		party_len = sizeof(party_addr);

		if ((newsockfd = accept(sockfd, (struct sockaddr*)&party_addr, &party_len)) < 0)
		{
			perror("Error accepting connection\n");
			return 4;
		}

		if (!fork())
		{
			close(sockfd);

			if ((len = recv(newsockfd, buf, BUFLEN, 0)) < 0)
			{
				perror("Error reading socket\n");
				return 5;
			}

			printf("Received: %s\n", buf);

			if (strncmp(buf, "GET /", 5))
			{
				if (send(newsockfd, BRSTR, strlen(BRSTR) + 1, 0) != strlen(BRSTR) + 1)
				{
					perror("Error writing socket\n");
					return 6;
				}
			}

			shutdown(newsockfd, 1);
			close(newsockfd);

			return 0;
		}

		for (int i = 5; buf[i] && buf[i] > ' '; ++i)
		{
			buf[i] = 0;
		}

		if ((filefd = open(buf+5, O_RDONLY)) < 0)
		{
			send(newsockfd, FNFSTR, strlen(FNFSTR) + 1, 0);

			shutdown(newsockfd, 1);
			close(newsockfd);

			return 8;
		}

		while (len = read(filefd, buf, BUFLEN))
		{
			if (send(newsockfd, buf, len, 0) < 0)
			{
				perror("Error writing socket\n");
				return 9;
			}

			close(filefd);
			shutdown(newsockfd, 1);
			close(newsockfd);

			return 0;
		}

		close(newsockfd);
	}

	return 0;
}
