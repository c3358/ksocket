


TARGET = demo kdb
OBJECTS = ksocket.o kdb.o


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




clean:
	rm -rf *.o
	rm -rf $(TARGET)
