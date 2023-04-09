main:main.o thread.o timer.o
	gcc timer.o thread.o main.o -o main

main.o:main.c
	gcc main.c -c

thread.o:thread.c
	gcc thread.c -c

timer.o:timer.c
	gcc timer.c -c

clean:
	rm *.o
