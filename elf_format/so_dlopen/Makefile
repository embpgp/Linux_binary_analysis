all:main_s main_d main_dlp


main_dlp: main_dlp.o libpart.so libpart_dlp.so
	gcc main_dlp.c -L. -lpart -ldl -o main_dlp
main_s:main.o part.o
	gcc main.o part.o -o main_s
main_d:main.o libpart.so libpart_dlp.so
	gcc main.o -L. -lpart -o main_d
libpart.so:
	gcc --shared -fPIC part.c -o libpart.so
libpart_dlp.so:
	gcc --shared -fPIC part_dlopen.c -o libpart_dlp.so
main.o:main.c
	gcc -c main.c -o main.o
part.o:part.c
	gcc -c part.c -o part.o


clean:
	rm -f *.o *.so main_s main_d main_dlp
