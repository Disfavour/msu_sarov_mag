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


	if (connect(sockfd, (struct sockaddr*)&own_addr, sizeof(own_addr)) < 0)
	{
		perror("Can't listen socket\n");
		return 3;
	}

	//printf("Type the string: ");

	send(sockfd, "hello123", 9, 0);


	shutdown(sockfd, 1);
	close(sockfd);

	return 0;

}
