.PHONY:clean

TARGET = ptrace
CC = gcc
OBJ = ptrace.c
$(TARGET):$(OBJ)
	$(CC) $(OBJ) -o $(TARGET) 
	$(CC) hello.c -o hello 
#%.o:%.c
clean:
	rm -f  $(TARGET) hello
