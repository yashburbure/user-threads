OBJS=\
	main.c\
	thread.c\
	timer.c\
	mutexlock.c\
	spinlock.c\
	sleeplock.c\

FLAGS=\
	-Wall -g\

main:${OBJS}
	gcc ${OBJS} ${FLAGS} -o main 

clean:
	rm *.o
