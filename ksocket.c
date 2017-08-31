#include "ksocket.h"


void INIT_KS_PROTOBYTE(struct ks_protobyte *protobyte)
{
    int i;
    INIT_LIST_HEAD(&protobyte->head);
    INIT_LIST_HEAD(&protobyte->freelist);
    bzero(&protobyte->defaults, sizeof(protobyte->defaults));
    protobyte->usingsize = 0;
    protobyte->members_count = 0;

    for(i = 0; i < KS_ARRAY_SIZE(protobyte->defaults); i++)
    {
        list_add_tail(&protobyte->defaults[i].entry, &protobyte->freelist);
    }
}

struct ks_protobyte_data *ks_protobyte_nextdata(struct ks_protobyte *protobyte)
{
    struct ks_protobyte_data *entry;
    if(!list_empty(&protobyte->freelist))
    {
        entry = list_first_entry(&protobyte->freelist, struct ks_protobyte_data, entry);
        list_del(&entry->entry);
        entry->alloc = 0;
        return entry;
    }

    entry = calloc(1, sizeof(struct ks_protobyte_data));
    entry->alloc = 1;
    return entry;
}

void ks_protobyte_destroy(struct ks_protobyte *protobyte)
{
    struct ks_protobyte_data *protobyte_data;
    while(!list_empty(&protobyte->head))
    {
        protobyte_data = list_first_entry(&protobyte->head, struct ks_protobyte_data, entry);
        list_del(&protobyte_data->entry);


        if(protobyte_data->type == ks_protobyte_type_string || protobyte_data->type == ks_protobyte_type_blob)
        {
            free(protobyte_data->buf);
            protobyte_data->buf = NULL;
        }

        if(protobyte_data->type == ks_protobyte_type_array)
        {
            free(protobyte_data->elts);
            protobyte_data->elts = NULL;
        }

        if(protobyte_data->alloc)
        {
            free(protobyte_data);
        }
    }

    INIT_LIST_HEAD(&protobyte->head);
    protobyte->usingsize = 0;
    protobyte->members_count = 0;
}

void ks_protobyte_push_bool(struct ks_protobyte *protobyte, unsigned char v)
{
    struct ks_protobyte_data *protobyte_data = ks_protobyte_nextdata(protobyte);

    protobyte_data->type = ks_protobyte_type_bool;
    protobyte_data->v_bool = v;

    list_add_tail(&protobyte_data->entry, &protobyte->head);
    protobyte->usingsize += sizeof(unsigned char) + sizeof(v);
    protobyte->members_count++;
}

void ks_protobyte_push_char(struct ks_protobyte *protobyte, char v)
{
    struct ks_protobyte_data *protobyte_data = ks_protobyte_nextdata(protobyte);

    protobyte_data->type = ks_protobyte_type_char;
    protobyte_data->v_char = v;

    list_add_tail(&protobyte_data->entry, &protobyte->head);
    protobyte->usingsize += sizeof(unsigned char) + sizeof(v);
    protobyte->members_count++;
}

void ks_protobyte_push_uchar(struct ks_protobyte *protobyte, unsigned char v)
{
    struct ks_protobyte_data *protobyte_data = ks_protobyte_nextdata(protobyte);

    protobyte_data->type = ks_protobyte_type_uchar;
    protobyte_data->v_uchar = v;

    list_add_tail(&protobyte_data->entry, &protobyte->head);
    protobyte->usingsize += sizeof(unsigned char) + sizeof(v);
    protobyte->members_count++;
}

void ks_protobyte_push_int32(struct ks_protobyte *protobyte, int32_t v)
{
    struct ks_protobyte_data *protobyte_data = ks_protobyte_nextdata(protobyte);

    protobyte_data->type = ks_protobyte_type_int32;
    protobyte_data->v_int32 = v;


    list_add_tail(&protobyte_data->entry, &protobyte->head);
    protobyte->usingsize += sizeof(unsigned char) + sizeof(v);
    protobyte->members_count++;
}

void ks_protobyte_push_uint32(struct ks_protobyte *protobyte, uint32_t v)
{
    struct ks_protobyte_data *protobyte_data = ks_protobyte_nextdata(protobyte);

    protobyte_data->type = ks_protobyte_type_uint32;
    protobyte_data->v_uint32 = v;


    list_add_tail(&protobyte_data->entry, &protobyte->head);
    protobyte->usingsize += sizeof(unsigned char) + sizeof(v);
    protobyte->members_count++;
}

void ks_protobyte_push_int64(struct ks_protobyte *protobyte, int64_t v)
{
    struct ks_protobyte_data *protobyte_data = ks_protobyte_nextdata(protobyte);

    protobyte_data->type = ks_protobyte_type_int64;
    protobyte_data->v_int64 = v;

    list_add_tail(&protobyte_data->entry, &protobyte->head);
    protobyte->usingsize += sizeof(unsigned char) + sizeof(v);
    protobyte->members_count++;
}

void ks_protobyte_push_uint64(struct ks_protobyte *protobyte, uint64_t v)
{
    struct ks_protobyte_data *protobyte_data = ks_protobyte_nextdata(protobyte);

    protobyte_data->type = ks_protobyte_type_uint64;
    protobyte_data->v_uint64 = v;

    list_add_tail(&protobyte_data->entry, &protobyte->head);
    protobyte->usingsize += sizeof(unsigned char) + sizeof(v);
    protobyte->members_count++;
}

void ks_protobyte_push_float(struct ks_protobyte *protobyte, float v)
{
    struct ks_protobyte_data *protobyte_data = ks_protobyte_nextdata(protobyte);

    protobyte_data->type = ks_protobyte_type_float;
    protobyte_data->v_float = v;

    list_add_tail(&protobyte_data->entry, &protobyte->head);
    protobyte->usingsize += sizeof(unsigned char) + sizeof(v);
    protobyte->members_count++;
}

void ks_protobyte_push_double(struct ks_protobyte *protobyte, double v)
{
    struct ks_protobyte_data *protobyte_data = ks_protobyte_nextdata(protobyte);

    protobyte_data->type = ks_protobyte_type_double;
    protobyte_data->v_double = v;

    list_add_tail(&protobyte_data->entry, &protobyte->head);
    protobyte->usingsize += sizeof(unsigned char) + sizeof(v);
    protobyte->members_count++;
}

void ks_protobyte_push_string(struct ks_protobyte *protobyte, const char *v)
{
    struct ks_protobyte_data *protobyte_data = ks_protobyte_nextdata(protobyte);

    
    protobyte_data->type = ks_protobyte_type_string;
    protobyte_data->n_buflen = strlen(v) + sizeof(char);
    protobyte_data->buf = malloc(protobyte_data->n_buflen);
    memcpy(protobyte_data->buf, v, protobyte_data->n_buflen);

    list_add_tail(&protobyte_data->entry, &protobyte->head);
    //  [unsigned char] -> type
    //  [int] -> size
    //  [data] -> v
    protobyte->usingsize += sizeof(unsigned char) + sizeof(int) + protobyte_data->n_buflen;
    protobyte->members_count++;
}

void ks_protobyte_push_blob(struct ks_protobyte *protobyte, void *data, int length)
{
    struct ks_protobyte_data *protobyte_data = ks_protobyte_nextdata(protobyte);

    protobyte_data->type = ks_protobyte_type_blob;
    protobyte_data->n_buflen = length;
    protobyte_data->buf = malloc(length);
    memcpy(protobyte_data->buf, data, protobyte_data->n_buflen);

    list_add_tail(&protobyte_data->entry, &protobyte->head);
    //  [unsigned char] -> type
    //  [int] -> size
    //  [data] -> data
    protobyte->usingsize += sizeof(unsigned char) + sizeof(int) + protobyte_data->n_buflen;
    protobyte->members_count++;
}

void ks_protobyte_push_array(struct ks_protobyte *protobyte, void *elts, size_t size, int nelts)
{
    struct ks_protobyte_data *protobyte_data = ks_protobyte_nextdata(protobyte);

    protobyte_data->type = ks_protobyte_type_array;
    protobyte_data->nelts = nelts;
    protobyte_data->size = (uint32_t)size;
    protobyte_data->elts = calloc(nelts, size);
    memcpy(protobyte_data->elts, elts, size * nelts);

    list_add_tail(&protobyte_data->entry, &protobyte->head);

    //  [unsigned char] -> type
    //  [int] -> nelts
    //  [uint32] -> size
    //  [data] -> elts
    protobyte->usingsize += sizeof(unsigned char) + sizeof(int) + sizeof(uint32_t) + (size * nelts);
    protobyte->members_count++;
}

