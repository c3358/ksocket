#ifndef _KSOCKET_INCLUDED_H_
#define _KSOCKET_INCLUDED_H_

#ifdef __cplusplus
extern "C"
{

#endif

#ifdef _WIN32

#define bzero ZeroMemory

struct  sockaddr_un {
        unsigned char   sun_len;        /* sockaddr len including null */
        u_short     sun_family;     /* [XSI] AF_UNIX */
        char            sun_path[104];  /* [XSI] path name (gag) */
};


#endif

typedef int kboolean;

//套接字接收数据的缓冲区
#define KS_SOCKET_CONTEXT_RDBUF_SIZE 4096
//环形BUFFER的数据缓冲大小
#define KS_CIRCULAR_BUFFER_BLOCK_SIZE 4096
//普通buffer的默认数据大小
#define KS_BUFFER_DEFAULT_DATA_SIZE 512

#define KS_REMOTE_ADDRESS_NONE 0
#define KS_REMOTE_ADDRESS_PIPE 1
#define KS_REMOTE_ADDRESS_IPV4 2
#define KS_REMOTE_ADDRESS_IPV6 3

#define KS_ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

struct ks_remoteaddress
{
    int type;
    const char *host;
    int port;
};

void init_remote_address_pipe(struct ks_remoteaddress *remoteaddress, const char *path);
void init_remote_address_ipv4(struct ks_remoteaddress *remoteaddress, const char *host, int port);
void init_remote_address_ipv6(struct ks_remoteaddress *remoteaddress, const char *host, int port);

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
    union {
        uv_handle_t handle;
        uv_stream_t stream;
        uv_tcp_t tcp;
        uv_pipe_t pipe;
    };
};

enum ks_socket_status
{
    KS_SOCKET_STATUS_UNINITIALIZE, //没初始化uv_handle_t的状态
    KS_SOCKET_STATUS_INITIALIZED,  //初始化uv_handle_t后的状态
    KS_SOCKET_STATUS_CONNECTING,   //建立连接中
    KS_SOCKET_STATUS_LISTEN,       //监听中
    KS_SOCKET_STATUS_ESTABLISHED,  //已经建立连接
    KS_SOCKET_STATUS_SHUTTINGDOWN, //正在执行shutdown操作
    KS_SOCKET_STATUS_SHUTDOWN,     //shutdown操作执行完成
    KS_SOCKET_STATUS_CLOSING,      //正在执行close操作
    KS_SOCKET_STATUS_CLOSED        //close操作执行完成
};

struct ks_netadr
{
    union {
        struct sockaddr addr;
        struct sockaddr_in addr_ipv4;
        struct sockaddr_in6 addr_ipv6;
        struct sockaddr_un addr_unix;
    };
};

enum ks_protobyte_type
{
    ks_protobyte_type_null,
    ks_protobyte_type_bool,
    ks_protobyte_type_char,
    ks_protobyte_type_uchar,
    ks_protobyte_type_int32,
    ks_protobyte_type_uint32,
    ks_protobyte_type_int64,
    ks_protobyte_type_uint64,
    ks_protobyte_type_float,
    ks_protobyte_type_double,
    ks_protobyte_type_string,
    ks_protobyte_type_blob,
    ks_protobyte_type_array     //object array
};

#define KS_PROTOBYTE_DEFAULTS_COUNT 100

struct ks_protobyte_data
{
    struct list_head entry;
    unsigned char type;
    unsigned char alloc;

    //using as string or blob
    int n_buflen;
    void *buf;

    //using as bool, (u)int32(64), float, double, char, uchar
    union
    {
        unsigned char v_bool;
        char v_char;
        unsigned char v_uchar;
        int32_t v_int32;
        uint32_t v_uint32;
        float v_float;
        int64_t v_int64;
        uint64_t v_uint64;
        double v_double;
    };

    //using as array
    int nelts;
    uint32_t size;
    void *elts;
};

