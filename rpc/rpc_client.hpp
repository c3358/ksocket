#ifndef _KS_RPC_CLIENT_HPP_
#define _KS_RPC_CLIENT_HPP_

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

class ks_rpc_client_proc
{
public:
	ks_rpc_client_proc(uint32_t id);
	virtual ~ks_rpc_client_proc();

	uint32_t id();

	virtual const ::google::protobuf::MessageLite &get_request() = 0;
	virtual const ::google::protobuf::MessageLite &get_response() = 0;
	virtual bool parse_response(const void *data, size_t length) = 0;


public:
	uint32_t status;
	uint64_t sessionid;
	bool is_posted;

private:
	uint32_t _id;
};

struct ks_rpc_client_context
{
	struct ks_socket_context context;
	struct ks_circular_buffer tempbuffers;
};

class ks_rpc_client
{
public:
	ks_rpc_client(uv_loop_t *loop, bool server_side_queue);
	virtual ~ks_rpc_client();

	void start(struct ks_remoteaddress *remoteaddress);
	void stop();
	bool call(ks_rpc_client_proc *object);

	virtual void connection_state(bool is_connected);
	virtual void notify_callback(ks_rpc_client_proc *object) = 0;
private:
	void recv(struct ks_buffer *buffer);
	void send(ks_rpc_client_proc *object);
	void do_connect();
	void failed_all();
	void failed(ks_rpc_client_proc *object);
	void do_next();

public:
	uv_loop_t *_loop;

	struct ks_socket_container _container;

	uv_timer_t _timer;

	struct ks_socket_context *_context;

	bool _connected;

	struct ks_remoteaddress _remoteaddress;

	bool _server_side_queue;
	uint64_t sessionid;

	std::queue<ks_rpc_client_proc *> _orders;

	bool _startup;

private:
	static struct ks_socket_callback socket_callbacks;

	static void on_timer(uv_timer_t *timer);

	static struct ks_socket_context* context_new(struct ks_socket_container *container);
	static void context_free(struct ks_socket_container *container, struct ks_socket_context *context);
	static void context_init(struct ks_socket_container *container, struct ks_socket_context *context);
	static void context_uninit(struct ks_socket_container *container, struct ks_socket_context *context);
	static void context_connected(struct ks_socket_container *container, struct ks_socket_context *context);
	static void context_connect_failed(struct ks_socket_container *container, struct ks_socket_context *socket_context, int status);
	static void context_disconnected(struct ks_socket_container *container, struct ks_socket_context *context);
	static void context_data_arrived(struct ks_socket_container *container, 
		struct ks_socket_context *context, const char *data, ssize_t nread);
	static void context_data_received(struct ks_socket_container *container, 
		struct ks_socket_context *context, struct ks_buffer *buffer);
	static void context_handle_error(struct ks_socket_container *container, struct ks_socket_context *context, int err);
};

#endif