size_t ks_protobyte_size(struct ks_protobyte *protobyte)
{
    return protobyte->usingsize;
}

size_t ks_protobyte_data_size(struct ks_protobyte_data *data)
{
    size_t n = 0;

    switch(data->type)
    {
        case ks_protobyte_type_bool:
        {
            n = sizeof(unsigned char) + sizeof(unsigned char);
            break;
        }
        case ks_protobyte_type_char:
        {
            n = sizeof(unsigned char) + sizeof(char);
            break;
        }
        case ks_protobyte_type_uchar:
        {
            n = sizeof(unsigned char) + sizeof(unsigned char);
            break;
        }
        case ks_protobyte_type_int32:
        {
            n = sizeof(unsigned char) + sizeof(int32_t);
            break;
        }
        case ks_protobyte_type_uint32:
        {
            n = sizeof(unsigned char) + sizeof(uint32_t);
            break;
        }
        case ks_protobyte_type_int64:
        {
            n = sizeof(unsigned char) + sizeof(int64_t);
            break;
        }
        case ks_protobyte_type_uint64:
        {
            n = sizeof(unsigned char) + sizeof(uint64_t);
            break;
        }
        case ks_protobyte_type_float:
        {
            n = sizeof(unsigned char) + sizeof(float);
            break;
        }
        case ks_protobyte_type_double:
        {
            n = sizeof(unsigned char) + sizeof(double);
            break;
        }
        case ks_protobyte_type_string:
        case ks_protobyte_type_blob:
        {
            n = sizeof(unsigned char) + sizeof(int) + data->n_buflen;
            break;
        }
        case ks_protobyte_type_array:
        {
            n = sizeof(unsigned char) + sizeof(int) + sizeof(uint32_t) + (data->size * data->nelts);;
            break;
        }
        default:
            break;
    }

    return n;
}

size_t ks_protobyte_serialize_as_array(struct ks_protobyte *protobyte, void *data, size_t length)
{
    size_t serialize_size;
    size_t block_size;
    unsigned char *next;
    struct ks_protobyte_data *pos;
    serialize_size = 0;

    next = data;

    list_for_each_entry(pos, &protobyte->head, entry)
    {
        block_size = ks_protobyte_data_size(pos);
        if((block_size + serialize_size) > length)
            return 0;
        next[0] = pos->type;
        switch(pos->type)
        {
            case ks_protobyte_type_bool:
            {
                *(unsigned char *)&next[1] = pos->v_bool;
                break;
            }
            case ks_protobyte_type_char:
            {
                *(char *)&next[1] = pos->v_char;
                break;
            }
            case ks_protobyte_type_uchar:
            {
                *(unsigned char *)&next[1] = pos->v_uchar;
                break;
            }
            case ks_protobyte_type_int32:
            {
                *(int32_t *)&next[1] = pos->v_int32;
                break;
            }
            case ks_protobyte_type_uint32:
            {
                *(uint32_t *)&next[1] = pos->v_uint32;
                break;
            }
            case ks_protobyte_type_int64:
            {
                *(int64_t *)&next[1] = pos->v_int64;
                break;
            }
            case ks_protobyte_type_uint64:
            {
                *(uint64_t *)&next[1] = pos->v_uint64;
                break;
            }
            case ks_protobyte_type_float:
            {
                *(float *)&next[1] = pos->v_float;
                break;
            }
            case ks_protobyte_type_double:
            {
                *(double *)&next[1] = pos->v_double;
                break;
            }
            case ks_protobyte_type_string:
            case ks_protobyte_type_blob:
            {
                //[int]
                *(int *)&next[1] = pos->n_buflen;
                //[data]
                memcpy(&next[1 + sizeof(int)], pos->buf, pos->n_buflen);
                break;
            }
            case ks_protobyte_type_array:
            {
                //[data] -> elts
                *(int *)&next[1] = pos->nelts;
                //[uint32] -> size
                *(uint32_t *)&next[1 + sizeof(int)] = pos->size;

                //[int] -> nelts
                memcpy(&next[1 + sizeof(int) + sizeof(uint32_t)], pos->elts, pos->nelts * pos->size);
                break;
            }
            default:
                break;
        }

        next += block_size;
        serialize_size += block_size;
    }

    return serialize_size;
}

void* ks_protobyte_serialize(struct ks_protobyte *protobyte, size_t *length)
{
    void *data;
    size_t compile_size;
    compile_size = ks_protobyte_size(protobyte);
    data = malloc(compile_size);
    length[0] = ks_protobyte_serialize_as_array(protobyte, data, compile_size);
    return data;
}



void INIT_KS_PROTOBYTE_READER(struct ks_protobyte_reader *reader, void *data, size_t length)
{
    reader->data = data;
    reader->length = length;
    reader->head = data;
    reader->tail = data + length;
    reader->pos = reader->head;
    reader->npos = 0;
}

bool ks_protobyte_read_bool(struct ks_protobyte_reader *reader, unsigned char *v)
{
    size_t n = sizeof(unsigned char) + sizeof(unsigned char);
    
    if((reader->pos + n) > reader->tail)
    {
        return 0;
    }
    
    if(reader->pos[0] != ks_protobyte_type_bool)
    {
        return 0;
    }
    
    *v = reader->pos[1];
    
    reader->pos += n;
    reader->npos += n;
    return 1;
}


bool ks_protobyte_read_char(struct ks_protobyte_reader *reader, char *v)
{
    size_t n = sizeof(char) + sizeof(char);    
    if((reader->pos + n) > reader->tail)
    {
        return 0;
    }
    if(reader->pos[0] != ks_protobyte_type_char)
    {
        return 0;
    }
    *v = reader->pos[1];
    reader->pos += n;
    reader->npos += n;
    return 1;
}



bool ks_protobyte_read_uchar(struct ks_protobyte_reader *reader, unsigned char *v)
{
    size_t n = sizeof(unsigned char) + sizeof(unsigned char);
    if((reader->pos + n) > reader->tail)
    {
        return 0;
    }
    if(reader->pos[0] != ks_protobyte_type_uchar)
    {
        return 0;
    }
    *v = reader->pos[1];
    reader->pos += n;
    reader->npos += n;
    return 1;
}


bool ks_protobyte_read_int32(struct ks_protobyte_reader *reader, int32_t *v)
{
    size_t n = sizeof(unsigned char) + sizeof(int32_t);
    if((reader->pos + n) > reader->tail)
    {
        return 0;
    }
    if(reader->pos[0] != ks_protobyte_type_int32)
    {
        return 0;
    }
    *v = *(int32_t *)&reader->pos[1];
    reader->pos += n;
    reader->npos += n;
    return 1;
}



bool ks_protobyte_read_uint32(struct ks_protobyte_reader *reader, uint32_t *v)
{
    size_t n = sizeof(unsigned char) + sizeof(uint32_t);
    if((reader->pos + n) > reader->tail)
    {
        return 0;
    }
    if(reader->pos[0] != ks_protobyte_type_uint32)
    {
        return 0;
    }
    *v = *(uint32_t *)&reader->pos[1];
    reader->pos += n;
    reader->npos += n;
    return 1;
}


bool ks_protobyte_read_int64(struct ks_protobyte_reader *reader, int64_t *v)
{
    size_t n = sizeof(unsigned char) + sizeof(int64_t);
    if((reader->pos + n) > reader->tail)
    {
        return 0;
    }
    if(reader->pos[0] != ks_protobyte_type_int64)
    {
        return 0;
    }
    *v = *(int64_t *)&reader->pos[1];
    reader->pos += n;
    reader->npos += n;
    return 1;
}


bool ks_protobyte_read_uint64(struct ks_protobyte_reader *reader, uint64_t *v)
{
    size_t n = sizeof(unsigned char) + sizeof(uint64_t);
    if((reader->pos + n) > reader->tail)
    {
        return 0;
    }
    if(reader->pos[0] != ks_protobyte_type_uint64)
    {
        return 0;
    }
    *v = *(uint64_t *)&reader->pos[1];
    reader->pos += n;
    reader->npos += n;
    return 1;
}

