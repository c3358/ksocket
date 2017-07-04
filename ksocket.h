#ifndef _KSOCKET_INCLUDED_H_
#define _KSOCKET_INCLUDED_H_

typedef int kboolean;

//套接字接收数据的缓冲区
#define KS_SOCKET_CONTEXT_RDBUF_SIZE 4096
//环形BUFFER的数据缓冲大小
#define KS_CIRCULAR_BUFFER_BLOCK_SIZE 4096
//普通buffer的默认数据大小
#define KS_BUFFER_DEFAULT_DATA_SIZE 512

struct ks_circular_buffer_block
{
	struct list_head entry;
	char buf[KS_CIRCULAR_BUFFER_BLOCK_SIZE];
};

struct ks_circular_buffer
{
	struct list_head head;
	size_t totalsize;
	size_t usingsize;
	
	struct list_head *tail, *first;
	
	size_t outpos, inpos;
};

struct ks_buffer
{
	int64_t refcount;

	struct list_head entry;
	
	unsigned char data[KS_BUFFER_DEFAULT_DATA_SIZE];
	unsigned char *data2;
	
	size_t totalsize, usingsize;
};

//列表成员信息
struct ks_table_slot
{
	//下一个成员
	struct ks_table_slot *nextslot;
	
	//保存唯一id
	uint64_t slotid;
	
	//用户的数据
	void *data;
};

//快速搜索列表
struct ks_table
{
	//成员总数量
	size_t member_count;
	
	//最大通道数量
	uint32_t max_slots;
	
	//通道成员
	struct ks_table_slot **slots;
};

typedef void (*ks_table_callback)(struct ks_table *table, uint64_t id, void *data, void *user_arg);

struct ks_handle_s
{
	union
	{
		uv_handle_t handle;
		uv_stream_t stream;
		uv_tcp_t tcp;
		uv_pipe_t pipe;
	};
};

enum ks_socket_status
{
	KS_SOCKET_STATUS_UNINITIALIZE,      //没初始化uv_handle_t的状态
	KS_SOCKET_STATUS_INITIALIZED,       //初始化uv_handle_t后的状态
	KS_SOCKET_STATUS_CONNECTING,        //建立连接中
	KS_SOCKET_STATUS_LISTEN,	    //监听中
	KS_SOCKET_STATUS_ESTABLISHED,       //已经建立连接
	KS_SOCKET_STATUS_SHUTTINGDOWN,      //正在执行shutdown操作
	KS_SOCKET_STATUS_SHUTDOWN,          //shutdown操作执行完成
	KS_SOCKET_STATUS_CLOSING,           //正在执行close操作
	KS_SOCKET_STATUS_CLOSED             //close操作执行完成
};


struct ks_netadr
{
	union
	{
		struct sockaddr addr;
		struct sockaddr_in addr_ipv4;
		struct sockaddr_in6 addr_ipv6;
		struct sockaddr_un addr_unix;
	};
};

struct ks_socket_context;
struct ks_writereq
{
	//关联到socket context的sendorder
	struct list_head entry;

	//写操作请求队列
	uv_write_t wrequest;

	//写操作的buf
	uv_buf_t bufptr;

	//写出的缓存数据
	struct ks_buffer *buffer;

	//所属套接字
	struct ks_socket_context *context;
};

struct ks_socket_container;

//单一socket context
struct ks_socket_context
{
	//user data
	void *data;
	
	//排除在链接数之外的对象
	int exclude;
    
    //是否客户端
    int client;
	
	//连接状态的列表位置
	struct list_head entry;
	
	//网络套接字
	struct ks_handle_s handle;
	
	//当前套接字是否为激活状态
	kboolean active;
	
	//当前连接的唯一id
	uint64_t uniqid;
	
	//引用计数器
	int64_t refcount;
	
	//读取缓冲区
	char rdbuf[KS_SOCKET_CONTEXT_RDBUF_SIZE];
	
