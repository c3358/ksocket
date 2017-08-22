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
        
        fprintf(stderr, "client connected:%lu\n", context->uniqid);
    }
    else
    {
        fprintf(stderr, "server connected:%lu\n", context->uniqid);
    }
}

void my_disconnected(struct ks_socket_container *container, struct ks_socket_context *context)
{
    fprintf(stderr, "disconnected:%lu\n", context->uniqid);
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
   // struct my_header *hdr;
//    char *string;
    //hdr = ks_buffer_getdata(buffer);
    
   // string = (char *)&hdr[1];
    
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
    fprintf(stderr, "transmit_package:%lu\n", transmit_package);
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


struct ks_locked_queue g_locked_queue;
void unit_test_locked_queue()
{
    int i;
    int c;
    struct list_head *item;

    INIT_KS_LOCKED_QUEUE(&g_locked_queue);

    c = 0;

    for(i = 0;i < 100; i++)
    {
        item = calloc(1, sizeof(struct list_head));
        ks_locked_queue_push_back(&g_locked_queue, item);
    }

    while(!ks_locked_queue_empty(&g_locked_queue))
    {
        item = ks_locked_queue_pop_front(&g_locked_queue);
        c++;
        free(item);
    }

    printf("post 100 receive:%d\n", c);

    c = 0;
    
    for(i = 0;i < 100; i++)
    {
        item = calloc(1, sizeof(struct list_head));
        ks_locked_queue_push_front(&g_locked_queue, item);
    }

    while(!ks_locked_queue_empty(&g_locked_queue))
    {
        item = ks_locked_queue_pop_front(&g_locked_queue);
        c++;
        free(item);
    }

    printf("post 100 receive:%d\n", c);

    c = 0;
    
    for(i = 0;i < 100; i++)
    {
        item = calloc(1, sizeof(struct list_head));
        ks_locked_queue_push_back(&g_locked_queue, item);
    }

    while(!ks_locked_queue_empty(&g_locked_queue))
    {
        item = ks_locked_queue_pop_back(&g_locked_queue);
        c++;
        free(item);
    }

    printf("post 100 receive:%d\n", c);

    ks_locked_queue_destroy(&g_locked_queue);
}


struct ks_queue_thread g_locked_queue_thread;
int g_process_count;
int g_complete_count;
void my_queue_thread_processorder(struct ks_queue_thread_order *order)
{
    g_process_count++;
}

void my_queue_thread_completeorder(struct ks_queue_thread_order *order)
{
    g_complete_count++;
}

void my_queue_thread_free_entry(struct ks_queue_thread_order *order)
{
    free(order);
}



void unit_test_locked_queue_thread()
{
    int i;
    struct ks_queue_thread_order *order;
    INIT_KS_QUEUE_THREAD(&g_locked_queue_thread, uv_default_loop(), 50, my_queue_thread_processorder,
    my_queue_thread_completeorder, my_queue_thread_free_entry);

    

    for(i = 0; i < 100; i++)
    {
        order = calloc(1, sizeof(struct ks_queue_thread_order));
        if(!ks_queue_thread_post(&g_locked_queue_thread, order))
        {
            printf("post %d failed is full.\n", i);
        }
    }
    ks_queue_thread_start(&g_locked_queue_thread);
    sleep(1);
    uv_run(uv_default_loop(), UV_RUN_NOWAIT);
    ks_queue_thread_stop(&g_locked_queue_thread);

    printf("process ok:%d  %d\n", g_complete_count, g_process_count);

    ks_queue_thread_destroy(&g_locked_queue_thread);
}

void dump_buffer(unsigned char *data, size_t length)
{
    size_t i;

    printf("dump buffer:");
    for(i = 0; i < length ; i++)
    {
        printf("%02X", data[i]);
    }
    printf("\n");
}

struct st_testA
{
    int a, b;
};

void unit_test_protobyte_reader(void *data, size_t length)
{
    struct ks_protobyte_reader sr;
    unsigned char v1;
    char v2;
    int32_t v3;
    uint32_t v4;
    int64_t v5;
    uint64_t v6;
    float v7;
    double v8;
    char *v9;
    void *v10;
    size_t v10_len;
    void *elts;
    size_t size;
    int nelts;
    int i;
    struct st_testA *testST;



    INIT_KS_PROTOBYTE_READER(&sr, data, length);

    assert(ks_protobyte_read_bool(&sr, &v1));
    printf("ks_protobyte_read_bool -> %d\n", v1);

    assert(ks_protobyte_read_bool(&sr, &v1));
    printf("ks_protobyte_read_bool -> %d\n", v1);

    assert(ks_protobyte_read_char(&sr, &v2));
    printf("ks_protobyte_read_char -> %d\n", v2);

    assert(ks_protobyte_read_uchar(&sr, &v1));
    printf("ks_protobyte_read_uchar -> %d\n", v1);

    assert(ks_protobyte_read_int32(&sr, &v3));
    printf("ks_protobyte_read_int32 -> %08x\n", v3);

    assert(ks_protobyte_read_uint32(&sr, &v4));
    printf("ks_protobyte_read_uint32 -> %08x\n", v4);

    assert(ks_protobyte_read_int64(&sr, &v5));
    printf("ks_protobyte_read_int64 -> %16lx\n", v5);

    assert(ks_protobyte_read_uint64(&sr, &v6));
    printf("ks_protobyte_read_uint64 -> %16lx\n", v6);


    assert(ks_protobyte_read_float(&sr, &v7));
    printf("ks_protobyte_read_float -> %f\n", v7);

    assert(ks_protobyte_read_double(&sr, &v8));
    printf("ks_protobyte_read_double -> %lf\n", v8);

    v9 = NULL;
    v10 = NULL;
    elts = NULL;

    assert(ks_protobyte_read_string(&sr, &v9));
    printf("ks_protobyte_read_string -> %s\n", v9);

    assert(ks_protobyte_read_blob(&sr, &v10, &v10_len));
    printf("ks_protobyte_read_blob ->");

    dump_buffer(v10, v10_len);


    assert(ks_protobyte_read_array(&sr, &elts, &size, &nelts));
    printf("ks_protobyte_read_array ->");


    testST = elts;

    for(i = 0; i < nelts; i++)
    {
        printf("%08x  %08x\n", testST[i].a, testST[i].b);
    }

    free(v10);
    free(v9);
    free(elts);

    printf("unit_test_protobyte_reader done.\n");
}

void unit_test_protobyte()
{
    struct ks_protobyte protobyte;
    struct st_testA *testST;
    void *serialize_data;
    size_t serialize_size, serialize_size2;

    INIT_KS_PROTOBYTE(&protobyte);
    ks_protobyte_push_bool(&protobyte, 1);
    ks_protobyte_push_bool(&protobyte, 0);
    ks_protobyte_push_char(&protobyte, '0');
    ks_protobyte_push_uchar(&protobyte, '1');
    assert(ks_protobyte_size(&protobyte) == 8);
    printf("protobyte size %zu\n", ks_protobyte_size(&protobyte));

    ks_protobyte_push_int32(&protobyte, 0x12341234);
    assert(ks_protobyte_size(&protobyte) == 13);
    printf("protobyte_push_int32 protobyte size %zu\n", ks_protobyte_size(&protobyte));

    ks_protobyte_push_uint32(&protobyte, 0xffffffff);
    assert(ks_protobyte_size(&protobyte) == 18);
    printf("protobyte_push_uint32 protobyte size %zu\n", ks_protobyte_size(&protobyte));

    ks_protobyte_push_int64(&protobyte, 0x1234123412345678);
    assert(ks_protobyte_size(&protobyte) == 27);
    printf("protobyte_push_int64 protobyte size %zu\n", ks_protobyte_size(&protobyte));

    ks_protobyte_push_uint64(&protobyte, 0xffffffffffffffff);
    assert(ks_protobyte_size(&protobyte) == 36);
    printf("protobyte_push_uint64 protobyte size %zu\n", ks_protobyte_size(&protobyte));

    ks_protobyte_push_float(&protobyte, 1.0f);
    assert(ks_protobyte_size(&protobyte) == 41);
    printf("protobyte_push_float protobyte size %zu\n", ks_protobyte_size(&protobyte));

    ks_protobyte_push_double(&protobyte, 10000.0);
    assert(ks_protobyte_size(&protobyte) == 50);
    printf("protobyte_push_double protobyte size %zu\n", ks_protobyte_size(&protobyte));

    ks_protobyte_push_string(&protobyte, "1234");
    assert(ks_protobyte_size(&protobyte) == 60);
    printf("protobyte_push_string protobyte size %zu\n", ks_protobyte_size(&protobyte));

    ks_protobyte_push_blob(&protobyte, "1234", 4);
    assert(ks_protobyte_size(&protobyte) == 69);
    printf("protobyte_push_blob protobyte size %zu\n", ks_protobyte_size(&protobyte));

    testST = calloc(100, sizeof(struct st_testA));
    ks_protobyte_push_array(&protobyte, testST, sizeof(struct st_testA), 100);
    assert(ks_protobyte_size(&protobyte) == 69 + sizeof(struct st_testA) * 100 + sizeof(int) + sizeof(uint32_t) + sizeof(unsigned char));
    printf("protobyte_push_array protobyte size %zu\n", ks_protobyte_size(&protobyte));

    printf("all struct push verify ok.\n");

    assert(protobyte.members_count == 13);
    printf("total %zu elements.\n", protobyte.members_count);

    serialize_size = ks_protobyte_size(&protobyte);
    serialize_data = malloc(serialize_size);
    serialize_size2 = ks_protobyte_serialize_as_array(&protobyte, serialize_data, serialize_size);
    assert(serialize_size == serialize_size2);
    printf("serialize_size %zu  serialize_size2:%zu \n", serialize_size, serialize_size2);

    dump_buffer(serialize_data, serialize_size);

    unit_test_protobyte_reader(serialize_data, serialize_size);

    free(serialize_data);

    ks_protobyte_destroy(&protobyte);

}

int main(int argc, char *argv[])
{
    unit_test_protobyte();
    return 0;

    unit_test_locked_queue();
    unit_test_locked_queue_thread();

    INIT_KS_SOCKET_CONTAINER(&socket_container, uv_default_loop(), &callback, 20000, 25000, 65535, 100000, 100000);
    
    ks_socket_addlistener_ipv4(&socket_container, "0.0.0.0", 27015);
    
    uv_timer_init(uv_default_loop(), &des_timer);
    uv_timer_start(&des_timer, on_timer, 1000, 1000);
    
    ks_socket_connect_ipv4(&socket_container, "127.0.0.1", 27015, NULL);
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
}
