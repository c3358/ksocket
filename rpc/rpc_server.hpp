#ifndef _KS_RPC_SERVER_HPP_
#define _KS_RPC_SERVER_HPP_

#include "ksocket.h"
#include "rpc_pack.hpp"

#include <string>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>

#include <queue>

class ks_rpc_server_handler
{
public:
	ks_rpc_server_handler(uint32_t id);
	virtual ~ks_rpc_server_handler();

	
	virtual int64_t addref();
	virtual void release();
	
	virtual bool parse_request(const void *data, size_t length) = 0;
	virtual const ::google::protobuf::MessageLite &get_request() = 0;
	virtual const ::google::protobuf::MessageLite &get_response() = 0;

	uint32_t id();

	struct ks_queue_thread_order *get_thread_order();

public:
	uint64_t _conn_id;
	uint64_t _sessionid;
	uint32_t _status;
protected:
	int64_t _refernece_count;

private:
	struct ks_queue_thread_order _thread_order;
	uint32_t _id;
};

struct ks_rpc_server_context
{
	struct ks_socket_context context;
	struct ks_circular_buffer tempbuffers;
};

class ks_rpc_server
{
public:
	ks_rpc_server(uv_loop_t *loop);
	virtual ~ks_rpc_server();

	//create ks_rpc_server_handler
	virtual ks_rpc_server_handler* make_handler(struct ks_buffer *buffer) = 0;

	//run as 3rd threads
	virtual void notify_handler(ks_rpc_server_handler *handler) = 0;


	bool start(const struct ks_remoteaddress *remoteaddress);
	void stop();

private:
	void send_response(ks_rpc_server_handler *handler);

protected:
	uv_loop_t *_loop;
	bool _startup;
	struct ks_socket_container _container;
	struct ks_queue_thread _queue_thread;
	struct ks_remoteaddress _remoteaddress;


private:
	static struct ks_socket_callback socket_callbacks;

	static void process_order(struct ks_queue_thread_order *order);
	static void complete_order(struct ks_queue_thread_order *order);
	static void free_order(struct ks_queue_thread_order *order);

	static struct ks_socket_context* context_new(struct ks_socket_container *container);
	static void context_free(struct ks_socket_container *container, struct ks_socket_context *context);
	static void context_init(struct ks_socket_container *container, struct ks_socket_context *context);
	static void context_uninit(struct ks_socket_container *container, struct ks_socket_context *context);
	static void context_connected(struct ks_socket_container *container, struct ks_socket_context *context);
	static void context_disconnected(struct ks_socket_container *container, struct ks_socket_context *context);
	static void context_data_arrived(struct ks_socket_container *container, 
		struct ks_socket_context *context, const char *data, ssize_t nread);
	static void context_data_received(struct ks_socket_container *container, 
		struct ks_socket_context *context, struct ks_buffer *buffer);
	static void context_handle_error(struct ks_socket_container *container, struct ks_socket_context *context, int err);
};

#endif