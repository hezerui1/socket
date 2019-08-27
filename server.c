#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/io.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "book.h"

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 9999
#define BACKLOG     1
#define MSG_SIZE    sizeof(struct message_t)

pthread_rwlock_t rwlock;

struct message_t{
	int cmd;
	char name[NAME_SIZE];
	char author[AUTHOR_SIZE];
	char intro[INTRO_SIZE];
	char _name[NAME_SIZE];
};


void *child_thread(void *ptr)
{
	int connfd = *(int*)ptr;
	char recv_buff[MSG_SIZE];
	char send_buff[MSG_SIZE];
	int ret = 0;
	while(1)
	{
		struct message_t *msg = (struct message_t*)malloc(sizeof(struct message_t));
		ret = recv(connfd, recv_buff, MSG_SIZE, 0);
		if(ret == 0)
		{
			printf("client %d disconnected\n", connfd);
			free(msg);
			msg = NULL;
			close(connfd);
			pthread_exit((void*)0);
		}
		else if(ret < 0)
		{
			printf("client %d recv error\n", connfd);
			free(msg);
			msg = NULL;
			close(connfd);
			pthread_exit((void*)0);
		}
		memcpy(msg, recv_buff, sizeof(struct message_t));
		switch(msg->cmd)
		{
		case 1:
			pthread_rwlock_wrlock(&rwlock);
			if((ret = add_book(msg->name, msg->author, msg->intro)) < 0)
				msg->cmd = ret;
			break;
		case 2:
			pthread_rwlock_wrlock(&rwlock);
			if((ret = del_book(msg->name)) < 0)
				msg->cmd = ret;
			break;
		case 3:
			pthread_rwlock_wrlock(&rwlock);
			if((ret = edt_book(msg->_name, msg->name, msg->author, msg->intro)) < 0)
				msg->cmd = ret;
			break;
		case 4:
			pthread_rwlock_rdlock(&rwlock);
			if((ret = sch_book(msg->name, msg->author, msg->intro)) < 0)
				msg->cmd = ret;
			break;
		default: break;
		}
		pthread_rwlock_unlock(&rwlock);
		write_book();
		memcpy(send_buff, msg, sizeof(struct message_t));
		send(connfd, send_buff, MSG_SIZE, 0);
	}
}

int main(void)
{
	struct sockaddr_in server;
	struct sockaddr_in client;
	int sockfd, connfd;
	int client_len = sizeof(client);
	int opt = SO_REUSEADDR;
	
	load_book();

	pthread_t thread;

	pthread_rwlock_init(&rwlock, NULL);

	memset(&server, 0, sizeof(server));
	memset(&client, 0, sizeof(client));
	server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(SERVER_ADDR); 
    server.sin_port = htons(SERVER_PORT); 

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("socket failed\n");
		exit(1);
	}
	printf("server sockfd is %d\n", sockfd);
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));	

	if(bind(sockfd, (struct sockaddr*)&server, sizeof(server)) < 0)
	{
		printf("bind failed\n");
		exit(1);
	}

	if(listen(sockfd, BACKLOG) < 0)
	{
		printf("listen failed\n");
		exit(1);
	}

	while(1)
	{
		if((connfd = accept(sockfd, (struct sockaddr*)&client, &client_len)) < 0)
		{
			printf("accept failed\n");
			exit(1);
		}
		printf("got a connection from %s:%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port));
		pthread_create(&thread, NULL, child_thread, (void*)&connfd);
	}

	close(sockfd);
	printf("socket closed\n");
	return 0;
}

