#include "ksocket.h"
#include "krpcproto.h"
#include "krpcclient.h"


static struct ks_socket_context* kRpcClientContextNew(struct ks_socket_container *container)
{
    return calloc(1, sizeof(struct ks_rpcc_context));
}

static void kRpcClientContextFree(struct ks_socket_container *container, struct ks_socket_context *context)
{
    free(context);
}

static void kRpcClientContextInit(struct ks_socket_container *container, struct ks_socket_context *context)
{
    struct ks_rpcc_context *mycontext = container_of(context, struct ks_rpcc_context, context);
    
    INIT_KS_CIRCULAR_BUFFER(&mycontext->tempbuffers);
}

static void kRpcClientContextUninit(struct ks_socket_container *container, struct ks_socket_context *context)
{
    struct ks_rpcc_context *mycontext = container_of(context, struct ks_rpcc_context, context);
    
    ks_circular_buffer_destroy(&mycontext->tempbuffers);
}


static void kRpcClientFailedAllRpc(struct ks_rpcc  *rc)
{
	while(!list_empty(&rc->queue))
	{
		struct ks_rpcc_object *rcobj = list_first_entry(&rc->queue, struct ks_rpcc_object, entry);
		list_del(&rcobj->entry);

		rcobj->status = KS_RPCSTATUS_FAILED;

		rc->callbacks.eventcallback(rc, rcobj);
		rc->callbacks.freecallback(rc, rcobj);
	}
}

static void kRpcClientConnected(struct ks_socket_container *container, struct ks_socket_context *context)
{
	struct ks_rpcc  *rc = container_of(container, struct ks_rpcc, container);
	rc->is_connected = true;
}

static void kRpcClientConnectFailed(struct ks_socket_container *container, struct ks_socket_context *socket_context, int status)
{
	struct ks_rpcc  *rc = container_of(container, struct ks_rpcc, container);
	rc->context = NULL;
	rc->is_connected = false;

	kRpcClientFailedAllRpc(rc);
}

static void kRpcClientDisconnected(struct ks_socket_container *container, struct ks_socket_context *context)
{
	struct ks_rpcc  *rc = container_of(container, struct ks_rpcc, container);
	rc->context = NULL;
	rc->is_connected = false;

	kRpcClientFailedAllRpc(rc);
}

static void kRpcClientDataArrived(struct ks_socket_container *container, 
	struct ks_socket_context *context, const char *data, ssize_t nread)
{
    struct ks_rpcheader myhdr;
    struct ks_buffer *buffer;
    void *buffer_data;
    struct ks_rpcc_context *mycontext = container_of(context, struct ks_rpcc_context, context);
    
    //fprintf(stderr, "connid:%llu  data arrived:  %lu bytes\n", context->uniqid, nread);
    
    ks_circular_buffer_queue(&mycontext->tempbuffers, data, nread);
    
    while (1)
    {
        if (!ks_circular_buffer_peek_array(&mycontext->tempbuffers, &myhdr, sizeof(myhdr)))
        {
            return;
        }
        
        if (myhdr.magic != KS_RPCHEADER_MAGIC)
        {
            ks_socket_close(context);
            return;
        }

        if(myhdr.length > KS_RPC_DATA_MAX_LENGTH)
        {
        	ks_socket_close(context);
        	return;
        }

        if (mycontext->tempbuffers.usingsize < myhdr.length)
        {
            return;
        }
        
        buffer = ks_socket_buffer_refernece(container, NULL);
        ks_buffer_setsize(buffer, myhdr.length);
        
        buffer_data = ks_buffer_getdata(buffer);

        if (ks_circular_buffer_dequeue_array(&mycontext->tempbuffers, 
        	 buffer_data, myhdr.length) == 0)
        {
            ks_socket_buffer_derefernece(container, buffer);
            ks_socket_close(context);
            fprintf(stderr, "unknown error\n");
            return;
        }
        
        if (container->callback->received)
        {
            container->callback->received(container, context, buffer);
        }
        
        ks_socket_buffer_derefernece(container, buffer);
    }
}

static void kRpcClientDo(struct ks_rpcc  *rc)
{
	if(list_empty(&rc->queue)){
		return;
	}

	if(rc->serverside_queue) {
		return;
	}

	struct ks_rpcc_object *rcobj = list_first_entry(&rc->queue, struct ks_rpcc_object, entry);

	if(rcobj->is_posted){
		return;
	}

	rc->callbacks.sendcallback(rc, rcobj);
}

static void kRpcClientDataReceived(struct ks_socket_container *container, 
	struct ks_socket_context *context, struct ks_buffer *buffer)
{
	struct ks_rpcc  *rc;
	 struct ks_rpcheader *myhdr = ks_buffer_getdata(buffer);
	 rc = container_of(container, struct ks_rpcc, container);

	 if(list_empty(&rc->queue))		//
	 {
	 	printf("kRpcClientDataReceived list_empty %s:%d\n", __FILE__, __LINE__);
	 	abort();
	 	return;
	 }

