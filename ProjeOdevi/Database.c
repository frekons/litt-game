#include "Database.h"


int book_count() // databasede þu anda bulunan kitap sayýsý
{
	int line_count = 0;

	FILE* file = fopen(DB_FILE, "r");

	if (file == NULL)
		return 0;

	char c;
	while ( (c = getc(file)) != EOF )
		if (c == '\n')
			line_count++;

	fclose(file);

	return line_count;
}

int last_books_id()
{
	int last_id = 0;

	FILE* file = fopen(DB_FILE, "r");

	if (file == NULL)
		return 0;

	char* line = (char*)calloc(1, MAX_STR_LEN + 1);

	while (!feof(file))
		fgets(line, MAX_STR_LEN + 1, file);
	
	char** splitd = split(line, (char*)"\t", NULL);

	last_id = atoi(splitd[0]);

	free(line);

	fclose(file);

	return last_id;
}

void add_book_to_stream(book book, FILE* stream)
{
	fprintf(stream, "%d\t%s\n", book.id, book.name);
}

bool add_book(book book)
{
	if (get_book_by_id(book.id).id != -1) // eðer kitap böyle id'e sahip kitap varsa ekleme
		return false;

	FILE* file = fopen(DB_FILE, "a+");

	add_book_to_stream(book, file);

	fclose(file);

	return true;
}


bool remove_book(book book)
{
	FILE* temp_file = fopen("temp.txt", "w");
	FILE* file = fopen(DB_FILE, "r");

	if (file == NULL)
		return false;

	char* line = (char*)calloc(1, MAX_STR_LEN + 1);

	while (fgets(line, MAX_STR_LEN + 1, file))
	{
		int size;
		char** splitd = split(line, (char*)"\t", &size);

		if (atoi(splitd[0]) != book.id) // silmek istediðimiz kitap dýþýndaki bütün kitaplarý streama yazdýrýyoruz
		{
			add_book_to_stream(book, temp_file);
		}
	}

	free(line);

	fclose(temp_file);
	fclose(file);


	file = fopen(DB_FILE, "w");
	temp_file = fopen("temp.txt", "r");

	char ch;
	while ((ch = fgetc(temp_file)) != EOF)
		fputc(ch, file);


	fclose(temp_file);
	fclose(file);

	return true;
}


book parse_str(char** str) //
{
	book book;
	book.id = atoi(str[0]);
	book.name = str[1];

	return book;
}

book get_book_by_id(int id)
{
	book book; book.id = -1;

	FILE* file = fopen(DB_FILE, "r");

	if (file == NULL)
		return book;

	char* line = (char*)calloc(1, MAX_STR_LEN + 1);

	while (fgets(line, MAX_STR_LEN + 1, file))
	{
		int size;
		char** splitd = split(line, (char*)"\t", &size); // id\tbook_name

		if (atoi(splitd[0]) == id) // eðer þu anki satýrýn id'si aranan id'e eþitse
		{
			book = parse_str(splitd);
			break;
		}
	}

	free(line);

	fclose(file);

	return book;
}