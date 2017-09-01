#include <iostream>
#include "rpc_client.hpp"
#include "helloworld.pb.h"


class my_hello_world_procedure : public ks_rpc_client_proc
{
public:
	my_hello_world_procedure() : ks_rpc_client_proc(1)
	{

	}

	virtual ~my_hello_world_procedure()
	{

	}

	const ::google::protobuf::MessageLite &get_request()
	{
		return myrequest;
	}

	const ::google::protobuf::MessageLite &get_response()
	{
		return myresponse;
	}

	bool parse_response(const void *data, size_t length)
	{
		return myresponse.ParseFromArray(data, length);
	}


public:
	HelloWorldRequest myrequest;
	HelloWorldResponse myresponse;
};

class my_remote_procedure : public ks_rpc_client
{
public:
	my_remote_procedure(bool server_side_queue) : ks_rpc_client(uv_default_loop(), server_side_queue)
	{

	}

	virtual ~my_remote_procedure()
	{

	}

	virtual void notify_callback(ks_rpc_client_proc *object)
	{
		my_hello_world_procedure *helloworld_rpc = (my_hello_world_procedure *)object;
		printf("run callback.\n");

		if(helloworld_rpc->status == KS_RPCSTATUS_SUCCESS)
		{
			std::cout << helloworld_rpc->myresponse.text() << std::endl;
			std::cout << "successful" << std::endl;
		}
		else
		{
			std::cout << "failed" << std::endl;
		}
	}
};

my_remote_procedure my_rpc(false);
uv_signal_t g_signal[2];
uv_timer_t g_run_timer;

static void signal_handler(uv_signal_t* handle, int signum)
{

	if(signum == SIGINT)
	{
		printf("SIGINT.\n");
		uv_signal_stop(&g_signal[0]);
		uv_signal_stop(&g_signal[1]);
		my_rpc.stop();

		uv_timer_stop(&g_run_timer);
	}

	if(signum == SIGTERM)
	{
		printf("SIGTERM.\n");
		uv_signal_stop(&g_signal[0]);
		uv_signal_stop(&g_signal[1]);
		my_rpc.stop();
		uv_timer_stop(&g_run_timer);
	}
}


void call_helloworld(const char *text)
{
	my_hello_world_procedure *helloworld = new my_hello_world_procedure();

	helloworld->myrequest.set_text(text);

	if(!my_rpc.call(helloworld))
	{
		printf("call rpc failed.\n");
	}
	else
	{
		printf("call rpc successful.\n");
	}
}




static void on_run_timer(uv_timer_t *timer)
{
	call_helloworld("helloworld");
}


int main(int argc, char *argv[])
{
	struct ks_remoteaddress remoteaddress;
	

	uv_signal_init(uv_default_loop(), &g_signal[0]);
	uv_signal_init(uv_default_loop(), &g_signal[1]);

	uv_timer_init(uv_default_loop(), &g_run_timer);


	printf("initialized signal.\n");

	uv_signal_start(&g_signal[0], signal_handler, SIGTERM);
	uv_signal_start(&g_signal[1], signal_handler, SIGINT);

	printf("signal started.\n");

	uv_timer_start(&g_run_timer, on_run_timer, 1000, 1000);

	init_remote_address_pipe(&remoteaddress, "/var/tmp/krpc_helloworld.sock");

	my_rpc.start(&remoteaddress);

	uv_run(uv_default_loop(), UV_RUN_DEFAULT);

	printf("application exit.\n");




	return 0;
}