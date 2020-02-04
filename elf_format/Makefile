.PHONY:clean

TARGET = parse_elf
CC = gcc
CFLAGS = -Wall -g 
OBJ = parse_elf.o
$(TARGET):$(OBJ)
	$(CC) $(CFLAGS) $^ -o $@
#%.o:%.c
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o $(TARGET)
