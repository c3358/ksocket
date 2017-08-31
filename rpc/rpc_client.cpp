#include "rpc_client.hpp"

ks_rpc_client_proc::ks_rpc_client_proc(uint16_t id)
    : status(KS_RPCSTATUS_SUCCESS)
    , sessionid(0)
    , is_posted(false)
    , _id(id)
{
}

ks_rpc_client_proc::~ks_rpc_client_proc()
{
}

uint16_t ks_rpc_client_proc::id()
{
    return _id;
}

struct ks_socket_context* ks_rpc_client::context_new(struct ks_socket_container* container)
{
    struct ks_rpc_client_context* mycontext = (struct ks_rpc_client_context*)calloc(1, sizeof(struct ks_rpc_client_context));

    return &mycontext->context;
}

void ks_rpc_client::context_free(struct ks_socket_container* container, struct ks_socket_context* context)
{
    free(context);
}

void ks_rpc_client::context_init(struct ks_socket_container* container, struct ks_socket_context* context)
{
    struct ks_rpc_client_context* mycontext = (struct ks_rpc_client_context*)context;
    INIT_KS_CIRCULAR_BUFFER(&mycontext->tempbuffers);
}

void ks_rpc_client::context_uninit(struct ks_socket_container* container, struct ks_socket_context* context)
{
    struct ks_rpc_client_context* mycontext = (struct ks_rpc_client_context*)context;
    ks_circular_buffer_destroy(&mycontext->tempbuffers);
}

void ks_rpc_client::context_connected(struct ks_socket_container* container, struct ks_socket_context* context)
{
    ks_rpc_client* rpc_client = (ks_rpc_client*)container->data;
    rpc_client->_connected = true;
}

void ks_rpc_client::context_connect_failed(struct ks_socket_container* container, struct ks_socket_context* socket_context, int status)
{
    ks_rpc_client* rpc_client = (ks_rpc_client*)container->data;
    rpc_client->_connected = false;
    rpc_client->_context = NULL;
    rpc_client->failed_all();
}

void ks_rpc_client::context_disconnected(struct ks_socket_container* container, struct ks_socket_context* context)
{
    ks_rpc_client* rpc_client = (ks_rpc_client*)container->data;
    rpc_client->_connected = false;
    rpc_client->_context = NULL;
    rpc_client->failed_all();
}