bool ks_protobyte_read_float(struct ks_protobyte_reader *reader, float *v)
{
    size_t n = sizeof(unsigned char) + sizeof(float);
    if((reader->pos + n) > reader->tail)
    {
        return 0;
    }
    if(reader->pos[0] != ks_protobyte_type_float)
    {
        return 0;
    }
    *v = *(float *)&reader->pos[1];
    reader->pos += n;
    reader->npos += n;
    return 1;
}


bool ks_protobyte_read_double(struct ks_protobyte_reader *reader,double *v)
{
    size_t n = sizeof(unsigned char) + sizeof(double);
    if((reader->pos + n) > reader->tail)
    {
        return 0;
    }
    if(reader->pos[0] != ks_protobyte_type_double)
    {
        return 0;
    }
    *v = *(double *)&reader->pos[1];
    reader->pos += n;
    reader->npos += n;
    return 1;
}

bool ks_protobyte_read_string(struct ks_protobyte_reader *reader, char **v)
{
    size_t n = sizeof(unsigned char) + sizeof(int);
    int n_buflen;
    char *temp;
    
    if((reader->pos + n) > reader->tail)
    {
        return 0;
    }
    
    
    n_buflen = *(int *)&reader->pos[sizeof(unsigned char)];
    if(n_buflen <= 0 || (reader->pos + n + n_buflen) > reader->tail)
    {
        return 0;
    }
    
    temp = malloc(n_buflen);
    memcpy(temp, &reader->pos[sizeof(unsigned char) + sizeof(int)], n_buflen);
    *v = temp;
    
    temp[n_buflen - sizeof(char)] = '\0';
    
    reader->pos += n + n_buflen;
    reader->npos += n + n_buflen;
    return 1;
}

bool ks_protobyte_read_blob(struct ks_protobyte_reader *reader, void **v, size_t *length)
{
    size_t n = sizeof(unsigned char) + sizeof(int);
    int n_buflen;
    void *temp;
    
    if((reader->pos + n) > reader->tail)
    {
        return 0;
    }
    
    
    n_buflen = *(int *)&reader->pos[sizeof(unsigned char)];
    if(n_buflen <= 0 || (reader->pos + n + n_buflen) > reader->tail)
    {
        return 0;
    }
    
    temp = malloc(n_buflen);
    memcpy(temp, &reader->pos[sizeof(unsigned char) + sizeof(int)], n_buflen);
    *v = temp;
    *length = n_buflen;
    
    reader->pos += n + n_buflen;
    reader->npos += n + n_buflen;
    return 1;
}

bool ks_protobyte_read_array(struct ks_protobyte_reader *reader, void **elts, size_t *size, int *nelts)
{
    int _nelts;
    uint32_t _size;
    size_t chunk_size;
    size_t n = sizeof(unsigned char) + sizeof(int) + sizeof(uint32_t);
    if((reader->pos + n) > reader->tail)
    {
        return 0;
    }
    
    _nelts = *(int *)&reader->pos[sizeof(unsigned char)];
    _size = *(uint32_t *)&reader->pos[sizeof(unsigned char) + sizeof(int)];
    
    chunk_size = _nelts * _size;
    
    if(chunk_size > 0x7fffffff)
    {
        return 0;
    }
    
    if((reader->pos + n + chunk_size) > reader->tail)
    {
        return 0;
    }
    *nelts = _nelts;
    *size = _size;
    *elts = malloc(chunk_size);
    memcpy(*elts, &reader->pos[sizeof(unsigned char) + sizeof(int) + sizeof(uint32_t)], chunk_size);
    
    reader->pos += n + chunk_size;
    reader->npos += n + chunk_size;
    return 1;
}

void init_remote_address_pipe(struct ks_remoteaddress *remoteaddress, const char *path)
{
    remoteaddress->type = KS_REMOTE_ADDRESS_PIPE;
    remoteaddress->host = path;
    remoteaddress->port = 0;
}

void init_remote_address_ipv4(struct ks_remoteaddress *remoteaddress, const char *host, int port)
{
    remoteaddress->type = KS_REMOTE_ADDRESS_IPV4;
    remoteaddress->host = host;
    remoteaddress->port = port;
}

void init_remote_address_ipv6(struct ks_remoteaddress *remoteaddress, const char *host, int port)
{
    remoteaddress->type = KS_REMOTE_ADDRESS_IPV6;
    remoteaddress->host = host;
    remoteaddress->port = port;
}

struct ks_buffer *ks_buffer_create()
{
    struct ks_buffer *buffer = calloc(1, sizeof(struct ks_buffer));
    
    buffer->totalsize = sizeof(buffer->data);
    
    return buffer;
}

void ks_buffer_destroy(struct ks_buffer *buffer)
{
    if (buffer->data2)
    {
        free(buffer->data2);
        buffer->data2 = NULL;
    }
    
    free(buffer);
}

int64_t ks_buffer_addref(struct ks_buffer *buffer)
{
    int64_t refcount;
    
    refcount = __sync_add_and_fetch(&buffer->refcount, 1);
    
    if (refcount == 1)
    {
        ks_buffer_reset(buffer);
    }
    
    return refcount;
}

bool ks_buffer_decref(struct ks_buffer *buffer)
{
    if (__sync_sub_and_fetch(&buffer->refcount, 1) <= 0)
    {
        return 1;
    }
    
    return 0;
}

void ks_buffer_reserve(struct ks_buffer *buffer, size_t size)
{
    size_t totalsize;
    
    totalsize = buffer->usingsize + size;
    
    if (buffer->data2)
    {
        if (totalsize > buffer->totalsize)
        {
            buffer->totalsize = align_size(totalsize, KS_BUFFER_DEFAULT_DATA_SIZE);
            buffer->data2 = realloc(buffer->data2, buffer->totalsize);
        }
    }
    else
    {
        if (buffer->totalsize < totalsize)
        {
            buffer->totalsize = align_size(totalsize, KS_BUFFER_DEFAULT_DATA_SIZE);
            buffer->data2 = malloc(buffer->totalsize);
            memcpy(buffer->data2, buffer->data, buffer->usingsize);
        }
    }
}

void ks_buffer_setsize(struct ks_buffer *buffer, size_t size)
{
    ks_buffer_reserve(buffer, size);
    buffer->usingsize = size;
}

void ks_buffer_write(struct ks_buffer *buffer, void *data, size_t size)
{
    unsigned char *dst;
    
    ks_buffer_reserve(buffer, size);
    
    dst = ks_buffer_getdata(buffer);
    dst += buffer->usingsize;
    
    memcpy(dst, data, size);
    
    buffer->usingsize += size;
}

void *ks_buffer_getdata(struct ks_buffer *buffer)
{
    if (buffer->data2)
    {
        return buffer->data2;
    }
    
    return buffer->data;
}

size_t ks_buffer_size(struct ks_buffer *buffer)
{
    return buffer->usingsize;
}

void ks_buffer_reset(struct ks_buffer *buffer)
{
    if (buffer->data2)
    {
        free(buffer->data2);
        buffer->data2 = NULL;
    }
    
    buffer->totalsize = sizeof(buffer->data);
    buffer->usingsize = 0;
}


void INIT_KS_BUFFER_READER(struct ks_buffer_reader *reader, void *data, size_t length)
{
    reader->data = data;
    reader->totalsize = length;
    reader->pos = 0;
}

bool ks_buffer_reader_peek(struct ks_buffer_reader *reader, void *data, size_t length)
{
    size_t unread_bytes = ks_buffer_reader_unread_bytes(reader);

    if(unread_bytes >= length)
    {
        memcpy(data, ks_buffer_reader_getpos(reader), length);
        return 1;
    }

    return 0;
}

bool ks_buffer_reader_read(struct ks_buffer_reader *reader, void *data, size_t length)
{
    size_t unread_bytes = ks_buffer_reader_unread_bytes(reader);

    if(unread_bytes >= length)
    {
        memcpy(data, ks_buffer_reader_getpos(reader), length);
        reader->pos += length;
        return 1;
    }

    return 0;
}

bool ks_buffer_reader_seek(struct ks_buffer_reader *reader, size_t position)
{
    if(reader->totalsize > position)
    {
        reader->pos = position;
        return 1;
    }

    return 0;
}

bool ks_buffer_reader_ignore(struct ks_buffer_reader *reader, size_t offset)
{
    size_t unread_bytes = ks_buffer_reader_unread_bytes(reader);

    if(unread_bytes >= offset)
    {
        reader->pos += offset;
        return 1;
    }

    return 0;
}

