ORGS=\
	main.c\
	thread.c\
	timer.c\
	thread_signal.c\
	mutexlock.c\
	spinlock.c\
	sleeplock.c\

FLAGS=\
	-g\

main:${ORGS}
	gcc ${ORGS} ${FLAGS} -o main

clean:
	rm *.o