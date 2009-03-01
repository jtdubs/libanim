libanim.so: anim.o
	gcc -shared -o libanim.so anim.o `pkg-config --libs glib-2.0`

anim.o: anim.c anim.h
	gcc -fPIC -c -o anim.o anim.c `pkg-config --cflags glib-2.0`

clean:
	rm -f anim.o libanim.so