struct ks_protobyte
{
    struct list_head head;
    size_t usingsize;
    size_t members_count;

    struct list_head freelist;
    struct ks_protobyte_data defaults[KS_PROTOBYTE_DEFAULTS_COUNT];
};

void INIT_KS_PROTOBYTE(struct ks_protobyte *protobyte);
void ks_protobyte_destroy(struct ks_protobyte *protobyte);
void ks_protobyte_push_bool(struct ks_protobyte *protobyte, unsigned char v);
void ks_protobyte_push_char(struct ks_protobyte *protobyte, char v);
void ks_protobyte_push_uchar(struct ks_protobyte *protobyte, unsigned char v);
void ks_protobyte_push_int32(struct ks_protobyte *protobyte, int32_t v);
void ks_protobyte_push_uint32(struct ks_protobyte *protobyte, uint32_t v);
void ks_protobyte_push_int64(struct ks_protobyte *protobyte, int64_t v);
void ks_protobyte_push_uint64(struct ks_protobyte *protobyte, uint64_t v);
void ks_protobyte_push_float(struct ks_protobyte *protobyte, float v);
void ks_protobyte_push_double(struct ks_protobyte *protobyte, double v);
void ks_protobyte_push_string(struct ks_protobyte *protobyte, const char *v);
void ks_protobyte_push_blob(struct ks_protobyte *protobyte, void *data, int length);
void ks_protobyte_push_array(struct ks_protobyte *protobyte, void *elts, size_t size, int nelts);
size_t ks_protobyte_size(struct ks_protobyte *protobyte);
size_t ks_protobyte_serialize_as_array(struct ks_protobyte *protobyte, void *data, size_t length);
void* ks_protobyte_serialize(struct ks_protobyte *protobyte, size_t *length);

struct ks_protobyte_reader
{
    void *data;
    size_t length;
    unsigned char *head;
    unsigned char *tail;
    unsigned char *pos;
    size_t npos;
};

void INIT_KS_PROTOBYTE_READER(struct ks_protobyte_reader *reader, void *data, size_t length);
kboolean ks_protobyte_read_bool(struct ks_protobyte_reader *reader, unsigned char *v);
kboolean ks_protobyte_read_char(struct ks_protobyte_reader *reader, char *v);
kboolean ks_protobyte_read_uchar(struct ks_protobyte_reader *reader, unsigned char *v);
kboolean ks_protobyte_read_int32(struct ks_protobyte_reader *reader, int32_t *v);
kboolean ks_protobyte_read_uint32(struct ks_protobyte_reader *reader, uint32_t *v);
kboolean ks_protobyte_read_int64(struct ks_protobyte_reader *reader, int64_t *v);
kboolean ks_protobyte_read_uint64(struct ks_protobyte_reader *reader, uint64_t *v);
kboolean ks_protobyte_read_float(struct ks_protobyte_reader *reader, float *v);
kboolean ks_protobyte_read_double(struct ks_protobyte_reader *reader,double *v);
kboolean ks_protobyte_read_string(struct ks_protobyte_reader *reader, char **v);
kboolean ks_protobyte_read_blob(struct ks_protobyte_reader *reader, void **v, size_t *length);
kboolean ks_protobyte_read_array(struct ks_protobyte_reader *reader, void **elts, size_t *size, int *nelts);


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

struct ks_locked_queue
{
    struct list_head head;
    size_t size;
    uv_mutex_t mutex;
};

#define KS_QUEUE_THREAD_FLAG_EXIT 0
#define KS_QUEUE_THREAD_FLAG_POST 1

struct ks_queue_thread_order
{
    struct list_head entry;
    int flag;
};

typedef void (*ks_queue_thread_processorder)(struct ks_queue_thread_order *order);
typedef void (*ks_queue_thread_completeorder)(struct ks_queue_thread_order *order);
typedef void (*ks_queue_thread_free_entry)(struct ks_queue_thread_order *order);

