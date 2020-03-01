#include "part.h"

int g_int1, g_int2 = 5;
char *g_str1, *g_str2 = "xyz";

void func_1(int i)
{
	int j;
	j = i;
	printf("func_1: j = %d\n", j);
}

void func_2(char *str)
{
	printf("func_2:str = %s\n", str);
}
int g_dlp_func_3(int i)
{
	if (i == 0xdead)
	{
		printf("sym bind and call by dlp\n");
		assert(0);
	}
    printf("[%s:%s:%d],i:%d\n", __FILE__, __func__, __LINE__, i);
    return i;
}