void *ks_buffer_reader_getpos(struct ks_buffer_reader *reader)
{
    return reader->data + reader->pos;
}

size_t ks_buffer_reader_unread_bytes(struct ks_buffer_reader *reader)
{
    return reader->totalsize - reader->pos;
}

bool ks_buffer_reader_iseof(struct ks_buffer_reader *reader)
{
    return reader->pos >= reader->totalsize;
}
///////////////////////////////////////////////////////////////////////////////////////////

void INIT_KS_CIRCULAR_BUFFER(struct ks_circular_buffer *circular_buffer)
{
    INIT_LIST_HEAD(&circular_buffer->head);
    
    circular_buffer->inpos = 0;
    circular_buffer->outpos = 0;
    
    circular_buffer->totalsize = 0;
    circular_buffer->usingsize = 0;
    
    ks_circular_buffer_addblock(circular_buffer);
    
    circular_buffer->tail = circular_buffer->head.next;
    circular_buffer->first = circular_buffer->head.next;
}

void ks_circular_buffer_destroy(struct ks_circular_buffer *circular_buffer)
{
    struct ks_circular_buffer_block *buffer_block;
    
    circular_buffer->tail = NULL;
    
    while (list_empty(&circular_buffer->head))
    {
        buffer_block = list_first_entry(&circular_buffer->head, struct ks_circular_buffer_block, entry);
        list_del(&buffer_block->entry);
        free(buffer_block);
    }
}

void ks_circular_buffer_queue(struct ks_circular_buffer *circular_buffer, const void *data, size_t size)
{
    struct ks_circular_buffer_block *buffer;
    size_t totalsize;
    size_t nread;
    size_t ncopy;
    const unsigned char *from;
    
    nread = 0;
    from = data;
    
    buffer = container_of(circular_buffer->tail, struct ks_circular_buffer_block, entry);
    
    while (nread != size)
    {
        totalsize = sizeof(buffer->buf) - circular_buffer->inpos;
        ncopy = MIN(totalsize, (size - nread));
        
        memcpy(&buffer->buf[circular_buffer->inpos], from, ncopy);
        
        from += ncopy;
        nread += ncopy;
        
        circular_buffer->usingsize += ncopy;
        circular_buffer->inpos += ncopy;
        if (circular_buffer->inpos == KS_CIRCULAR_BUFFER_BLOCK_SIZE)
        {
            circular_buffer->inpos = 0;
            
            if (list_is_last(&buffer->entry, &circular_buffer->head))
            {
                ks_circular_buffer_addblock(circular_buffer);
            }
            
            buffer = container_of(buffer->entry.next, struct ks_circular_buffer_block, entry);
        }
    }
    
    circular_buffer->tail = &buffer->entry;
}

void ks_circular_buffer_queue_ks_buffer(struct ks_circular_buffer *circular_buffer, struct ks_buffer *buffer)
{
    ks_circular_buffer_queue(circular_buffer, ks_buffer_getdata(buffer), ks_buffer_size(buffer));
}

bool ks_circular_buffer_peek_array(struct ks_circular_buffer *circular_buffer, void *data, size_t size)
{
    struct ks_circular_buffer_block *buffer;
    size_t nread;
    size_t cursor;
    size_t ncopy;
    unsigned char *to;
    
    if (circular_buffer->usingsize < size)
        return 0;
    
    nread = 0;
    cursor = circular_buffer->outpos;
    buffer = container_of(circular_buffer->first, struct ks_circular_buffer_block, entry);
    to = data;
    
    while (nread != size)
    {
        ncopy = MIN((KS_CIRCULAR_BUFFER_BLOCK_SIZE - cursor), (size - nread));
        memcpy(to, &buffer->buf[cursor], ncopy);
        nread += ncopy;
        to += ncopy;
        
        if (nread < size)
        {
            cursor = 0;
            buffer = container_of(buffer->entry.next, struct ks_circular_buffer_block, entry);
        }
    }
    
    return 1;
}

bool ks_circular_buffer_dequeue_array(struct ks_circular_buffer *circular_buffer, void *data, size_t size)
{
    struct ks_circular_buffer_block *buffer;
    size_t nread;
    size_t ncopy;
    unsigned char *to;
    
    if (circular_buffer->usingsize < size)
        return 0;
    
    nread = 0;
    buffer = container_of(circular_buffer->first, struct ks_circular_buffer_block, entry);
    to = data;
    
    while (nread != size)
    {
        ncopy = MIN((KS_CIRCULAR_BUFFER_BLOCK_SIZE - circular_buffer->outpos), (size - nread));
        
        memcpy(to, &buffer->buf[circular_buffer->outpos], ncopy);
        nread += ncopy;
        to += ncopy;
        
        circular_buffer->outpos += ncopy;
        circular_buffer->usingsize -= ncopy;
        
        if (nread < size)
        {
            circular_buffer->outpos = 0;
            buffer = container_of(buffer->entry.next, struct ks_circular_buffer_block, entry);
            list_move_tail(buffer->entry.prev, &circular_buffer->head);
        }
    }
    
    circular_buffer->first = &buffer->entry;
    
    return 1;
}

bool ks_circular_buffer_empty(struct ks_circular_buffer *circular_buffer)
{
    if (circular_buffer->usingsize == 0)
        return 1;
    
    if (circular_buffer->tail == circular_buffer->first)
    {
        if (circular_buffer->inpos == circular_buffer->outpos)
            return 1;
    }
    
    return 0;
}

void ks_circular_buffer_reset(struct ks_circular_buffer *circular_buffer)
{
    circular_buffer->inpos = 0;
    circular_buffer->outpos = 0;
    circular_buffer->usingsize = 0;
    circular_buffer->tail = circular_buffer->head.next;
    circular_buffer->first = circular_buffer->head.next;
}

void ks_circular_buffer_addblock(struct ks_circular_buffer *circular_buffer)
{
    struct ks_circular_buffer_block *buffer_block = calloc(1, sizeof(struct ks_circular_buffer_block));
    circular_buffer->totalsize += sizeof(buffer_block->buf);
    list_add_tail(&buffer_block->entry, &circular_buffer->head);
}

void INIT_KS_LOCKED_QUEUE(struct ks_locked_queue *locked_queue)
{
    INIT_LIST_HEAD(&locked_queue->head);
    uv_mutex_init(&locked_queue->mutex);
    locked_queue->size = 0;
}

void ks_locked_queue_push_front(struct ks_locked_queue *locked_queue, struct list_head *entry)
{
    uv_mutex_lock(&locked_queue->mutex);

    list_add_head(entry, &locked_queue->head);
    locked_queue->size++;

    uv_mutex_unlock(&locked_queue->mutex);
}

void ks_locked_queue_push_back(struct ks_locked_queue *locked_queue, struct list_head *entry)
{
    uv_mutex_lock(&locked_queue->mutex);

    list_add_tail(entry, &locked_queue->head);
    locked_queue->size++;

    uv_mutex_unlock(&locked_queue->mutex);
}

bool ks_locked_queue_empty(struct ks_locked_queue *locked_queue)
{
    bool is_empty;

    uv_mutex_lock(&locked_queue->mutex);

    is_empty = list_empty(&locked_queue->head);

    uv_mutex_unlock(&locked_queue->mutex);

    return is_empty;
}

size_t ks_locked_queue_size(struct ks_locked_queue *locked_queue)
{
    size_t size;

    uv_mutex_lock(&locked_queue->mutex);

    size = locked_queue->size;

    uv_mutex_unlock(&locked_queue->mutex);
    return size;
}

struct list_head *ks_locked_queue_pop_front(struct ks_locked_queue *locked_queue)
{
    struct list_head *entry = NULL;

    uv_mutex_lock(&locked_queue->mutex);

    if(!list_empty(&locked_queue->head))
    {
        entry = locked_queue->head.next;
        list_del(entry);
        locked_queue->size--;
    }

    uv_mutex_unlock(&locked_queue->mutex);

    return entry;
}

struct list_head *ks_locked_queue_pop_back(struct ks_locked_queue *locked_queue)
{
    struct list_head *entry = NULL;

    uv_mutex_lock(&locked_queue->mutex);

