#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/io.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "book.h"

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 9999


struct message_t{
	int cmd;
	char name[NAME_SIZE];
	char author[AUTHOR_SIZE];
	char intro[INTRO_SIZE];
	char _name[NAME_SIZE];
};


int socket_connect(void)
{
	struct sockaddr_in server;
	int sockfd;
	
	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(SERVER_ADDR);
	server.sin_port = htons(SERVER_PORT);

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("socket failed\n");
		exit(1);
	}
	if(connect(sockfd, (struct sockaddr*)&server, sizeof(server)) < 0)
	{
		printf("connect failed\n");
		exit(1);
	}
	return sockfd;
}

void get_book_info(char *info)
{
	char buff[100];
	fgets(buff, 100, stdin);
	if(buff[strlen(buff)-1] == '\n')
		buff[strlen(buff)-1] = '\0';
	memcpy(info, buff, 100);
}

void add_book_func(struct message_t *msg)
{
	printf("please enter book name:");
	get_book_info(msg->name);
	printf("please enter book author:");
	get_book_info(msg->author);
	printf("please enter book introduction:");
	get_book_info(msg->intro);
}

void del_book_func(struct message_t *msg)
{
	printf("please enter book name:");
	get_book_info(msg->name);
}

void edt_book_func(struct message_t *msg)
{
	printf("please enter book name:");
	get_book_info(msg->_name);
	printf("please enter new book name:");
	get_book_info(msg->name);
	printf("please enter new book author:");
	get_book_info(msg->author);
	printf("please enter new book introduction:");
	get_book_info(msg->intro);
}

void sch_book_func(struct message_t *msg)
{
	printf("please enter book name:");
	get_book_info(msg->name);
}

int main(void)
{
	int sockfd = 0;
	char buff[100];
	char send_buff[500];
	char recv_buff[500];
	while(1)
	{
		printf("***************************\n");
		printf("1.add book\n");
		printf("2.del book\n");
		printf("3.edt book\n");
		printf("4.sch book\n");
		printf("***************************\n");
		struct message_t *msg = (struct message_t*)malloc(sizeof(struct message_t));
		memset(msg, 0, sizeof(struct message_t));
		fgets(buff, 100, stdin);
		msg->cmd = atoi(buff);
		switch(msg->cmd)
		{
		case 1:  add_book_func(msg);  break;
		case 2:  del_book_func(msg);  break;
		case 3:  edt_book_func(msg);  break;
		case 4:  sch_book_func(msg);  break;
		default:
			free(msg);
			msg = NULL;
			printf("invalid input\n");
			printf("press enter to return homepage\n");
			fgets(buff, 100, stdin);
			continue;
		}
//		memcpy(send_buff, (char*)msg, sizeof(struct message_t));
		sockfd = socket_connect();
		if(send(sockfd, (char*)msg, 500, 0) < 0)
		{
			printf("send message fail\n");
			free(msg);
			msg = NULL;
			close(sockfd);
			printf("press enter to return homepage\n");
			fgets(buff, 100, stdin);
			continue;
		}
		recv(sockfd, recv_buff, 500, 0);
		memcpy(msg, recv_buff, sizeof(struct message_t));
		switch(msg->cmd)
		{
		case 1:
			printf("add book succeed\n");
			break;
		case 2:
			printf("del book succeed\n");
			break;
		case 3:
			printf("edt book succeed\n");
			break;
		case 4:
			printf("book name:%s\n", msg->name);
			printf("book author:%s\n", msg->author);
			printf("book introduction:%s\n", msg->intro);
			break;
		default: 
			printf("error\n");
			break;
		}
		
		free(msg);
		msg = NULL;
		close(sockfd);
		
		printf("press enter to return homepage\n");
		fgets(buff, 100, stdin);
	}
    return 0;
}