struct ks_queue_thread
{
    struct ks_locked_queue input_locked_queue;
    struct ks_locked_queue output_locked_queue;
    struct ks_queue_thread_order exitorder;
    uv_loop_t *loop;
    kboolean started;
    uv_thread_t thread;
    uv_async_t async_notify;
    uv_sem_t semaphore;
    ks_queue_thread_processorder processorder;
    ks_queue_thread_completeorder completeorder;
    ks_queue_thread_free_entry freeentry;
    size_t input_queue_maxcount;
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
    
    //数据发送完成通知
    void (*send_notify)(struct ks_socket_container *container, struct ks_socket_context *context, struct ks_buffer *buffer, int status);
    
    //收到数据通知, 由此调用received
    void (*arrived)(struct ks_socket_container *container, struct ks_socket_context *socket_context, const char *data, ssize_t nread);
    
    //用户定义的数据到达通知
    void (*received)(struct ks_socket_container *container, struct ks_socket_context *socket_context, struct ks_buffer *buffer);
    
    //错误回调函数
    void (*handle_error)(struct ks_socket_container *container, struct ks_socket_context *socket_context, int err);
    
    //服务器已满处理函数
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

struct ks_buffer_reader
{
    void *data;
    size_t totalsize;
    size_t pos;
};

#define align_size(size, align) ((size % align) == 0) ? size : (((size / align) + 1) * align)

/**
 * ks_buffer functions
 */
struct ks_buffer *ks_buffer_create();                                    //创建一个buffer
void ks_buffer_destroy(struct ks_buffer *buffer);                        //直接释放buffer
int64_t ks_buffer_addref(struct ks_buffer *buffer);                      //给buffer增加引用计数器
kboolean ks_buffer_decref(struct ks_buffer *buffer);                     //减少这个buffer缓冲区的引用
void ks_buffer_write(struct ks_buffer *buffer, void *data, size_t size); //将数据写入buffer
void *ks_buffer_getdata(struct ks_buffer *buffer);                       //获取buffer的数据指针
size_t ks_buffer_size(struct ks_buffer *buffer);                         //获取buffer的大小
void ks_buffer_reset(struct ks_buffer *buffer);                          //重置buffer信息释放data2
void ks_buffer_reserve(struct ks_buffer *buffer, size_t size);           //预留缓冲区大小
void ks_buffer_setsize(struct ks_buffer *buffer, size_t size);           //设置缓冲区大小

/**
 * ks_buffer_reader functions
 */
void INIT_KS_BUFFER_READER(struct ks_buffer_reader *reader, void *data, size_t length);
kboolean ks_buffer_reader_peek(struct ks_buffer_reader *reader, void *data, size_t length);
kboolean ks_buffer_reader_read(struct ks_buffer_reader *reader, void *data, size_t length);
kboolean ks_buffer_reader_seek(struct ks_buffer_reader *reader, size_t position);
kboolean ks_buffer_reader_ignore(struct ks_buffer_reader *reader, size_t offset);
void *ks_buffer_reader_getpos(struct ks_buffer_reader *reader);
size_t ks_buffer_reader_unread_bytes(struct ks_buffer_reader *reader);
kboolean ks_buffer_reader_iseof(struct ks_buffer_reader *reader);

/**
 * ks_circular_buffer functions
 */

void INIT_KS_CIRCULAR_BUFFER(struct ks_circular_buffer *circular_buffer);                                       //初始化一个circular buffer
void ks_circular_buffer_destroy(struct ks_circular_buffer *circular_buffer);                                    //销毁circular buffer
void ks_circular_buffer_queue(struct ks_circular_buffer *circular_buffer, const void *data, size_t size);       //追加数据,并移动inpos
void ks_circular_buffer_queue_ks_buffer(struct ks_circular_buffer *circular_buffer, struct ks_buffer *buffer);  //追加一个buffer,并移动inpos
kboolean ks_circular_buffer_peek_array(struct ks_circular_buffer *circular_buffer, void *data, size_t size);    //只获取数据
kboolean ks_circular_buffer_dequeue_array(struct ks_circular_buffer *circular_buffer, void *data, size_t size); //取出数据,并移动outpos
kboolean ks_circular_buffer_empty(struct ks_circular_buffer *circular_buffer);
void ks_circular_buffer_reset(struct ks_circular_buffer *circular_buffer); //重置circular buffer,释放多余的buffer block
void ks_circular_buffer_addblock(struct ks_circular_buffer *circular_buffer);

/**
 * ks_table functions
 */
void INIT_KS_TABLE(struct ks_table *table, int max_slots);
void ks_table_destroy(struct ks_table *table);
kboolean ks_table_insert(struct ks_table *table, uint64_t id, void *data);
kboolean ks_table_remove(struct ks_table *table, uint64_t id);
void *ks_table_find(struct ks_table *table, uint64_t id);
void ks_table_enum(struct ks_table *table, ks_table_callback cb, void *user_arg);




/*
 * locked queue functions
 */
void INIT_KS_LOCKED_QUEUE(struct ks_locked_queue *locked_queue);
void ks_locked_queue_push_front(struct ks_locked_queue *locked_queue, struct list_head *entry);
void ks_locked_queue_push_back(struct ks_locked_queue *locked_queue, struct list_head *entry);
kboolean ks_locked_queue_empty(struct ks_locked_queue *locked_queue);
size_t ks_locked_queue_size(struct ks_locked_queue *locked_queue);
struct list_head *ks_locked_queue_pop_front(struct ks_locked_queue *locked_queue);
struct list_head *ks_locked_queue_pop_back(struct ks_locked_queue *locked_queue);
void ks_locked_queue_destroy(struct ks_locked_queue *locked_queue);


/*
 * locked queue thread functions
 */
void INIT_KS_QUEUE_THREAD(  struct ks_queue_thread *thread,
                            uv_loop_t *loop,
                            size_t input_queue_maxcount, 
                            ks_queue_thread_processorder processorder,
                            ks_queue_thread_completeorder completeorder,
                            ks_queue_thread_free_entry freeentry
);

void ks_queue_thread_start(struct ks_queue_thread *thread);
void ks_queue_thread_stop(struct ks_queue_thread *thread);
kboolean ks_queue_thread_post(struct ks_queue_thread *thread, struct ks_queue_thread_order *entry);
size_t ks_socket_thread_input_size(struct ks_queue_thread *thread);
size_t ks_socket_thread_output_size(struct ks_queue_thread *thread);
void ks_queue_thread_destroy(struct ks_queue_thread *thread);


/**
 * ks_socket_container functions
 */
void INIT_KS_SOCKET_CONTAINER(struct ks_socket_container *container, uv_loop_t *loop, struct ks_socket_callback *callback, int max_connections, int initial_socket_count, int max_slots, int init_buffers_count, int init_writereq_count);

//socket context引用
struct ks_socket_context *ks_socket_refernece(struct ks_socket_container *container, struct ks_socket_context *context);
void ks_socket_derefernece(struct ks_socket_container *container, struct ks_socket_context *context);

//服务器监听函数
int ks_socket_addlistener_ipv4(struct ks_socket_container *container, const char *addr, int port);
int ks_socket_addlistener_ipv6(struct ks_socket_container *container, const char *addr, int port);
int ks_socket_addlistener_pipe(struct ks_socket_container *container, const char *name);

//客户端连接服务器
int ks_socket_connect_ipv4(struct ks_socket_container *container, const char *addr, int port, struct ks_socket_context **pcontext);
int ks_socket_connect_ipv6(struct ks_socket_container *container, const char *addr, int port, struct ks_socket_context **pcontext);
int ks_socket_connect_pipe(struct ks_socket_container *container, const char *name, struct ks_socket_context **pcontext);

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

#ifdef __cplusplus
};
#endif

#endif
