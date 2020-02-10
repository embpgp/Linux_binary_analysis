/*
 * In this case we are patching our push/ret shellcode at the first
 * byte so return_entry_start = 1, but modify this to whatever offset
 * into the shellcode your 'push $entry_point; ret' starts at. (To jmp back to original entry point)
 */


char parasite[] =/*将deadbeef打印出来，通过push字符串值到栈空间，通过系统调用write写入1*/
	"\x55\x48\x89\xe5\x50\x57\x56\x52\x48\xc7\xc7\x01\x00\x00\x00\x48\xb8\x64\x65\x61\x64\x62\x65\x65\x66\x50\x48\x89\xe6\x48\xc7\xc0\x01\x00\x00\x00\x48\xc7\xc2\x08\x00\x00\x00\x0f\x05\x5a\x5a\x5e\x5f\x58\x90\x5d"
	"\x68\x00\x00\x00\x00"      
	"\xc3";             
;
int return_entry_start = sizeof(parasite)-6;
