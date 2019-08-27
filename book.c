#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "book.h"


struct book_t *head;
struct book_t *tail;

static struct book_t* sch_book_node(const char *name)
{
	if(!name || strlen(name) == 0)
		return NULL;
	struct book_t *book = head;
	while(book)
	{
		if(strcmp(name, book->name) == 0)
			return book;	
		book = book->next;
	}
	return NULL;
}

int add_book(const char *name, const char *author, const char *intro)
{
	if(!name || !author || !intro || strlen(name) == 0)
		return -1;
	if(sch_book_node(name) != NULL)
		return -2;
	struct book_t *newbook = (struct book_t*)calloc(1, sizeof(struct book_t));
	struct book_t *book = head;
	memcpy(newbook->name, name, NAME_SIZE);
	memcpy(newbook->author, author, AUTHOR_SIZE);
	memcpy(newbook->intro, intro, INTRO_SIZE);
	if(head)
	{
		newbook->pre = tail;
		tail->next = newbook;
		tail = newbook;
	}
	else
	{
		head = newbook;
		tail = newbook;
	}
	return 0;
}

int del_book(const char *name)
{
	if(!name || strlen(name) == 0)
		return -1;
	struct book_t *book;
	if((book = sch_book_node(name)) == NULL)
		return -2;
	if(book == head)
		head = head->next;
	else
		book->pre->next = book->next;
	if(book == tail)
		tail = tail->pre;
	else
		book->next->pre = book->pre;
	free(book);
	book = NULL;
	return 0;
}

int edt_book(const char *name, const char *new_name, const char *new_author, const char *new_intro)
{
	if(!name || !new_name || !new_author || !new_intro || strlen(name) == 0)
		return -1;
	struct book_t *book;
	if((book = sch_book_node(name)) == NULL)
		return -2;
	if(sch_book_node(new_name) != NULL)
		return -3;
	if(strlen(new_name) > 0)
		memcpy(book->name, new_name, sizeof(NAME_SIZE));
	if(strlen(new_author) > 0)
		memcpy(book->author, new_author, sizeof(AUTHOR_SIZE));
	if(strlen(new_intro) > 0)
		memcpy(book->intro, new_intro, sizeof(INTRO_SIZE));
	return 0;
}

int sch_book(const char *name, char *author, char *intro)
{
	if(!name || !author || !intro || strlen(name) == 0)
		return -1;
	struct book_t *book;
	if((book = sch_book_node(name)) == NULL)
		return -2;
	memcpy(author, book->author, sizeof(AUTHOR_SIZE));
	memcpy(intro, book->intro, sizeof(INTRO_SIZE));
	return 0;
}

int load_book(void)
{
	FILE * fp;
	int i=0;
	char *tmp;
	char buf[100];
	char *str[3];
	char bookname[100];
    char author[100];
    char intro[100];

	fp=fopen("bookinfo.txt","r");
	if(fp==NULL)
	{
		perror("open file");
		exit(0);
	}

	while((fgets(buf,100,fp))!=NULL)
    {
        tmp = strtok(buf, "\t"); //分割字符串
        while (tmp != NULL)
        {     
            str[i]=tmp;
            i++;
            tmp = strtok(NULL, "\t");
        }
		str[2][strlen(str[2])-1]='\0';
		if(add_book(str[0], str[1], str[2])<0)
			printf("load error!\n");
		i=0;
    }
	
	fclose(fp);
	return 0;
}

int write_book(void)
{
	FILE * fd;
	char bookname[100];
    char author[100];
    char intro[100];
	extern struct book_t *head;
	struct book_t *book = head;

    fd=fopen("bookinfo.txt","w");
	if(fd==NULL)
	{
		perror("open file");
		exit(0);
	}

	while(book)
	{
		fprintf(fd,"%s\t%s\t%s", book->name,book->author,book->intro);
		fprintf(fd,"\n");
		book = book->next;
	}
	
    fclose(fd);
	return 0;
}

