    if(!list_empty(&locked_queue->head))
    {
        entry = locked_queue->head.prev;
        list_del(entry);
        locked_queue->size--;
    }

    uv_mutex_unlock(&locked_queue->mutex);

    return entry;
}

void ks_locked_queue_destroy(struct ks_locked_queue *locked_queue)
{
    assert(list_empty(&locked_queue->head));

    uv_mutex_destroy(&locked_queue->mutex);
}

///////////////////////////////////////////////////////////////////////////////////////////

static void _ks_queue_thread_entry(void *arg)
{
    struct ks_queue_thread *thread = (struct ks_queue_thread *)arg;
    struct list_head *entry;
    struct ks_queue_thread_order *threadorder;

    while(1)
    {
        uv_sem_wait(&thread->semaphore);

        entry = ks_locked_queue_pop_front(&thread->input_locked_queue);

        if(entry)
        {
            threadorder = container_of(entry, struct ks_queue_thread_order, entry);
            if(threadorder->flag == KS_QUEUE_THREAD_FLAG_EXIT)
            {
                return;
            }

            thread->processorder(threadorder);

            ks_locked_queue_push_back(&thread->output_locked_queue, &threadorder->entry);

            uv_async_send(&thread->async_notify);
        }
    }
}

static void _ks_queue_thread_complete(uv_async_t *asyncnotify)
{
    struct ks_queue_thread *thread = (struct ks_queue_thread *)asyncnotify->data;
    struct list_head *entry;
    struct ks_queue_thread_order *threadorder;

    while(!ks_locked_queue_empty(&thread->output_locked_queue))
    {
        entry = ks_locked_queue_pop_front(&thread->output_locked_queue);
        if(entry)
        {
            threadorder = container_of(entry, struct ks_queue_thread_order, entry);

            thread->completeorder(threadorder);
            thread->freeentry(threadorder);
        }
    }
}

void INIT_KS_QUEUE_THREAD(  struct ks_queue_thread *thread,
                            uv_loop_t *loop,
                            size_t input_queue_maxcount, 
                            ks_queue_thread_processorder processorder,
                            ks_queue_thread_completeorder completeorder,
                            ks_queue_thread_free_entry freeentry
)
{
    INIT_KS_LOCKED_QUEUE(&thread->input_locked_queue);
    INIT_KS_LOCKED_QUEUE(&thread->output_locked_queue);
    thread->started = 0;
    uv_sem_init(&thread->semaphore, 0);
    thread->processorder = processorder;
    thread->completeorder = completeorder;
    thread->freeentry = freeentry;
    thread->input_queue_maxcount = input_queue_maxcount;
    thread->loop = loop;
    bzero(&thread->exitorder, sizeof(thread->exitorder));
    thread->exitorder.flag = KS_QUEUE_THREAD_FLAG_EXIT;
}

void ks_queue_thread_start(struct ks_queue_thread *thread)
{
    if(thread->started == 0)
    {
        thread->started = 1;
        uv_async_init(thread->loop, &thread->async_notify, _ks_queue_thread_complete);
        thread->async_notify.data = thread;
        uv_thread_create(&thread->thread, _ks_queue_thread_entry, thread);
    }
}

void _ks_queue_thread_post(struct ks_queue_thread *thread, struct ks_queue_thread_order *entry)
{
    ks_locked_queue_push_back(&thread->input_locked_queue, &entry->entry);
    uv_sem_post(&thread->semaphore);
}

void ks_queue_thread_stop(struct ks_queue_thread *thread)
{
    if(thread->started)
    {
        thread->started = 0;
        _ks_queue_thread_post(thread, &thread->exitorder);
        uv_thread_join(&thread->thread);
        uv_close((uv_handle_t *)&thread->async_notify, NULL);
    }
}

bool ks_queue_thread_post(struct ks_queue_thread *thread, struct ks_queue_thread_order *entry)
{
    entry->flag = KS_QUEUE_THREAD_FLAG_POST;

    if(ks_locked_queue_size(&thread->input_locked_queue) >= thread->input_queue_maxcount)
    {
        return 0;
    }

    _ks_queue_thread_post(thread, entry);

    return 1;
}

size_t ks_socket_thread_input_size(struct ks_queue_thread *thread)
{
    return ks_locked_queue_size(&thread->input_locked_queue);
}

size_t ks_socket_thread_output_size(struct ks_queue_thread *thread)
{
    return ks_locked_queue_size(&thread->output_locked_queue);
}

void ks_queue_thread_destroy(struct ks_queue_thread *thread)
{
    ks_queue_thread_stop(thread);
    ks_locked_queue_destroy(&thread->input_locked_queue);
    ks_locked_queue_destroy(&thread->output_locked_queue);
    uv_sem_destroy(&thread->semaphore);
}

///////////////////////////////////////////////////////////////////////////////////////////
static void ks_table_free_node(struct ks_table_slot **slot)
{
    struct ks_table_slot *curr;
    struct ks_table_slot *tmp;
    
    curr = *slot;
    
    while (curr)
    {
        tmp = curr->nextslot;
        free(curr);
        curr = tmp;
    }
    
    *slot = NULL;
}

void ks_table_clear(struct ks_table *table)
{
    uint32_t i;
    
    for (i = 0; i < table->max_slots; i++)
    {
        ks_table_free_node(&table->slots[i]);
        table->slots[i] = NULL;
    }
    
    table->member_count = 0;
}

void INIT_KS_TABLE(struct ks_table *table, int max_slots)
{
    table->member_count = 0;
    table->max_slots = max_slots;
    table->slots = calloc(max_slots, sizeof(struct ks_table_slot *));
}

void ks_table_destroy(struct ks_table *table)
{
    ks_table_clear(table);
    free(table->slots);
    table->slots = NULL;
}

bool ks_table_insert(struct ks_table *table, uint64_t id, void *data)
{
    uint32_t index;
    struct ks_table_slot *current, *new;
    
    index = id % table->max_slots;
    
    new = calloc(1, sizeof(struct ks_table_slot));
    new->slotid = id;
    new->data = data;
    new->nextslot = NULL;
    
    if (table->slots[index] == NULL)
    {
        table->slots[index] = new;
        table->member_count++;
        
        return 1;
    }
    else
    {
        current = table->slots[index];
        
        while (current)
        {
            if (current->slotid == id)
            {
                free(new);
                
                return 0;
            }
            current = current->nextslot;
        }
        
        new->nextslot = table->slots[index];
        table->slots[index] = new;
        table->member_count++;
    }
    
    return 1;
}

bool ks_table_remove(struct ks_table *table, uint64_t id)
{
    uint32_t index;
    struct ks_table_slot *current, *prev;
    
    index = id % table->max_slots;
    
    current = table->slots[index];
    prev = NULL;
    
    if (current)
    {
        do
        {
            if (current->slotid == id)
            {
                if (prev)
                {
                    table->member_count--;
                    prev->nextslot = current->nextslot;
                }
                else
                {
                    table->member_count--;
                    table->slots[index] = current->nextslot;
                }
                
                free(current);
                return 1;
            }
            
            prev = current;
            current = current->nextslot;
        } while (current);
    }
    
    return 0;
}

void *ks_table_find(struct ks_table *table, uint64_t id)
{
    struct ks_table_slot *current;
    uint32_t index;
    
    index = id % table->max_slots;
    current = table->slots[index];
    
    while (current)
    {
        if (current->slotid == id)
        {
            return current->data;
        }
        
        current = current->nextslot;
    }
    
    return NULL;
}

void ks_table_enum(struct ks_table *table, ks_table_callback cb, void *user_arg)
{
    uint32_t i;
    struct ks_table_slot *current, *temp;
    
    current = NULL;
    temp = NULL;
    
    for (i = 0; i < table->max_slots; i++)
    {
        current = table->slots[i];
        
        while (current)
        {
            temp = current->nextslot;
            cb(table, current->slotid, current->data, user_arg);
            current = temp;
        }
    }
}

