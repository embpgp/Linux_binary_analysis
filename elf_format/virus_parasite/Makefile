.PHONY:clean

TARGET = virus
CC = gcc
OBJ = virus.c
$(TARGET):$(OBJ)
	$(CC) $(OBJ) -o $(TARGET) -m32 -nostdlib
	$(CC) hello.c -o hello -m32
#%.o:%.c
clean:
	rm -f  $(TARGET) hello