	//当前连接的状态
	enum ks_socket_status status;
	
	//发送队列
	//记录投递到异步操作的操作对象 ks_writereq <=> sendorder <=> ks_writereq
	struct list_head sendorder;
	
	//套接字容器
	struct ks_socket_container *container;

	//连接请求
	uv_connect_t connect_req;

	//关闭请求
	uv_shutdown_t shutdown_req;
	
	//是否在close函数回调通知断开
	int after_close_disconnected;
};

struct ks_socket_callback
{
	//创建socket_context,不能为NULL
	struct ks_socket_context *(*socket_context_new)(struct ks_socket_container *container);

	//释放socket_context,不能为NULL
	void (*socket_context_free)(struct ks_socket_container *container, struct ks_socket_context *context);

	//初始化socket_context, 可以设置为NULL
	void (*socket_init)(struct ks_socket_container *container, struct ks_socket_context *context);

	//销毁socket_context,可以设置为NULL
	void (*socket_uninit)(struct ks_socket_container *container, struct ks_socket_context *context);

	//建立连接成功通知
	void (*connected)(struct ks_socket_container *container, struct ks_socket_context *socket_context);

	//建立连接失败通知
	void (*connect_failed)(struct ks_socket_container *container, struct ks_socket_context *socket_context, int status);

	//断开连接通知
	void (*disconnected)(struct ks_socket_container *container, struct ks_socket_context *socket_context);

	void (*send_notify)(struct ks_socket_container *container, struct ks_socket_context *context, struct ks_buffer *buffer, int status);

	//收到数据通知, 由此调用received
	void (*arrived)(struct ks_socket_container *container, struct ks_socket_context *socket_context, const char *data, ssize_t nread);

	//用户定义的数据到达通知
	void (*received)(struct ks_socket_container *container, struct ks_socket_context *socket_context, struct ks_buffer *buffer);

	//错误回调函数
	void (*handle_error)(struct ks_socket_container *container, struct ks_socket_context *socket_context, int err);

	void (*handle_serverfull)(struct ks_socket_container *container, struct ks_socket_context *context);
};

struct ks_socket_container
{
	//回调函数
	struct ks_socket_callback *callback;

	//总循环
	uv_loop_t *loop;
	
	//唯一id的迭代
	uint64_t uniqid_iterators;
	
	//已经激活的连接，引用计数器不归0则始终判定为active_connections
	struct list_head active_connections;
	
	//引用计数器归0后未使用的connections
	struct list_head inactive_connections;
	
	//connections的表,用于id查找
	struct ks_table connections;
	
	//ks_buffer的内存池
	struct list_head buffers;
	struct list_head using_buffers;

	//ks_writereq的内存池
	struct list_head writereq_buffers;

	//最大连接数量
	int max_connections;
	int num_connections;
	
	//初始化socket数量
	int init_socket_count;
	
	//当前连接数量
	int current_socket_count;

	int init_buffers_count;
	int init_writereq_count;
	
};

#define align_size(size, align) ((size % align) == 0) ? size : (((size / align) + 1) * align)

/**
 * ks_buffer functions
 */
struct ks_buffer *ks_buffer_create();			//创建一个buffer
void ks_buffer_destroy(struct ks_buffer *buffer);  //直接释放buffer
int64_t ks_buffer_addref(struct ks_buffer *buffer);   //给buffer增加引用计数器
kboolean ks_buffer_decref(struct ks_buffer *buffer); //减少这个buffer缓冲区的引用
void ks_buffer_write(struct ks_buffer *buffer, void *data, size_t size); //将数据写入buffer
void *ks_buffer_getdata(struct ks_buffer *buffer); //获取buffer的数据指针
size_t ks_buffer_size(struct ks_buffer *buffer); //获取buffer的大小
void ks_buffer_reset(struct ks_buffer *buffer); //重置buffer信息释放data2
void ks_buffer_reserve(struct ks_buffer *buffer, size_t size); //预留缓冲区大小

