CC = gcc
CFLAGS = -Wall -O2 -g -lm -lpthread

matrix-cal: matrix-cal.c matrix-cal.h 
	$(CC) matrix-cal.c -o matrix-cal $(CFLAGS)
clean:
	rm -f matrix-cal *.o
