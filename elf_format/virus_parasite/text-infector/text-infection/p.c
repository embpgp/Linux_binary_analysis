/*
 * In this case we are patching our push/ret shellcode at the first
 * byte so return_entry_start = 1, but modify this to whatever offset
 * into the shellcode your 'push $entry_point; ret' starts at. (To jmp back to original entry point)
 */
/*
//先将下面的c程序编译成可执行文件，然后objdump -d提取print函数的地址  code码  汇编指令格式到另一个文件保存
//执行for i in $(cat $text_name |grep "^ " |cut -f2); do echo -n '\x'$i; done;echo  即可提取
#include <stdio.h>
#include <stdlib.h>
void print()
{
    __asm__(
    "push %rax \n"
    "push %rdi \n"
    "push %rsi \n"
    "push %rdx \n"
    "mov $1, %rdi \n"
    "mov $0x6665656264616564,%rax \n"
    "push %rax \n"
    "mov %rsp, %rsi \n"
    "mov $1, %rax \n"
    "mov $8, %rdx \n"
    "syscall \n"
    "pop %rdx \n"
    "pop %rdx \n"
    "pop %rsi \n"
    "pop %rdi \n"
    "pop %rax \n"
    );
}

void main()
{
    print();
    exit(0);
}
*/

char parasite[] =/*将deadbeef打印出来，通过push字符串值到栈空间，通过系统调用write写入1*/
	"\x55\x48\x89\xe5\x50\x57\x56\x52\x48\xc7\xc7\x01\x00\x00\x00\x48\xb8\x64\x65\x61\x64\x62\x65\x65\x66\x50\x48\x89\xe6\x48\xc7\xc0\x01\x00\x00\x00\x48\xc7\xc2\x08\x00\x00\x00\x0f\x05\x5a\x5a\x5e\x5f\x58\x90\x5d"
	"\x68\x00\x00\x00\x00"      
	"\xc3";             
;
int return_entry_start = sizeof(parasite)-6;
