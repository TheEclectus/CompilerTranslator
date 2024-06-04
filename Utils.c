#include "Utils.h"

#include <ctype.h>
#include <malloc.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>

const char* Skip(const char* Str, const char* Delims)
{
	if (Str == NULL)
		return Str;

	if (strlen(Str) == 0)
		return NULL;

	while (1)
	{
		if (strchr(Delims, *Str))
		{
			Str++;
		}
		else
			return Str;
	}
}

const char* SkipWhitespace(const char* Str)
{
	/*while (1)
	{
		if (*Str == 0)
			return NULL;
		else if (*Str == ' ' || *Str == '\t')
			Str++;
		else
			return Str;
	}*/

	return Skip(Str, " \t");
}

const char* NextWord(const char** Str, const char* Delims)
{
	const char* WordStart;
	const char* WordEnd;
	long RemainingChars;
	char* Buf;

	WordStart = Skip(*Str, Delims);
	if (WordStart == NULL)
		return NULL;


	WordEnd = strpbrk(WordStart, Delims);
	if (WordEnd == NULL)
	{
		RemainingChars = strlen(WordStart);
		Buf = (char*)malloc(sizeof(char) * (RemainingChars + 1));
		memset(Buf, 0, sizeof(char) * (RemainingChars + 1));
		memcpy(Buf, WordStart, RemainingChars);

		*Str = NULL;
		return Buf;
	}

	RemainingChars = (WordEnd - WordStart);
	Buf = (char*)malloc(sizeof(char) * (RemainingChars + 1));
	memset(Buf, 0, sizeof(char) * (RemainingChars + 1));
	memcpy(Buf, WordStart, RemainingChars);

	*Str = WordEnd;
	return Buf;
}

int StrToInt(const char* Str, int* Err)
{
	long StrLen = strlen(Str);
	int i = 0;
	for (i = 0; i < StrLen; i++)
	{
		if (i == 0 && Str[i] == '-')
			continue;
		else if (isdigit(Str[i]) == 0)
		{
			if (Err != NULL) *Err = 1;
			return 0;
		}
	}

	if (Err != NULL) *Err = 0;
	return atoi(Str);
}