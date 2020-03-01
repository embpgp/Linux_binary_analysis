#include "part_dlp.h"
int g_int1, g_int2 = 5;
char *g_str1, *g_str2 = "xyz";
int g_dlp = 5555;

void func_1(int i)
{
	int j;
	j = i;
	g_dlp = g_dlp_func_3(44444);

	printf("dlopen_func_1: j = %d\n", j);
	printf("dlopen g_dlp: %d\n", g_dlp);
}


int g_dlp_func_3(int i)
{
    printf("[%s:%s:%d],i:%d\n", __FILE__, __func__, __LINE__, i);
	return i;
}

void func_2(char *str)
{
	printf("dlopen_func_2:str = %s\n", str);
	
}

__attribute__((constructor))
static void dlp_init(void)
{
	g_dlp = g_dlp_func_3(0xdead);
}

