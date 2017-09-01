#include "ksocket.h"
#include "rpc_server.hpp"


ks_rpc_server_handler::ks_rpc_server_handler(uint32_t id)
{
	_refernece_count = 1;
	_conn_id = 0;
	_sessionid = 0;
	_status = KS_RPCSTATUS_SUCCESS;
	_id = id;
}

ks_rpc_server_handler::~ks_rpc_server_handler()
{

}

int64_t ks_rpc_server_handler::addref()
{
	return __sync_add_and_fetch(&_refernece_count, 1);
}

void ks_rpc_server_handler::release()
{
	if(__sync_sub_and_fetch(&_refernece_count, 1) <= 0)
	{
		delete this;
	}
}

struct ks_queue_thread_order *ks_rpc_server_handler::get_thread_order()
{
	_thread_order.data = this;
	return &_thread_order;
}

uint32_t ks_rpc_server_handler::id()
{
	return _id;
}

ks_rpc_server::ks_rpc_server(uv_loop_t *loop) : _loop(loop), _startup(false)
{
	INIT_KS_SOCKET_CONTAINER(&_container, loop, &socket_callbacks, 5000, 5000, 65535, 10000, 10000);
	INIT_KS_QUEUE_THREAD(&_queue_thread, loop,  100000, process_order, complete_order, free_order);

	_container.data = this;
	_queue_thread.data = this;
}


ks_rpc_server::~ks_rpc_server()
{
	ks_queue_thread_destroy(&_queue_thread);
	ks_socket_container_destroy(&_container);
}

bool ks_rpc_server::start(const struct ks_remoteaddress *remoteaddress)
{
	_remoteaddress = *remoteaddress;
	return true;
}

void ks_rpc_server::stop()
{

}

void ks_rpc_server::send_response(ks_rpc_server_handler *handler)
{
	struct ks_socket_context *context = ks_socket_find(&_container, handler->_conn_id);
	if(context == nullptr)
		return;

	struct ks_rpcheader rpchdr;

    const ::google::protobuf::MessageLite& request = handler->get_response();
    int32_t request_size = request.ByteSize();

    rpchdr.magic = KS_RPCHEADER_MAGIC;
    rpchdr.id = handler->id();
    rpchdr.sessionid = handler->_sessionid;
    rpchdr.status = KS_RPCSTATUS_SUCCESS;
    rpchdr.length = sizeof(rpchdr) + request_size;

    struct ks_buffer* buffer = ks_socket_buffer_refernece(&_container, NULL);
    ks_buffer_setsize(buffer, sizeof(rpchdr) + request_size);

    unsigned char* data = (unsigned char*)ks_buffer_getdata(buffer);
    memcpy(data, &rpchdr, sizeof(rpchdr));
    data += sizeof(rpchdr);
    request.SerializeToArray(data, request_size);

    ks_socket_send(context, buffer);
    ks_socket_buffer_derefernece(&_container, buffer);
}















//static functions

struct ks_socket_context* ks_rpc_server::context_new(struct ks_socket_container* container)
{
    struct ks_rpc_server_context* mycontext = (struct ks_rpc_server_context*)calloc(1, sizeof(struct ks_rpc_server_context));
    return &mycontext->context;
}

void ks_rpc_server::context_free(struct ks_socket_container* container, struct ks_socket_context* context)
{
    free(context);
}

void ks_rpc_server::context_init(struct ks_socket_container* container, struct ks_socket_context* context)
{
    struct ks_rpc_server_context* mycontext = (struct ks_rpc_server_context*)context;
    INIT_KS_CIRCULAR_BUFFER(&mycontext->tempbuffers);
}

void ks_rpc_server::context_uninit(struct ks_socket_container* container, struct ks_socket_context* context)
{
    struct ks_rpc_server_context* mycontext = (struct ks_rpc_server_context*)context;
    ks_circular_buffer_destroy(&mycontext->tempbuffers);
}

void ks_rpc_server::context_connected(struct ks_socket_container* container, struct ks_socket_context* context)
{
    printf("connect to server completed.\n");
}

void ks_rpc_server::context_disconnected(struct ks_socket_container* container, struct ks_socket_context* context)
{
    printf("server connection disconnected.\n");
}

void ks_rpc_server::context_data_arrived(struct ks_socket_container* container, struct ks_socket_context* context, const char* data, ssize_t nread)
{
    struct ks_rpc_server_context* mycontext = (struct ks_rpc_server_context*)context;

    struct ks_buffer* buffer;
    void* buffer_data;

    struct ks_rpcheader myhdr;

    ks_circular_buffer_queue(&mycontext->tempbuffers, data, nread);

    while (1) {
        if (!ks_circular_buffer_peek_array(&mycontext->tempbuffers, &myhdr, sizeof(myhdr))) {
            return;
        }

        if (myhdr.magic != KS_RPCHEADER_MAGIC) {
            ks_socket_close(context);
            return;
        }

        if (myhdr.length > KS_RPC_DATA_MAX_LENGTH) {
            ks_socket_close(context);
            return;
        }
        if (myhdr.length < sizeof(struct ks_rpcheader)) {
            ks_socket_close(context);
            return;
        }

        if (mycontext->tempbuffers.usingsize < myhdr.length) {
            return;
        }

        buffer = ks_socket_buffer_refernece(container, NULL);

        ks_buffer_setsize(buffer, myhdr.length);

        buffer_data = ks_buffer_getdata(buffer);

        if (ks_circular_buffer_dequeue_array(&mycontext->tempbuffers, buffer_data, myhdr.length) == 0) {
            ks_socket_buffer_derefernece(container, buffer);
            ks_socket_close(context);
            return;
        }

        if (container->callback->received) {
            container->callback->received(container, context, buffer);
        }

        ks_socket_buffer_derefernece(container, buffer);
    }
}


void ks_rpc_server::context_data_received(struct ks_socket_container* container, struct ks_socket_context* context, struct ks_buffer* buffer)
{
}

void ks_rpc_server::context_handle_error(struct ks_socket_container* container, struct ks_socket_context* context, int err)
{
}

void ks_rpc_server::process_order(struct ks_queue_thread_order *order)
{
	ks_rpc_server *rpc_server = (ks_rpc_server *)order->queue_thread->data;
	ks_rpc_server_handler *handler = (ks_rpc_server_handler *)order->data;

	rpc_server->notify_handler(handler);
}

void ks_rpc_server::complete_order(struct ks_queue_thread_order *order)
{
	ks_rpc_server *rpc_server = (ks_rpc_server *)order->queue_thread->data;
	ks_rpc_server_handler *handler = (ks_rpc_server_handler *)order->data;
	rpc_server->send_response(handler);
}

void ks_rpc_server::free_order(struct ks_queue_thread_order *order)
{
	ks_rpc_server_handler *handler = (ks_rpc_server_handler *)order->data;

	delete handler;
}

struct ks_socket_callback ks_rpc_server::socket_callbacks = {
    .socket_context_new = context_new,
    .socket_context_free = context_free,
    .socket_init = context_init,
    .socket_uninit = context_uninit,
    .connected = context_connected,
    .connect_failed = nullptr,
    .disconnected = context_disconnected,
    .send_notify = nullptr,
    .arrived = context_data_arrived,
    .received = context_data_received,
    .handle_error = context_handle_error,
    .handle_serverfull = nullptr
};