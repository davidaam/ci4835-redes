all: svr_s svr_c

svr_s: svr_s.c svr_s.h
	gcc -pthread svr_s.c -o svr_s

svr_c: svr_c.c svr_c.h
	gcc svr_c.c -o svr_c

clean:
	rm *.o