void INIT_KS_SOCKET_CONTAINER(struct ks_socket_container *container, uv_loop_t *loop, struct ks_socket_callback *callback, int max_connections, int init_socket_count, int max_slots, int init_buffers_count, int init_writereq_count)
{
    int i;
    struct ks_socket_context *context;
    struct ks_buffer *buffer;
    struct ks_writereq *wreq;
    
    container->callback = callback;
    container->loop = loop;
    container->uniqid_iterators = 1;
    container->init_buffers_count = init_buffers_count;
    container->init_writereq_count = init_writereq_count;
    container->max_connections = max_connections;
    container->init_socket_count = init_socket_count;
    container->current_socket_count = 0;
    container->num_connections = 0;
    
    INIT_LIST_HEAD(&container->active_connections);
    INIT_LIST_HEAD(&container->inactive_connections);
    INIT_LIST_HEAD(&container->buffers);
    INIT_LIST_HEAD(&container->using_buffers);
    INIT_LIST_HEAD(&container->writereq_buffers);
    
    INIT_KS_TABLE(&container->connections, max_slots);
    
    for (i = 0; i < init_socket_count; i++)
    {
        context = callback->socket_context_new(container);
        list_add_tail(&context->entry, &container->inactive_connections);
    }
    
    for (i = 0; i < init_buffers_count; i++)
    {
        buffer = ks_buffer_create();
        list_add_head(&buffer->entry, &container->buffers);
    }
    
    for (i = 0; i < init_writereq_count; i++)
    {
        wreq = calloc(1, sizeof(struct ks_writereq));
        list_add_head(&wreq->entry, &container->writereq_buffers);
    }
}

bool ks_socket_getsockname(const struct ks_socket_context *context, struct ks_netadr *netadr)
{
    int namelen = sizeof(struct sockaddr);
    size_t bufferlen = sizeof(netadr->addr_unix.sun_path);
    
    bzero(netadr, sizeof(struct ks_netadr));
    
    if (context->handle.handle.type == UV_TCP)
    {
        if (uv_tcp_getsockname(&context->handle.tcp, &netadr->addr, &namelen) == 0)
            return 1;
    }
    
    if (context->handle.handle.type == UV_NAMED_PIPE)
    {
        if (uv_pipe_getsockname(&context->handle.pipe, netadr->addr_unix.sun_path, &bufferlen) == 0)
            return 1;
    }
    
    return 0;
}

bool ks_socket_getpeername(const struct ks_socket_context *context, struct ks_netadr *netadr)
{
    int namelen = sizeof(struct sockaddr);
    size_t bufferlen = sizeof(netadr->addr_unix.sun_path);
    
    bzero(netadr, sizeof(struct ks_netadr));
    
    if (context->handle.handle.type == UV_TCP)
    {
        if (uv_tcp_getpeername(&context->handle.tcp, &netadr->addr, &namelen) == 0)
            return 1;
    }
    else if (context->handle.handle.type == UV_NAMED_PIPE)
    {
        if (uv_pipe_getpeername(&context->handle.pipe, netadr->addr_unix.sun_path, &bufferlen) == 0)
            return 1;
    }
    
    return 0;
}

struct ks_socket_context *ks_socket_refernece(struct ks_socket_container *container, struct ks_socket_context *context)
{
    if (context == NULL)
    {
        if (list_empty(&container->inactive_connections))
        {
            context = container->callback->socket_context_new(container);
        }
        else
        {
            context = list_first_entry(&container->inactive_connections, struct ks_socket_context, entry);
            list_del(&context->entry);
        }
        
        context->client = 0;
        context->active = 1;
        context->refcount = 1;
        context->status = KS_SOCKET_STATUS_UNINITIALIZE;
        context->container = container;
        context->uniqid = __sync_add_and_fetch(&container->uniqid_iterators, 1);
        context->handle.handle.data = context;
        context->after_close_disconnected = 0;
        INIT_LIST_HEAD(&context->sendorder);
        
        list_add_tail(&context->entry, &container->active_connections);
        ks_table_insert(&container->connections, context->uniqid, context);
        
        if (container->callback->socket_init)
        {
            container->callback->socket_init(container, context);
        }
    }
    else
    {
        __sync_add_and_fetch(&context->refcount, 1);
    }
    
    return context;
}

void ks_socket_derefernece(struct ks_socket_container *container, struct ks_socket_context *context)
{
    int64_t refcount;
    
    refcount = __sync_sub_and_fetch(&context->refcount, 1);
    
    if (refcount <= 0)
    {
        assert(list_empty(&context->sendorder));
        assert(context->status == KS_SOCKET_STATUS_CLOSED || context->status == KS_SOCKET_STATUS_UNINITIALIZE);
        
        if (container->callback->socket_uninit)
        {
            container->callback->socket_uninit(container, context);
        }
        context->active = 0;
        bzero(&context->handle, sizeof(context->handle));
        context->status = KS_SOCKET_STATUS_UNINITIALIZE;
        ks_table_remove(&container->connections, context->uniqid);
        list_del(&context->entry);
        list_add_tail(&context->entry, &container->inactive_connections);
    }
}

static void ks_socket_after_close(uv_handle_t *handle)
{
    struct ks_socket_context *context = handle->data;
    
    if (context->after_close_disconnected)
    {
        if (context->container->callback->disconnected)
        {
            context->container->callback->disconnected(context->container, context);
        }
        
        if (!context->exclude)
        {
            context->container->num_connections--;
        }
    }
    
    context->status = KS_SOCKET_STATUS_CLOSED;
    ks_socket_derefernece(context->container, context);
}

static void ks_socket_alloc_cb(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    struct ks_socket_context *context = handle->data;
    buf->base = context->rdbuf;
    buf->len = sizeof(context->rdbuf);
}

static void ks_socket_read_cb(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
    struct ks_socket_context *context = stream->data;
    
    if (nread == UV_EOF)
    {
        if (context->container->callback->disconnected)
        {
            context->container->callback->disconnected(context->container, context);
        }
        
        if (!context->exclude)
        {
            context->container->num_connections--;
        }
        context->status = KS_SOCKET_STATUS_CLOSING;
        uv_close(&context->handle.handle, ks_socket_after_close);
        return;
    }
    
    if (nread < 0)
    {
        if (context->container->callback->handle_error)
        {
            context->container->callback->handle_error(context->container, context, (int)nread);
        }
        if (context->container->callback->disconnected)
        {
            context->container->callback->disconnected(context->container, context);
        }
        
        if (!context->exclude)
        {
            context->container->num_connections--;
        }
        
        context->status = KS_SOCKET_STATUS_CLOSING;
        uv_close(&context->handle.handle, ks_socket_after_close);
        return;
    }
    
    if (context->container->callback->arrived)
    {
        context->container->callback->arrived(context->container, context, buf->base, nread);
    }
}

static void ks_connected_cb(uv_connect_t *req, int status)
{
    struct ks_socket_context *context = req->data;
    int err;
    
    if (status == 0)
    {
        context->status = KS_SOCKET_STATUS_ESTABLISHED;
        
        err = uv_read_start(&context->handle.stream, ks_socket_alloc_cb, ks_socket_read_cb);
        if (err)
        {
            if (context->container->callback->handle_error)
            {
                context->container->callback->handle_error(context->container, context, err);
            }
            
            context->status = KS_SOCKET_STATUS_CLOSING;
            uv_close(&context->handle.handle, ks_socket_after_close);
            return;
        }
        
        if (context->container->callback->connected)
        {
            context->container->callback->connected(context->container, context);
        }
    }
    else
    {
        if (context->container->callback->connect_failed)
        {
            context->container->callback->connect_failed(context->container, context, status);
        }
        
        context->status = KS_SOCKET_STATUS_CLOSING;
        uv_close(&context->handle.handle, ks_socket_after_close);
    }
}

