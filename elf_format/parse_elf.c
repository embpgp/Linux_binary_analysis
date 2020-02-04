#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <elf.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

static void print_elf_header(Elf64_Ehdr *pelfh)
{
	assert(pelfh);
	int i = 0;
	printf("Magic：");
	for (i = 0; i<EI_NIDENT; ++i)
		printf("%02x ", pelfh->e_ident[i]);
	printf("\n");
    
    printf("类别：%d\n", pelfh->e_type);
    printf("体系结构： %d\n", pelfh->e_machine);
    printf("版本：%d\n", pelfh->e_version);
    printf("入口点地址： 0x%llx\n", pelfh->e_entry);
    printf("程序头表起始偏移： 0x%llx\n", pelfh->e_phoff);
    printf("节头表起始偏移：0x%llx\n", pelfh->e_shoff);
    printf("标志：0x%x\n", pelfh->e_flags);
    printf("本头的大小：%d\n", pelfh->e_ehsize);
    printf("程序头大小：%d\n", pelfh->e_phentsize);
    printf("程序头数量：%d\n", pelfh->e_phnum);
    printf("节头大小：%d\n", pelfh->e_shentsize);
    printf("节头数量：%d\n", pelfh->e_shnum);
    printf("字符串表索引节头：%d\n", pelfh->e_shstrndx);
    
}



int main(int argc, char *argv[])
{
    int fd;
    Elf64_Ehdr *p;
    void *p_file;	
	if (argc != 2)
	{
        printf("input error, ie:%s elf_file_name\n", argv[0]);
        exit(-1);
	}

    fd = open(argv[1], O_RDONLY, 0666);
    if (fd < 0)
    {
        printf("%s open faild", argv[1]);
        exit(-2);
    }
    
    struct stat st;
    int ret = fstat(fd, &st);
    if (ret == -1)
    {
        printf("[%s:%d]get file size faild\n", __func__, __LINE__);
        goto faild;
    }
	
    int len = st.st_size;
    p_file = mmap(NULL, len, PROT_READ, MAP_SHARED, fd, 0);
    if (p_file == NULL || p_file == (void*)-1)
    {
        printf("[%s:%d]mmap faild, p_file:%p\n", __func__, __LINE__, p_file);
        goto faild;
        
    }
    
    p = (Elf32_Ehdr*)p_file;
    print_elf_header(p);
    close(fd);
    fd = -1;
    munmap(p_file, len);
	return 0;
 faild:
    close(fd);
    fd = -1;
    return -1;
}

