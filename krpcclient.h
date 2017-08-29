#ifndef _KS_RPCCLIENT_H_
#define _KS_RPCCLIENT_H_

struct ks_rpcc;
struct ks_rpcc_object;

struct ks_rpcc_object		//user object extends this
{
	struct list_head entry;
	uint64_t sessionid;
	bool is_posted;
	uint32_t rpcid;
	uint32_t status;
};



struct ks_rpcc_context		//client network context
{
	struct ks_socket_context context;
	struct ks_circular_buffer tempbuffers;
};


typedef void (*ks_rpc_connstate)(struct ks_rpcc *rpcc, bool connected, bool is_connect_failed);
typedef void (*ks_rpc_eventcallback)(struct ks_rpcc *rpcc, struct ks_rpcc_object *qobject);

struct ks_rpcc_cbs			//callbacks
{
	ks_rpc_connstate connstate;			//notify connection state
	ks_rpc_eventcallback sendcallback;	//encoder callback & send buffer
	ks_rpc_eventcallback recvcallback;	//recv buffer & decoder callback
	ks_rpc_eventcallback eventcallback;	//process event
	ks_rpc_eventcallback freecallback;		//free buffers
};

struct ks_rpcc 				//rpc client object
{
	struct ks_socket_container container;
	struct ks_rpcc_context *context;
	uint64_t num_queue;
	struct list_head queue;
	struct ks_rpcc_cbs *callbacks;
	bool serverside_queue;
	struct ks_remoteaddress *remoteaddress;
	uint64_t sessionid;
};

void INIT_KS_RPCC(struct ks_rpcc *rc,  uv_loop_t *loop, 
							struct ks_remoteaddress *remoteaddress, 
							struct ks_rpcc_cbs *callbacks, bool serverside_queue);

void ks_rpc_client_destroy(struct ks_rpcc *rpcc);

void ks_rpc_client_start(struct ks_rpcc *rpcc);
void ks_rpc_client_stop(struct ks_rpcc *rpcc);

uint64_t ks_rpc_client_queue_size(struct ks_rpcc *rpcc);
bool ks_rpcc_post(struct ks_rpcc *rpcc, struct ks_rpcc_object *qobject);




#endif