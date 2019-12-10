#pragma once

#ifndef DATABASE_H
#define DATABASE_H

#include <stdlib.h>
#include <stdio.h> 
#include <string.h>
#include <ctype.h>
#include <stdbool.h>


#include "Utils.h"

#define DB_FILE "db.txt"

#define MAX_STR_LEN 4096
#define MAX_BOOK_NAME_LEN 255


struct user_struct { // kütüphanelerden 1 haftalýðýna vs. kitap ödünç alýnýyor, onun kaydýný tutmak için,  TO DO

	int book_id;

	char* name, *surname;

	int taken_time, expire_time;

};

typedef struct user_struct user;

struct book_struct {

	int id;

	char* name;
	
};

typedef struct book_struct book;



int last_books_id();


int book_count();


book get_book_by_id(int id);


void add_book_to_stream(book book, FILE* stream);


bool add_book(book book);


bool remove_book(book book);



#endif // !DATABASE_H
