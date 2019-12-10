#include "Functions.h"

#include "Database.h"



void add_book_processes()
{
	system("cls");
	book book;
	book.id = last_books_id() + 1;
	book.name = (char*)calloc(MAX_BOOK_NAME_LEN + 1, sizeof(char));

	printf("name\n");
	scanf(" %[^\n]", book.name);
	
	if (add_book(book))
		printf("Successfully added the book. Press any key to back mainmenu\n");
	else
		printf("Couldn't add the book, this id is already used. Press any key to back mainmenu\n");

	wait_after_input();
}

void remove_book_processes() // TO DO
{

}

void change_book_processes() // TO DO
{

}

void borrow_book_processes() // TO DO
{

}

void takeback_book_processes() // TO DO
{

}