	 struct ks_rpcc_object *rcobj = list_first_entry(&rc->queue, struct ks_rpcc_object, entry);
	 if(rcobj->sessionid == myhdr->sessionid)
	 {
	 	rc->callbacks.recvcallback(rc, rcobj, buffer);
	 	rc->callbacks.eventcallback(rc, rcobj);
	 	list_del(&rcobj->entry);
	 	rc->callbacks.freecallback(rc, rcobj);

	 	kRpcClientDo(rc);
	 }
	 else
	 {
	 	printf("rcobj->sessionid != myhdr->sessionid %s:%d\n", __FILE__, __LINE__);
	 	abort();
	 	return;
	 }
}

static void kRpcClientHandleError(struct ks_socket_container *container, struct ks_socket_context *context, int err)
{
}



static struct ks_socket_callback kRpcClientSocketCallback = {
	.socket_context_new = kRpcClientContextNew,
        .socket_context_free = kRpcClientContextFree,
        .socket_init = kRpcClientContextInit,
        .socket_uninit = kRpcClientContextUninit,
        .connected = kRpcClientConnected,
        .connect_failed = kRpcClientConnectFailed,
        .disconnected = kRpcClientDisconnected,
        .arrived = kRpcClientDataArrived,
        .received = kRpcClientDataReceived,
        .handle_error = kRpcClientHandleError
};

static bool kRpcClientDoConnect(struct ks_rpcc *rc)
{
	if(rc->context)
		return false;

	switch(rc->remoteaddress.type)
	{
		case KS_REMOTE_ADDRESS_PIPE:
			return (ks_socket_connect_pipe(&rc->container, rc->remoteaddress.host, &rc->context) == 0);
			break;
		case KS_REMOTE_ADDRESS_IPV4:
			return (ks_socket_connect_ipv4(&rc->container, rc->remoteaddress.host, 
						rc->remoteaddress.port, &rc->context) == 0);
			break;
		case KS_REMOTE_ADDRESS_IPV6:
			return (ks_socket_connect_ipv6(&rc->container, rc->remoteaddress.host,
			rc->remoteaddress.port,  &rc->context) == 0);
			break;
	}

	return false;
}

static void kRpcClientTimer(uv_timer_t *timer)
{
	struct ks_rpcc *rc = container_of(timer, struct ks_rpcc , timer);

	if(!rc->is_connected && rc->context == NULL)
	{
		kRpcClientDoConnect(rc);
	}
}


static void kRpcDoSend(struct ks_rpcc *rc, struct ks_rpcc_object *rcobj)
{
	rc->callbacks.sendcallback(rc, rcobj);
	rcobj->is_posted = true;
}


void INIT_KS_RPCC(struct ks_rpcc *rc,  uv_loop_t *loop, 
							struct ks_remoteaddress *remoteaddress, 
							struct ks_rpcc_cbs *callbacks, bool serverside_queue)
{
	INIT_KS_SOCKET_CONTAINER(&rc->container, 
		loop, 
		&kRpcClientSocketCallback, 2, 2, 2, 1000, 1000);

	rc->context = NULL;
	rc->num_queue = 0;
	INIT_LIST_HEAD(&rc->queue);
	rc->callbacks = *callbacks;
	rc->serverside_queue = serverside_queue;
	rc->remoteaddress = *remoteaddress;
	rc->sessionid = 0;
	rc->is_connected = false;
	rc->loop = loop;
	uv_timer_init(loop, &rc->timer);
	rc->is_started = false;
}

void ks_rpc_client_start(struct ks_rpcc *rpcc)
{
	if(rpcc->is_started)
		return;

	rpcc->is_started = true;
	kRpcClientDoConnect(rpcc);
	uv_timer_start(&rpcc->timer, kRpcClientTimer, 1000, 1000);
}

void ks_rpc_client_stop(struct ks_rpcc *rpcc)
{
	if(!rpcc->is_started)
		return;
	uv_timer_stop(&rpcc->timer);
	if(rpcc->context) ks_socket_close(rpcc->context);

	rpcc->is_started = false;
}

bool ks_rpcc_post(struct ks_rpcc *rpcc, struct ks_rpcc_object *qobject)
{
	bool first_send;
	if(!rpcc->is_connected) {
		qobject->status = KS_RPCSTATUS_FAILED;
		rpcc->callbacks.eventcallback(rpcc, qobject);
		rpcc->callbacks.freecallback(rpcc, qobject);
		return false;
	}

	first_send = false;

	if(list_empty(&rpcc->queue))
	{
		first_send = true;
	}
	
	list_add_tail(&qobject->entry, &rpcc->queue);
	qobject->is_posted = false;

	if(first_send)
	{
		kRpcDoSend(rpcc, qobject);
		return true;
	}

	if(rpcc->serverside_queue)
	{
		kRpcDoSend(rpcc, qobject);
	}

	return true;
}