main:main.o thread.o timer.o thread_signal.o
	gcc main.o thread.o timer.o thread_signal.o -o main -g

thread.o:thread.c
	gcc thread.c -c -g

timer.o:timer.c
	gcc timer.c -c -g

thread_signal.o:thread_signal.c
	gcc thread_signal.c -c -g
clean:
	rm *.o