void ks_rpc_client::context_data_arrived(struct ks_socket_container* container, struct ks_socket_context* context, const char* data, ssize_t nread)
{
    struct ks_rpc_client_context* mycontext = (struct ks_rpc_client_context*)context;

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
        if(myhdr.length < sizeof(struct ks_rpcheader))
        {
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

void ks_rpc_client::context_data_received(struct ks_socket_container* container, struct ks_socket_context* context, struct ks_buffer* buffer)
{

}

void ks_rpc_client::context_handle_error(struct ks_socket_container* container, struct ks_socket_context* context, int err)
{
}

struct ks_socket_callback ks_rpc_client::socket_callbacks = {
    .socket_context_new = context_new,
    .socket_context_free = context_free,
    .socket_init = context_init,
    .socket_uninit = context_uninit,
    .connected = context_connected,
    .connect_failed = context_connect_failed,
    .disconnected = context_disconnected,
    .send_notify = nullptr,
    .arrived = context_data_arrived,
    .received = context_data_received,
    .handle_error = context_handle_error,
    .handle_serverfull = nullptr
};

void ks_rpc_client::on_timer(uv_timer_t* timer)
{
    ks_rpc_client* rpc_client = (ks_rpc_client*)timer->data;
    rpc_client->do_connect();
}

ks_rpc_client::ks_rpc_client(uv_loop_t* loop, bool server_side_queue)
    : _loop(loop)
    , _server_side_queue(server_side_queue)
{
    uv_timer_init(loop, &_timer);
    INIT_KS_SOCKET_CONTAINER(&_container, loop, &socket_callbacks, 2, 2, 2, 1000, 1000);
    _timer.data = this;
}

ks_rpc_client::~ks_rpc_client()
{
    ks_socket_container_destroy(&_container);
    uv_close((uv_handle_t*)&_timer, NULL);
}

void ks_rpc_client::do_connect()
{
    if (_context == NULL) {
        switch (_remoteaddress.type) {
        case KS_REMOTE_ADDRESS_PIPE:
            ks_socket_connect_pipe(&_container, _remoteaddress.host, &_context);
            break;
        case KS_REMOTE_ADDRESS_IPV4:
            ks_socket_connect_ipv4(&_container, _remoteaddress.host, _remoteaddress.port, &_context);
            break;
        case KS_REMOTE_ADDRESS_IPV6:
            ks_socket_connect_ipv6(&_container, _remoteaddress.host, _remoteaddress.port, &_context);
            break;
        }
    }
}

void ks_rpc_client::start(struct ks_remoteaddress* remoteaddress)
{
    if (_startup)
        return;

    _remoteaddress = *remoteaddress;
    uv_timer_start(&_timer, on_timer, 1000, 1000);
    do_connect();

    _startup = true;
}

void ks_rpc_client::stop()
{
    if (_startup) {
    	uv_timer_stop(&_timer);
    	if(_context) {
    		ks_socket_close(_context);
    	}

    	_startup = false;
    }
}

void ks_rpc_client::call(ks_rpc_client_proc* proc)
{
	if(!_connected)
	{
		failed(proc);
		delete proc;
		return;
	}

	proc->sessionid = __sync_fetch_and_add(&sessionid, 1);

	_orders.push(proc);

	if(_server_side_queue)
	{
		send(proc);
		proc->is_posted = true;
	}
}

void ks_rpc_client::send(ks_rpc_client_proc* proc)
{
	struct ks_rpcheader rpchdr;

	const ::google::protobuf::MessageLite &request = proc->get_request();
	int32_t request_size = request.ByteSize();

	rpchdr.magic = KS_RPCHEADER_MAGIC;
	rpchdr.id = proc->id();
	rpchdr.sessionid = proc->sessionid;
	rpchdr.status = KS_RPCSTATUS_SUCCESS;
	rpchdr.length = sizeof(rpchdr) + request_size;


	struct ks_buffer *buffer = ks_socket_buffer_refernece(&_container, NULL);
	ks_buffer_setsize(buffer, sizeof(rpchdr) + request_size);

	unsigned char *data = (unsigned char *)ks_buffer_getdata(buffer);
	memcpy(data, &rpchdr, sizeof(rpchdr));
	data += sizeof(rpchdr);
	request.SerializeToArray(data, request_size);

	ks_socket_send(_context, buffer);
	ks_socket_buffer_derefernece(&_container, buffer);
}

void ks_rpc_client::do_next()
{
	if(_orders.empty())
		return;

	ks_rpc_client_proc *proc = _orders.front();

	if(proc->is_posted){
		return;
	}

	send(proc);
	proc->is_posted = true;
}

void ks_rpc_client::recv(struct ks_buffer *buffer)
{
	struct ks_rpcheader *rpchdr = (struct ks_rpcheader *)ks_buffer_getdata(buffer);

	if(_orders.empty())
	{
		ks_socket_close(_context);
		return;
	}

	ks_rpc_client_proc *procedure = _orders.front();

	if(procedure->id() != rpchdr->id)
	{
		ks_socket_close(_context);
		return;
	}

	const void *data = &rpchdr[1];

	procedure->status = rpchdr->status;

	if(!procedure->parse_response(data, rpchdr->length - sizeof(struct ks_rpcheader)))
	{
		procedure->status = KS_RPCSTATUS_PARSE_PACKET;
	}


	notify_callback(procedure);

	_orders.pop();

	delete procedure;

	do_next();
}

void ks_rpc_client::failed_all()
{
	while(!_orders.empty())
	{
		ks_rpc_client_proc *proc = _orders.front();
		failed(proc);
		_orders.pop();
		delete proc;
	}
}

void ks_rpc_client::failed(ks_rpc_client_proc *proc)
{
	proc->status = KS_RPCSTATUS_FAILED;
	notify_callback(proc);
}