
all: phone.o
	gcc phone.o

phone.o: phone.c
	gcc -c phone.c

clean:
	rm *.o
	rm a.out

run: all
	./a.out
	make clean
