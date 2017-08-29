


TARGET = demo kdb krpcserver krpcclient
OBJECTS = ksocket.o kdb.o krpcserver.o krpcclient.o


MYSQL_CFLAGS = $(shell mysql_config --cflags)
MYSQL_LIBS = $(shell mysql_config --libs)

PKG_CFLAGS = $(shell pkg-config --cflags openssl jansson libuv) 
PKG_LIBS = $(shell pkg-config --libs openssl jansson libuv)


CFLAGS = -c -Werror -Wall -g $(MYSQL_CFLAGS) $(PKG_CFLAGS)
LDFLAGS =   -lm $(MYSQL_LIBS) $(PKG_LIBS)

CC = gcc

%.o : %.c 
	$(CC) $(CFLAGS) $< -o $@

all : $(TARGET)

demo : $(OBJECTS) demo.o
	gcc -o demo $(LDFLAGS) $(OBJECTS) demo.o

kdb: $(OBJECTS) main.o
	gcc -o kdb $(LDFLAGS) $(OBJECTS) main.o

krpcserver : $(OBJECTS) krpcserver_demo.o
	gcc -o demo $(LDFLAGS) $(OBJECTS) krpcserver_demo.o

krpcclient: $(OBJECTS) krpcclient_demo.o
	gcc -o kdb $(LDFLAGS) $(OBJECTS) krpcclient_demo.o




clean:
	rm -rf *.o
	rm -rf $(TARGET)
