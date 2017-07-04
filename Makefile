


TARGET = demo
OBJECTS = ksocket.o demo.o



all : $(TARGET)

$(TARGET) : $(OBJECTS)
	gcc -o $(TARGET) -luv -L/usr/local/opt/libuv/lib $(OBJECTS)

ksocket.o : ksocket.c
	gcc -o ksocket.o -I/usr/local/opt/libuv/include -c -O2 ksocket.c

demo.o : demo.c
	gcc -o demo.o -I/usr/local/opt/libuv/include -c -O2 demo.c

clean:
	rm -rf *.o
	rm -rf $(TARGET)
