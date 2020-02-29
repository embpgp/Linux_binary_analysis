#include "part.h"

int g_int1, g_int2 = 5;
char *g_str1, *g_str2 = "xyz";
int g_dlp = 32322323;
void func_1(int i)
{
	int j;
	j = i;
	printf("dlopen_func_1: j = %d\n", j);
	printf("dlopen g_dlp: %d\n", g_dlp);
}

void func_2(char *str)
{
	printf("dlopen_func_2:str = %s\n", str);
}
