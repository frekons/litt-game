
#include <stdio.h>
#include <locale.h> 
#include <stdbool.h>

#include "Utils.h"
#include "Database.h"
#include "Functions.h"


int main()
{
	setlocale(LC_ALL, "Turkish");
		


	while (true)
	{
		system("cls");
		printf("Kütüphane\n");

		printf("1- Yeni kitap ekle\n");
		printf("2- Var olan kitap bilgilerini deðiþtir\n");
		printf("3- Kitap ver\n");
		printf("4- Kitap geri al\n");
		printf("5- Kitap sil\n");

		int process;
		scanf(" %d", &process);
		
		switch (process)
		{
		case 1:
			add_book_processes();
			break;

		case 2:
			change_book_processes();
			break;

		case 3:
			borrow_book_processes();
			break;

		case 4:
			takeback_book_processes();
			break;

		case 5:
			remove_book_processes();
			break;

		}
	}


	return 0;
}
