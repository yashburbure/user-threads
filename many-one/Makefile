main:main.o thread.o timer.o 
	gcc timer.o thread.o main.o -o main 

main.o:main.c 
	gcc main.c -c -g 

thread.o:thread.c
	gcc thread.c -c -g 

timer.o:timer.c
	gcc timer.c -c -g

clean:
	rm *.o