/**
 * ks_circular_buffer functions
 */

void INIT_KS_CIRCULAR_BUFFER(struct ks_circular_buffer *circular_buffer);		//初始化一个circular buffer
void ks_circular_buffer_destroy(struct ks_circular_buffer *circular_buffer);	//销毁circular buffer
void ks_circular_buffer_queue(struct ks_circular_buffer *circular_buffer, const void *data, size_t size);	//追加数据,并移动inpos
void ks_circular_buffer_queue_ks_buffer(struct ks_circular_buffer *circular_buffer, struct ks_buffer *buffer);	//追加一个buffer,并移动inpos
kboolean ks_circular_buffer_peek_array(struct ks_circular_buffer *circular_buffer, void *data, size_t size);	//只获取数据
kboolean ks_circular_buffer_dequeue_array(struct ks_circular_buffer *circular_buffer, void *data, size_t size); //取出数据,并移动outpos
kboolean ks_circular_buffer_empty(struct ks_circular_buffer *circular_buffer);
void ks_circular_buffer_reset(struct ks_circular_buffer *circular_buffer);	//重置circular buffer,释放多余的buffer block
void ks_circular_buffer_addblock(struct ks_circular_buffer *circular_buffer);


/**
 * ks_table functions
 */
void INIT_KS_TABLE(struct ks_table *table, int max_slots);
void ks_table_destroy(struct ks_table *table);
kboolean ks_table_insert(struct ks_table *table, uint64_t id, void* data);
kboolean ks_table_remove(struct ks_table *table, uint64_t id);
void *ks_table_find(struct ks_table *table, uint64_t id);
void ks_table_enum(struct ks_table *table, ks_table_callback cb,void *user_arg);


/**
 * ks_socket_container functions
 */
void INIT_KS_SOCKET_CONTAINER(struct ks_socket_container *container, uv_loop_t *loop, struct ks_socket_callback *callback, int max_connections, int initial_socket_count, int max_slots, int init_buffers_count, int init_writereq_count);

//socket context引用
struct ks_socket_context* ks_socket_refernece(struct ks_socket_container *container, struct ks_socket_context *context);
void ks_socket_derefernece(struct ks_socket_container *container, struct ks_socket_context *context);

//服务器监听函数
int ks_socket_addlistener_ipv4(struct ks_socket_container *container, const char *addr, int port);
int ks_socket_addlistener_ipv6(struct ks_socket_container *container, const char *addr, int port);
int ks_socket_addlistener_pipe(struct ks_socket_container *container, const char *name);

//客户端连接服务器
int ks_socket_connect_ipv4(struct ks_socket_container *container, const char *addr, int port);
int ks_socket_connect_ipv6(struct ks_socket_container *container, const char *addr, int port);
int ks_socket_connect_pipe(struct ks_socket_container *container, const char *name);

//ks_buffer对象池的引用
struct ks_buffer *ks_socket_buffer_refernece(struct ks_socket_container *container, struct ks_buffer *buffer);
void ks_socket_buffer_derefernece(struct ks_socket_container *container, struct ks_buffer *buffer);

//发送数据包
int ks_socket_send(struct ks_socket_context *context, struct ks_buffer *buffer);

//发送EOF到远程，通知连接断开，此操作会将数据完整的发送到客户端后进行断开
int ks_socket_shutdown(struct ks_socket_context *context);

//直接关闭连接，此操作不会将数据发送到客户端而是直接断开
int ks_socket_close(struct ks_socket_context *context);

//停止所有服务器对象
int ks_socket_stop(struct ks_socket_container *container);

//获取一个服务器socket对象
struct ks_socket_context *ks_socket_find(struct ks_socket_container *container, uint64_t uniqid);

kboolean ks_socket_getpeername(const struct ks_socket_context *context, struct ks_netadr *netadr);
kboolean ks_socket_getsockname(const struct ks_socket_context *context, struct ks_netadr *netadr);

#endif
