#include "part.h"
#include <dlfcn.h>  

extern int g_int1, g_int2;
extern char *g_str1, *g_str2;
int g_dlp = 22323;
int main(int argc, char **argv)
{
	void *handle = NULL;
	void (*func_1_dlp)(int);
	int i = 0;
	char *str = "abc";
	g_int1 = 9;
	g_str1 = "defg";
	char *err = NULL;
	handle = dlopen ("./libpart_dlp.so", RTLD_LAZY);
	if (!handle) {  
        fprintf (stderr, "%s ", dlerror());  
        exit(1);  
    } 
	func_1_dlp = (void(*)(int))dlsym(handle, "func_1");
	if ((err = dlerror()) != NULL)  {  
        fprintf (stderr, "%s ", err);  
        exit(1);  
    }  
	(*func_1_dlp)(111111111);  
	func_1(i);
	func_2(str);
	printf("g_dlp:%d\n", g_dlp);
    dlclose(handle);  
	printf("Global integer is %d and %d, global string is %s and %s.\n", g_int1, g_int2, g_str1, g_str2);
	return i;
}
