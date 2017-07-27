#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <assert.h>
#include <unistd.h>
#include <uv.h>
#include <sys/un.h>
#include "list.h"
#include "ksocket.h"


//=======================================================================================================

//demo echo server

struct my_socket_context
{
    struct ks_socket_context socket_context;
    struct ks_circular_buffer split_buffers;
};

#define DEMO_MAGIC 0x44aa
struct my_header
{
    uint16_t demo_magic;
    uint16_t length;
};

uint64_t transmit_package = 0;

struct ks_socket_context *my_socket_context_new(struct ks_socket_container *container)
{
    return calloc(1, sizeof(struct my_socket_context));
}

void my_socket_context_free(struct ks_socket_container *container, struct ks_socket_context *context)
{
    free(context);
}

void my_socket_init(struct ks_socket_container *container, struct ks_socket_context *context)
{
    struct my_socket_context *mycontext = container_of(context, struct my_socket_context, socket_context);
    
    INIT_KS_CIRCULAR_BUFFER(&mycontext->split_buffers);
}

void my_socket_uninit(struct ks_socket_container *container, struct ks_socket_context *context)
{
    struct my_socket_context *mycontext = container_of(context, struct my_socket_context, socket_context);
    
    ks_circular_buffer_destroy(&mycontext->split_buffers);
}

void my_connected(struct ks_socket_container *container, struct ks_socket_context *context)
{
    struct ks_buffer *buffer;
    struct my_header hdr;
    char helloworld[] = "helloworld";
    
    if (context->client)
    {
        buffer = ks_socket_buffer_refernece(container, NULL);
        hdr.demo_magic = DEMO_MAGIC;
        hdr.length = strlen(helloworld) + 1;
        ks_buffer_write(buffer, &hdr, sizeof(hdr));
        ks_buffer_write(buffer, helloworld, strlen(helloworld) + 1);
        
        ks_socket_send(context, buffer);
        
        ks_socket_buffer_derefernece(container, buffer);
        
        fprintf(stderr, "client connected:%llu\n", context->uniqid);
    }
    else
    {
        fprintf(stderr, "server connected:%llu\n", context->uniqid);
    }
}

void my_disconnected(struct ks_socket_container *container, struct ks_socket_context *context)
{
    fprintf(stderr, "disconnected:%llu\n", context->uniqid);
}

void my_arrived(struct ks_socket_container *container, struct ks_socket_context *context, const char *data, ssize_t nread)
{
    struct my_header myhdr;
    struct ks_buffer *buffer;
    struct my_socket_context *mycontext = container_of(context, struct my_socket_context, socket_context);
    
    //fprintf(stderr, "connid:%llu  data arrived:  %lu bytes\n", context->uniqid, nread);
    
    ks_circular_buffer_queue(&mycontext->split_buffers, data, nread);
    
    while (1)
    {
        if (!ks_circular_buffer_peek_array(&mycontext->split_buffers, &myhdr, sizeof(myhdr)))
        {
            return;
        }
        
        if (myhdr.demo_magic != DEMO_MAGIC)
        {
            ks_socket_close(context);
            return;
        }
        
        if (mycontext->split_buffers.usingsize < (myhdr.length + sizeof(myhdr)))
        {
            return;
        }
        
        buffer = ks_socket_buffer_refernece(container, NULL);
        ks_buffer_reserve(buffer, (myhdr.length + sizeof(myhdr)));
        
        if (ks_circular_buffer_dequeue_array(&mycontext->split_buffers, ks_buffer_getdata(buffer), myhdr.length + sizeof(myhdr)) == 0)
        {
            ks_socket_buffer_derefernece(container, buffer);
            ks_socket_close(context);
            fprintf(stderr, "unknown error\n");
            return;
        }
        
        buffer->usingsize += myhdr.length + sizeof(myhdr);
        
        if (container->callback->received)
        {
            container->callback->received(container, context, buffer);
        }
        
        ks_socket_buffer_derefernece(container, buffer);
    }
}

void my_received(struct ks_socket_container *container, struct ks_socket_context *context, struct ks_buffer *buffer)
{
    struct my_header *hdr;
    char *string;
    hdr = ks_buffer_getdata(buffer);
    
    string = (char *)&hdr[1];
    
    //printf("receive:%s\n", string);
    transmit_package++;
    
    ks_socket_send(context, buffer);
}

void my_handle_error(struct ks_socket_container *container, struct ks_socket_context *context, int err)
{
}

uv_timer_t des_timer;
struct ks_socket_container socket_container;
void on_timer(uv_timer_t *timer)
{
    //ks_socket_stop(&socket_container);
    fprintf(stderr, "transmit_package:%llu\n", transmit_package);
}

struct ks_socket_callback callback =
    {
        .socket_context_new = my_socket_context_new,
        .socket_context_free = my_socket_context_free,
        .socket_init = my_socket_init,
        .socket_uninit = my_socket_uninit,
        .connected = my_connected,
        .disconnected = my_disconnected,
        .arrived = my_arrived,
        .received = my_received,
        .handle_error = my_handle_error};

int main(int argc, char *argv[])
{
    INIT_KS_SOCKET_CONTAINER(&socket_container, uv_default_loop(), &callback, 20000, 25000, 65535, 100000, 100000);
    
    ks_socket_addlistener_ipv4(&socket_container, "0.0.0.0", 27015);
    
    uv_timer_init(uv_default_loop(), &des_timer);
    uv_timer_start(&des_timer, on_timer, 1000, 1000);
    
    ks_socket_connect_ipv4(&socket_container, "127.0.0.1", 27015, NULL);
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
