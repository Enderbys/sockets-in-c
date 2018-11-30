/*
	client.c <--> serve.c

This:	Pi2 :	Webserver
		cient
	ip:	192.168.1.65

That:	Pi1 :	Leds
		server
	ip:	192.168.1.69

	port :	49999
	Comp:	gcc client.c -o client
*/

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void die(char * msg)
{
	perror(msg);
	exit(1);
}

int main(int argc, char* argv[])
{
	char *servIP = "192.168.1.69";
	char *message=malloc(50);
	unsigned short port = 49999;

	int sock;
	struct sockaddr_in serv_addr;
	unsigned int messageLen, sentLen;

	pid_t pid=getpid();
	unsigned long networkPid;
	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family 		= AF_INET;
	serv_addr.sin_addr.s_addr	= inet_addr(servIP);
	serv_addr.sin_port		= htons(port);


	int flag;
	do {
		printf(":)>");
		fgets(message,50,stdin);
		messageLen = strlen(message);

		sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(sock<0)die("socket() failed");

		if( connect(sock,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) <0)
			die("connect() failed");


		unsigned int networkLen = htonl(messageLen);
		send(sock,&networkLen,sizeof(networkLen),0);
		networkPid=htonl(pid);
		send(sock,&networkPid,sizeof(pid),0);

		sentLen = send(sock, message, messageLen, 0);
		if( sentLen != messageLen )
			die("didn't send correct length");
		flag=1;
		char *exit="exit";
		for(int i=0;i<4;i++)
			if(message[i]!=exit[i])flag=0;
	}while(!flag);
	sleep(2);//allow server socket to clear
	close(sock);
	free(message);
	return 0;
}

