main:main.o thread.o spinlock.o timer.o
	gcc timer.o spinlock.o thread.o main.o -o main

main.o:main.c
	gcc main.c -c

thread.o:thread.c
	gcc thread.c -c


spinlock.o:spinlock.c 
	gcc spinlock.c -c

timer.o:timer.c
	gcc timer.c -c

clean:
	rm *.o