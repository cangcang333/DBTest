#include <stdio.h>
#include <string.h>

int main()
{
	/* These 2 definitions of haystack have huge differences, pay attention to this!!! */
//	char *haystack = "20200205,143557,12345,333777999";
	char haystack[100] = "20200205,143557,12345,333777999";
	
	const char *needle = ",";
	char trans_date[8+1];
	int len = strlen(haystack);
	printf("haystack length is %d\n", len);
	char substr[len+1];
	char *ptr = strstr(haystack, needle);
	char *pos;
	if (NULL != ptr)
	{
		printf("%c\n", *ptr);
		printf("%ld, %ld\n", sizeof(trans_date), sizeof(substr));
		pos = ptr + 1;
		*ptr = '\0';
		/*
		strcpy(trans_date, haystack);
		strcpy(substr, pos);
		*/

		printf("haystack now: %s\n", haystack);
		strcpy(trans_date, haystack);
		strncpy(substr, pos, sizeof(substr));

		/*
		strncpy(trans_date, haystack, sizeof(trans_date));
		strncpy(substr, pos, sizeof(substr));
		*/



		printf("trans_date is %s\n", trans_date);
		printf("substr is %s\n", substr);
	}

	return 0;
}
