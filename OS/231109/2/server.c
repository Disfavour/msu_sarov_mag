#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

int mesid;


void handler(int i)
{
	msgctl(mesid, IPC_RMID, 0);
	exit(0);
}


int main()
{
	signal(SIGINT, handler);

	struct
	{
		long mestype;
		char mes[100];
	} messageto;

	struct
	{
		long mestype;
		long mes;
	} messagefrom;

	key_t key;
	

	key = ftok("example", 'r');
	mesid = msgget(key, 0666 | IPC_CREAT | IPC_EXCL);

	while(1)
	{
		msgrcv(mesid, &messagefrom, sizeof(messagefrom) - sizeof(long), 1, 0);
		messageto.mestype = messagefrom.mes;
		sprintf(messageto.mes, "Message for client %ld\n", messagefrom.mes);
		msgsnd(mesid, &messageto, sizeof(messageto) - sizeof(long), 0);
	}

	return 0;
}