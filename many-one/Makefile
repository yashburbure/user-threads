main:main.o thread.o
	gcc main.o thread.o -o main

main.o:main.c
	gcc main.c -c

thread.o:thread.c
	gcc thread.c -c

clean:
	rm *.o