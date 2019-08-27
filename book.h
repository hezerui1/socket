#ifndef __BOOK_H__
#define __BOOK_H__


#define NAME_SIZE   100
#define AUTHOR_SIZE 100
#define INTRO_SIZE  100

struct book_t {
	struct book_t *pre;
	struct book_t *next;
	char name[NAME_SIZE];
	char author[AUTHOR_SIZE];
	char intro[INTRO_SIZE];
};

int add_book(const char *name, const char *author, const char *intro);
int del_book(const char *name);
int edt_book(const char *name, const char *new_name, const char *new_author, const char *new_intro);
int sch_book(const char *name, char *author, char *intro);
int load_book(void);
int write_book(void);


#endif  /* __BOOK_H__ */
