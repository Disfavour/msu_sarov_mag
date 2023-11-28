task: server.c client.c 
	gcc server.c -o server
	gcc client.c -o client

	./client 5 & ./client 3 & ./client 4 & ./client 1 & ./server

clean:
	rm server client