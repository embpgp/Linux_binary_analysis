.PHONY:clean

TARGET = infect 
CC = gcc
OBJ = infect.c p.c
$(TARGET):$(OBJ)
	$(CC) $(OBJ) -o $(TARGET)
	$(CC) host.c -o host -g
#%.o:%.c
clean:
	rm -f  $(TARGET) host
