TARGET = rpcclnt rpcsevr
OBJECTS = ksocket.o

PKG_CFLAGS = $(shell pkg-config --cflags libuv protobuf) 
PKG_LIBS = $(shell pkg-config --libs libuv protobuf)


CFLAGS = -c -Werror -Wall -g  -I.. $(PKG_CFLAGS)
LDFLAGS =  -lm -lstdc++ $(MYSQL_LIBS) $(PKG_LIBS)

CC = gcc

%.o : %.c 
	$(CC) $(CFLAGS) $< -o $@

%.o : %.cpp
	$(CC) $(CFLAGS) -std=c++11 $< -o $@

%.o : %.cc
	$(CC) $(CFLAGS)  -std=c++11 $< -o $@

all : $(TARGET)

rpcclnt : ksocket.o rpc_client.o rpc_client_demo.o
	$(CC) $(LDFLAGS) -o rpcclnt ksocket.o rpc_client.o rpc_client_demo.o

rpcsevr : ksocket.o rpc_server.o rpc_server_demo.o
	$(CC) $(LDFLAGS) -o rpcsevr ksocket.o rpc_server.o rpc_server_demo.o

ksocket.o : ../ksocket.c ../ksocket.h
	$(CC) -o ksocket.o $(CFLAGS) ../ksocket.c 


clean:
	rm -rf *.o
	rm -rf $(TARGET)
