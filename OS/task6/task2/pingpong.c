#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define MSG_SIZE 256


int mesid;
int childs[2];


typedef struct msg_buffer_ {
    long msg_type;
    char msg_text[MSG_SIZE];
} msg_buffer;


void handler(int i)
{
    kill(childs[0], SIGTERM);
    kill(childs[1], SIGTERM);

    for (int i = 0; i < 2; i++)
	{
		wait(NULL);
	}

	msgctl(mesid, IPC_RMID, NULL);
    printf("\nThe end of the game\n");
	exit(0);
}


int main()
{
	signal(SIGINT, handler);

    key_t key;

    key = ftok("pingpong.c", 'r');

    mesid = msgget(key, 0666 | IPC_CREAT | IPC_EXCL);

    if (!(childs[0] = fork()))
    {
        signal(SIGINT, SIG_DFL);

        msg_buffer msg_send, msg_recv;
        msg_send.msg_type = 1;
        strcpy(msg_send.msg_text, "ping");

        msgsnd(mesid, &msg_send, sizeof(msg_send) - sizeof(long), 0);

        while (1)
        {
            msgrcv(mesid, &msg_recv, sizeof(msg_recv) - sizeof(long), 2, 0);

            printf("%s\n", msg_recv.msg_text);
            sleep(1);

            msgsnd(mesid, &msg_send, sizeof(msg_send) - sizeof(long), 0);
        }
    }

    if (!(childs[1] = fork()))
    {
        signal(SIGINT, SIG_DFL);

        msg_buffer msg_send, msg_recv;
        msg_send.msg_type = 2;
        strcpy(msg_send.msg_text, "pong");

        while (1)
        {
            msgrcv(mesid, &msg_recv, sizeof(msg_recv) - sizeof(long), 1, 0);

            printf("%s\n", msg_recv.msg_text);
            sleep(1);

            msgsnd(mesid, &msg_send, sizeof(msg_send) - sizeof(long), 0);
        }
    }

    printf("lasdasf\n");

    while (1);

    return 0;
}
