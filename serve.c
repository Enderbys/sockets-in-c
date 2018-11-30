/*
	serve.c <--> client.c

This:	Pi1 :	leds
		server
	ip:	192.168.1.69

That	Pi2 :	Webserver
		client
	ip:	192.168.1.65

	port:	49999
	Comp:	gcc serve.c -o serve
*/

#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_PENDING 5

void die(char *msg)
{
	perror(msg);
	exit(1);
}
int handleClient(int clnt_sock);
int main(int argc, char* argv[])
{
	int serv_sock;
	int clnt_sock;

	struct sockaddr_in serv_addr;
	struct sockaddr_in clnt_addr;

	unsigned short port = 49999;

	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family 		= AF_INET;
	serv_addr.sin_addr.s_addr 	= htonl(INADDR_ANY);
	serv_addr.sin_port		= htons(port);


	serv_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(serv_sock<0)die("socket() failed");

	int yes = 1;
	if( setsockopt(serv_sock,SOL_SOCKET,SO_REUSEADDR, &yes,sizeof(int)) == -1)
		die("setsockopt");

	if( bind(serv_sock,(struct sockaddr *)&serv_addr, sizeof(serv_addr)) <0)
		die("bind() failed");

	if( listen(serv_sock, MAX_PENDING) <0)
		die("listen() failed");

	do {
		printf("Listening...\n");
		unsigned int len = sizeof(clnt_addr);
		clnt_sock = accept(serv_sock,(struct sockaddr *)&clnt_addr, &len);
		if(clnt_sock < 0)die("accept() failed");
		printf("Handling client %s\n", inet_ntoa(clnt_addr.sin_addr));
	}while(handleClient(clnt_sock));
	close(serv_sock);
	printf("\nend\n");
	return 0;
}
#define BUFSIZE 100
int handleClient(int clnt_sock)
{
	char buffer[BUFSIZE];
	int recvd;
	unsigned int networkLen;
	unsigned int messageLen;
	unsigned int pid,networkPid;

	recv(clnt_sock,&networkLen,sizeof(networkLen),0);
	messageLen = ntohl(networkLen);

	recv(clnt_sock,&networkPid,sizeof(networkPid),0);
	pid=ntohl(networkPid);

	recvd = recv(clnt_sock, buffer,messageLen, 0);
		if(recvd<0)die("recv failed");

	printf("(%d) >",pid);
	for(unsigned int i=0;i<messageLen;i++)
		printf("%c",buffer[i]);
	printf("\n");
	close(clnt_sock);

	int flag=1;
	char *exit="exit";
	for(int i=0;i<4;i++)
		if(buffer[i]!=exit[i])flag=0;
	return !flag;
}
