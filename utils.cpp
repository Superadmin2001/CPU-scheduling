#include "utils.h"

void getUint32(const char *str, uint32 *value)
{
	int32 t;
	printf("%s", str);
	scanf("%d", &t);

	for (; t < 0; )
	{
		printf("Invalid value!\n%s", str);
		scanf("%d", &t);
	}

	*value = t;
}

uint32 getRandomValueWithinRangeUint32(uint32 min, uint32 max)
{
	return ((rand() % (max - min + 1)) + min);
}

int32 getRandomValueWithinRangeInt32(int32 min, int32 max)
{
	return ((rand() % (max - min + 1)) + min);
}

uint32 strLength(char *str)
{	
	uint32 count;
	for (count = 0; *str; str++, count++)
		;

	return count;
}