static void ks_socket_connection_cb(uv_stream_t *server, int status)
{
    int err;
    
    struct ks_socket_container *container;
    struct ks_socket_context *context = server->data;
    struct ks_socket_context *newcontext;
    
    if (status)
    {
        return;
    }
    
    err = 0;
    
    container = context->container;
    
    newcontext = ks_socket_refernece(container, NULL);
    if (newcontext == NULL)
    {
        fprintf(stderr, "out of memory\n");
        return;
    }
    
    if (server->type == UV_NAMED_PIPE)
    {
        err = uv_pipe_init(container->loop, &newcontext->handle.pipe, 0);
    }
    
    if (server->type == UV_TCP)
    {
        err = uv_tcp_init(container->loop, &newcontext->handle.tcp);
    }
    
    if (err)
    {
        if (container->callback->handle_error)
        {
            container->callback->handle_error(container, newcontext, err);
        }
        
        ks_socket_derefernece(container, newcontext);
        return;
    }
    
    newcontext->status = KS_SOCKET_STATUS_INITIALIZED;
    
    err = uv_accept(server, &newcontext->handle.stream);
    
    if (err)
    {
        if (container->callback->handle_error)
        {
            container->callback->handle_error(container, newcontext, err);
        }
        
        newcontext->status = KS_SOCKET_STATUS_CLOSING;
        uv_close(&newcontext->handle.handle, ks_socket_after_close);
        return;
    }
    
    newcontext->status = KS_SOCKET_STATUS_ESTABLISHED;
    
    if (container->num_connections + 1 > container->max_connections)
    {
        if (container->callback->handle_serverfull)
        {
            container->callback->handle_serverfull(container, newcontext);
        }
        
        ks_socket_shutdown(newcontext);
        return;
    }
    
    container->num_connections++;
    
    err = uv_read_start(&newcontext->handle.stream, ks_socket_alloc_cb, ks_socket_read_cb);
    if (err)
    {
        if (container->callback->handle_error)
        {
            container->callback->handle_error(container, newcontext, err);
        }
        
        newcontext->status = KS_SOCKET_STATUS_CLOSING;
        uv_close(&newcontext->handle.handle, ks_socket_after_close);
        return;
    }
    
    if (container->callback->connected)
    {
        container->callback->connected(container, newcontext);
    }
}

int ks_socket_addlistener_ipv4(struct ks_socket_container *container, const char *addr, int port)
{
    int err;
    struct sockaddr_in adr;
    struct ks_socket_context *context;
    
    err = uv_ip4_addr(addr, port, &adr);
    
    if (err)
    {
        return err;
    }
    
    context = ks_socket_refernece(container, NULL);
    if (context == NULL)
    {
        fprintf(stderr, "out of memory\n");
        abort();
    }
    
    err = uv_tcp_init(container->loop, &context->handle.tcp);
    
    if (err)
    {
        if (container->callback->handle_error)
        {
            container->callback->handle_error(container, context, err);
        }
        ks_socket_derefernece(container, context);
        return err;
    }
    
    context->exclude = 1;
    context->status = KS_SOCKET_STATUS_INITIALIZED;
    
    err = uv_tcp_bind(&context->handle.tcp, (const struct sockaddr *)&adr, 0);
    
    if (err)
    {
        if (container->callback->handle_error)
        {
            container->callback->handle_error(container, context, err);
        }
        context->status = KS_SOCKET_STATUS_CLOSING;
        uv_close(&context->handle.handle, ks_socket_after_close);
        return err;
    }
    
    err = uv_listen(&context->handle.stream, SOMAXCONN, ks_socket_connection_cb);
    
    if (err)
    {
        if (container->callback->handle_error)
        {
            container->callback->handle_error(container, context, err);
        }
        
        context->status = KS_SOCKET_STATUS_CLOSING;
        uv_close(&context->handle.handle, ks_socket_after_close);
        return err;
    }
    
    context->status = KS_SOCKET_STATUS_LISTEN;
    
    return 0;
}

int ks_socket_addlistener_ipv6(struct ks_socket_container *container, const char *addr, int port)
{
    int err;
    struct sockaddr_in6 adr;
    struct ks_socket_context *context;
    
    err = uv_ip6_addr(addr, port, &adr);
    
    if (err)
    {
        return err;
    }
    
    context = ks_socket_refernece(container, NULL);
    if (context == NULL)
    {
        fprintf(stderr, "out of memory\n");
        abort();
    }
    
    err = uv_tcp_init(container->loop, &context->handle.tcp);
    
    if (err)
    {
        if (container->callback->handle_error)
        {
            container->callback->handle_error(container, context, err);
        }
        ks_socket_derefernece(container, context);
        return err;
    }
    
    context->exclude = 1;
    context->status = KS_SOCKET_STATUS_INITIALIZED;
    
    err = uv_tcp_bind(&context->handle.tcp, (const struct sockaddr *)&adr, 0);
    
    if (err)
    {
        if (container->callback->handle_error)
        {
            container->callback->handle_error(container, context, err);
        }
        context->status = KS_SOCKET_STATUS_CLOSING;
        uv_close(&context->handle.handle, ks_socket_after_close);
        return err;
    }
    
    err = uv_listen(&context->handle.stream, SOMAXCONN, ks_socket_connection_cb);
    
    if (err)
    {
        if (container->callback->handle_error)
        {
            container->callback->handle_error(container, context, err);
        }
        context->status = KS_SOCKET_STATUS_CLOSING;
        uv_close(&context->handle.handle, ks_socket_after_close);
        return err;
    }
    
    context->status = KS_SOCKET_STATUS_LISTEN;
    
    return 0;
}

int ks_socket_addlistener_pipe(struct ks_socket_container *container, const char *path)
{
    int err;
    struct ks_socket_context *context;
    
    context = ks_socket_refernece(container, NULL);
    if (context == NULL)
    {
        fprintf(stderr, "out of memory\n");
        abort();
    }
    
    err = uv_pipe_init(container->loop, &context->handle.pipe, 0);
    
    if (err)
    {
        if (container->callback->handle_error)
        {
            container->callback->handle_error(container, context, err);
        }
        ks_socket_derefernece(container, context);
        return err;
    }
    
    context->exclude = 1;
    context->status = KS_SOCKET_STATUS_INITIALIZED;

    unlink(path);
    
    err = uv_pipe_bind(&context->handle.pipe, path);
    
    if (err)
    {
        if (container->callback->handle_error)
        {
            container->callback->handle_error(container, context, err);
        }
        context->status = KS_SOCKET_STATUS_CLOSING;
        
        uv_close(&context->handle.handle, ks_socket_after_close);
        return err;
    }
    
    err = uv_listen(&context->handle.stream, SOMAXCONN, ks_socket_connection_cb);
    
    if (err)
    {
        if (container->callback->handle_error)
        {
            container->callback->handle_error(container, context, err);
        }
        context->status = KS_SOCKET_STATUS_CLOSING;
        uv_close(&context->handle.handle, ks_socket_after_close);
        return err;
    }
    
    context->status = KS_SOCKET_STATUS_LISTEN;
    
    return 0;
}

int ks_socket_connect_ipv4(struct ks_socket_container *container, const char *addr, int port, struct ks_socket_context **pcontext)
{
    int err;
    struct sockaddr_in adr;
    struct ks_socket_context *context;
    
    if(pcontext)
    {
        *pcontext = NULL;
    }

    err = uv_ip4_addr(addr, port, &adr);
    
    if (err)
    {
        return err;
    }
    
    context = ks_socket_refernece(container, NULL);
    if (context == NULL)
    {
        fprintf(stderr, "out of memory\n");
        abort();
    }
    
    err = uv_tcp_init(container->loop, &context->handle.tcp);
    
    if (err)
    {
        if (container->callback->handle_error)
        {
            container->callback->handle_error(container, context, err);
        }
        ks_socket_derefernece(container, context);
        return err;
    }
    
    context->client = 1;
    context->exclude = 1;
    context->status = KS_SOCKET_STATUS_INITIALIZED;
    
    context->connect_req.data = context;
    
    err = uv_tcp_connect(&context->connect_req, &context->handle.tcp, (const struct sockaddr *)&adr, ks_connected_cb);
    
    if (err)
    {
        if (container->callback->handle_error)
        {
            container->callback->handle_error(container, context, err);
        }
        
        context->status = KS_SOCKET_STATUS_CLOSING;
        
        uv_close(&context->handle.handle, ks_socket_after_close);
        return err;
    }
    
    context->status = KS_SOCKET_STATUS_CONNECTING;

    if(pcontext)
    {
        *pcontext = context;
    }
    
    return 0;
}

