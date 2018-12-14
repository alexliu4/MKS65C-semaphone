
all: phone.o control.o
	  gcc control.o -o control
		gcc phone.o

phone.o: phone.c
		gcc -c phone.c

control.o: control.c
		gcc -c control.c

clean:
	rm *.o
	rm a.out
	rm control


setup:
		./control $(args)

run: all
	./a.out
