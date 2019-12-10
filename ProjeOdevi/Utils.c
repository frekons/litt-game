#include "Utils.h"

 
char** split(char* str, char* delimiter, int* size)
{
	char** to_return = NULL;
	char* context = NULL;

	char* word = strtok_s(str, delimiter, &context);

	int i = 0;

	while (word != NULL)
	{
		to_return = (char**)realloc(to_return, (i + 1) * sizeof(char*));

		to_return[i++] = word;

		word = strtok_s(NULL, delimiter, &context);
	}

	if (size != NULL)
		*size = i;

	return to_return;
}

void wait_after_input()
{
	getchar();
	getchar();
}

void wait()
{
	getchar();
}