int ks_socket_connect_ipv6(struct ks_socket_container *container, const char *addr, int port, struct ks_socket_context **pcontext)
{
    int err;
    struct sockaddr_in6 adr;
    struct ks_socket_context *context;

    if(pcontext)
    {
        *pcontext = NULL;
    }

    
    err = uv_ip6_addr(addr, port, &adr);
    
    if (err)
    {
        return err;
    }
    
    context = ks_socket_refernece(container, NULL);
    if (context == NULL)
    {
        fprintf(stderr, "out of memory\n");
        abort();
    }
    
    err = uv_tcp_init(container->loop, &context->handle.tcp);
    
    if (err)
    {
        if (container->callback->handle_error)
        {
            container->callback->handle_error(container, context, err);
        }
        ks_socket_derefernece(container, context);
        return err;
    }
    
    context->client = 1;
    context->exclude = 1;
    context->status = KS_SOCKET_STATUS_INITIALIZED;
    
    context->connect_req.data = context;
    
    err = uv_tcp_connect(&context->connect_req, &context->handle.tcp, (const struct sockaddr *)&adr, ks_connected_cb);
    
    if (err)
    {
        if (container->callback->handle_error)
        {
            container->callback->handle_error(container, context, err);
        }
        
        context->status = KS_SOCKET_STATUS_CLOSING;
        
        uv_close(&context->handle.handle, ks_socket_after_close);
        return err;
    }
    
    context->status = KS_SOCKET_STATUS_CONNECTING;
    if(pcontext)
    {
        *pcontext = context;
    }
    
    return 0;
}
int ks_socket_connect_pipe(struct ks_socket_container *container, const char *name, struct ks_socket_context **pcontext)
{
    int err;
    struct ks_socket_context *context;

    if(pcontext)
    {
        *pcontext = NULL;
    }
    
    context = ks_socket_refernece(container, NULL);
    if (context == NULL)
    {
        fprintf(stderr, "out of memory\n");
        abort();
    }
    
    err = uv_pipe_init(container->loop, &context->handle.pipe, 0);
    
    if (err)
    {
        if (container->callback->handle_error)
        {
            container->callback->handle_error(container, context, err);
        }
        ks_socket_derefernece(container, context);
        return err;
    }
    
    context->client = 1;
    context->exclude = 1;
    context->status = KS_SOCKET_STATUS_INITIALIZED;
    
    context->connect_req.data = context;
    
    uv_pipe_connect(&context->connect_req, &context->handle.pipe, name, ks_connected_cb);
    context->status = KS_SOCKET_STATUS_CONNECTING;
    if(pcontext)
    {
        *pcontext = context;
    }
    return 0;
}

struct ks_buffer *ks_socket_buffer_refernece(struct ks_socket_container *container, struct ks_buffer *buffer)
{
    if (buffer)
    {
        ks_buffer_addref(buffer);
        return buffer;
    }
    
    if (list_empty(&container->buffers))
    {
        buffer = ks_buffer_create();
        list_add_tail(&buffer->entry, &container->using_buffers);
        
        ks_buffer_addref(buffer);
        
        return buffer;
    }
    
    buffer = list_last_entry(&container->buffers, struct ks_buffer, entry);
    list_del(&buffer->entry);
    list_add_tail(&buffer->entry, &container->using_buffers);
    ks_buffer_addref(buffer);
    
    return buffer;
}

void ks_socket_buffer_derefernece(struct ks_socket_container *container, struct ks_buffer *buffer)
{
    if (ks_buffer_decref(buffer))
    {
        list_del(&buffer->entry);
        ks_buffer_reset(buffer);
        list_add_tail(&buffer->entry, &container->buffers);
    }
}

static void ks_write_cb(uv_write_t *req, int status)
{
    struct ks_socket_container *container;
    struct ks_writereq *writereq = req->data;
    
    container = writereq->context->container;
    
    if (writereq->context->container->callback->send_notify)
    {
        writereq->context->container->callback->send_notify(container, writereq->context, writereq->buffer, status);
    }
    
    list_del(&writereq->entry);
    ks_socket_buffer_derefernece(container, writereq->buffer);
    ks_socket_derefernece(container, writereq->context);
    
    writereq->buffer = NULL;
    writereq->bufptr.base = NULL;
    writereq->bufptr.len = 0;
    writereq->context = NULL;
    
    list_add_tail(&writereq->entry, &container->writereq_buffers);
}

int ks_socket_send(struct ks_socket_context *context, struct ks_buffer *buffer)
{
    int err;
    struct ks_writereq *wreq;
    
    if (context->status == KS_SOCKET_STATUS_ESTABLISHED)
    {
        if (list_empty(&context->container->writereq_buffers))
        {
            wreq = calloc(1, sizeof(struct ks_writereq));
        }
        else
        {
            wreq = list_last_entry(&context->container->writereq_buffers, struct ks_writereq, entry);
            list_del(&wreq->entry);
        }
        
        ks_socket_refernece(context->container, context);
        ks_socket_buffer_refernece(context->container, buffer);
        list_add_head(&wreq->entry, &context->sendorder);
        
        wreq->context = context;
        wreq->buffer = buffer;
        wreq->bufptr.base = ks_buffer_getdata(buffer);
        wreq->bufptr.len = ks_buffer_size(buffer);
        wreq->wrequest.data = wreq;
        
        err = uv_write(&wreq->wrequest, &context->handle.stream, &wreq->bufptr, 1, ks_write_cb);
        
        if (err)
        {
            list_del(&wreq->entry);
            ks_socket_buffer_derefernece(context->container, buffer);
            ks_socket_derefernece(context->container, context);
            
            wreq->buffer = NULL;
            wreq->bufptr.base = NULL;
            wreq->bufptr.len = 0;
            wreq->context = NULL;
            
            list_add_tail(&wreq->entry, &context->container->writereq_buffers);
            
            uv_close(&context->handle.handle, ks_socket_after_close);
            return err;
        }
        
        return 0;
    }
    
    return 1;
}

static void ks_shutdown_cb(uv_shutdown_t *req, int status)
{
    struct ks_socket_context *context = req->data;
    
    context->status = KS_SOCKET_STATUS_SHUTDOWN;
    
    if (context->container->callback->disconnected)
    {
        context->container->callback->disconnected(context->container, context);
    }
    
    context->status = KS_SOCKET_STATUS_CLOSING;
    uv_close(&context->handle.handle, ks_socket_after_close);
}

int ks_socket_shutdown(struct ks_socket_context *context)
{
    int err;
    if (context->status == KS_SOCKET_STATUS_ESTABLISHED)
    {
        context->status = KS_SOCKET_STATUS_SHUTTINGDOWN;
        context->shutdown_req.data = context;
        
        err = uv_shutdown(&context->shutdown_req, &context->handle.stream, ks_shutdown_cb);
        
        if (err)
        {
            context->after_close_disconnected = 1;
            context->status = KS_SOCKET_STATUS_CLOSING;
            uv_close(&context->handle.handle, ks_socket_after_close);
            return err;
        }
        
        return 0;
    }
    return 1;
}

int ks_socket_close(struct ks_socket_context *context)
{
    if (context->status == KS_SOCKET_STATUS_ESTABLISHED)
    {
        context->after_close_disconnected = 1;
        context->status = KS_SOCKET_STATUS_CLOSING;
        uv_close(&context->handle.handle, ks_socket_after_close);
        return 0;
    }
    
    if (context->status == KS_SOCKET_STATUS_LISTEN)
    {
        context->status = KS_SOCKET_STATUS_CLOSING;
        uv_close(&context->handle.handle, ks_socket_after_close);
        return 0;
    }
    
    return 1;
}

struct ks_socket_context *ks_socket_find(struct ks_socket_container *container, uint64_t uniqid)
{
    return ks_table_find(&container->connections, uniqid);
}

int ks_socket_stop(struct ks_socket_container *container)
{
    struct list_head *pos;
    struct ks_socket_context *context;
    list_for_each(pos, &container->active_connections)
    {
        context = container_of(pos, struct ks_socket_context, entry);
        ks_socket_close(context);
    }
    
    return 0;
}

void ks_socket_container_destroy(struct ks_socket_container *container)
{
    struct ks_writereq *wreq;
    struct ks_buffer *buffer;
    struct ks_socket_context *context;

    assert(list_empty(&container->using_buffers));
    assert(list_empty(&container->active_connections));

    while(!list_empty(&container->writereq_buffers))
    {
        wreq = list_first_entry(&container->writereq_buffers, struct ks_writereq, entry);
        list_del(&wreq->entry);
        free(wreq);
    }

    while(!list_empty(&container->buffers))
    {
        buffer = list_first_entry(&container->buffers, struct ks_buffer, entry);
        list_del(&buffer->entry);

        ks_buffer_destroy(buffer);
    }

    while(!list_empty(&container->inactive_connections))
    {
        context = list_first_entry(&container->inactive_connections, struct ks_socket_context, entry);
        list_del(&context->entry);
        container->callback->socket_context_free(container, context);
    }

    ks_table_destroy(&container->connections);
}