main:main.o thread.o timer.o 
	gcc main.o thread.o timer.o -o main -g

thread.o:thread.c
	gcc thread.c -c -g

timer.o:timer.c
	gcc timer.c -c -g

clean:
	rm *.o