all: libanim.so test

libanim.so: anim.o
	gcc -g -shared -o libanim.so anim.o -lm `pkg-config --libs glib-2.0`

anim.o: anim.c anim.h
	gcc -g -fPIC -c -o anim.o anim.c `pkg-config --cflags glib-2.0`

test: test.c libanim.so
	gcc -g -o test test.c -L. -lanim `pkg-config --cflags --libs glib-2.0`

clean:
	rm -f anim.o libanim.so test
