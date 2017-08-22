#ifndef _KS_DB_H_
#define _KS_DB_H_

#include "ksocket.h"
#include <mysql.h>
#include <alloca.h>


struct ks_db_serverinfo
{
	char *host;
	char *user;
	char *passwd;
	char *db;
	unsigned int port;
	char *unix_socket;
	char *charset;
	int connect_timeout;
};

struct ks_db
{
	struct ks_db_serverinfo serverinfo;
	bool connected;
	MYSQL db;

	unsigned int last_errno;
	char last_error[MYSQL_ERRMSG_SIZE];
	char sqlstate[SQLSTATE_LENGTH+1];

	my_ulonglong affected_rows;
	my_ulonglong insert_id;
};

enum ks_db_value_type
{
	KS_DB_VALUE_TYPE_NULL,
	KS_DB_VALUE_TYPE_INT32,
	KS_DB_VALUE_TYPE_UINT32,
	KS_DB_VALUE_TYPE_INT64,
	KS_DB_VALUE_TYPE_UINT64,
	KS_DB_VALUE_TYPE_FLOAT,
	KS_DB_VALUE_TYPE_DOUBLE,
	KS_DB_VALUE_TYPE_STRING,
	KS_DB_VALUE_TYPE_BLOB,
	KS_DB_VALUE_TYPE_DATETIME,
};

struct ks_db_value
{
	struct list_head entry;

	int type;
	my_bool is_null;
	union
	{
		int32_t v_int32;
		uint32_t v_uint32;
		int64_t v_int64;
		uint64_t v_uint64;
		float v_float;
		double v_double;
		const char *v_str;
		const void *v_data;
		struct st_mysql_time v_time;
	};

	int alloc;
	size_t length;
};

struct ks_db_field
{
	char *name;
	enum enum_field_types type;
	unsigned int flags;
	unsigned long length;
};

struct ks_db_column
{
	struct ks_db_value value;
	my_bool is_null;
	my_bool error;
	unsigned long length;
};

struct ks_db_result
{
	struct list_head entry;

	my_ulonglong affected_rows;
	my_ulonglong insert_id;
	
	unsigned int field_count;
	struct ks_db_field *fields;

	my_ulonglong num_rows;
	struct ks_db_column **rows;

	//memory allocator optimize
	struct ks_db_column *columns;

	my_ulonglong cursor_index;
	struct ks_db_column *cursor;
};

struct ks_db_resultset
{
	int num_results;
	struct list_head results;
	struct ks_db_result **results_array;
	struct ks_db_result *result;
};

//initialize and destroy ks_db_value
void ks_db_value_null(struct ks_db_value *value);
void ks_db_value_int32(struct ks_db_value *value, int32_t v);
void ks_db_value_uint32(struct ks_db_value *value, uint32_t v);
void ks_db_value_int64(struct ks_db_value *value, int64_t v);
void ks_db_value_uint64(struct ks_db_value *value, uint64_t v);
void ks_db_value_float(struct ks_db_value *value, float v);
void ks_db_value_double(struct ks_db_value *value, double v);
void ks_db_value_string(struct ks_db_value *value, const char *v);
void ks_db_value_blob(struct ks_db_value *value, const void *v, size_t length);
void ks_db_value_string2(struct ks_db_value *value, const char *v);
void ks_db_value_blob2(struct ks_db_value *value, const void *v, size_t length);
void ks_db_value_timestamp(struct ks_db_value *value, time_t ts);
void ks_db_value_date(struct ks_db_value *value, unsigned int  year, unsigned int month, unsigned int day, unsigned int hour, unsigned int minute, unsigned int second);
void ks_db_value_destroy(struct ks_db_value *value);



size_t ks_db_value_length(const struct ks_db_value *value);
bool ks_db_value_isnull(const struct ks_db_value *value);

//Force type checked
bool ks_db_value_get_int32(const struct ks_db_value *value, int32_t *v);
bool ks_db_value_get_uint32(const struct ks_db_value *value, uint32_t *v);
bool ks_db_value_get_int64(const struct ks_db_value *value, int64_t *v);
bool ks_db_value_get_uint64(const struct ks_db_value *value, uint64_t *v);
bool ks_db_value_get_float(const struct ks_db_value *value, float *v);
bool ks_db_value_get_double(const struct ks_db_value *value, double *v);
const char* ks_db_value_get_string(const struct ks_db_value *value);
const void* ks_db_value_get_blob(const struct ks_db_value *value);
bool ks_db_value_get_timestamp(const struct ks_db_value *value, time_t *v);
bool ks_db_value_get_date(const struct ks_db_value *value, 
	unsigned int  *year, unsigned int *month, unsigned int *day,
	 unsigned int *hour, unsigned int *minute, unsigned int *second);

//some convert functions
void ks_db_value2string(struct ks_db_value *value, char *buffer, size_t buffer_size);
uint32_t ks_db_value2uint32(struct ks_db_value *value);
uint64_t ks_db_value2uint64(struct ks_db_value *value);


//=============================================================================================

struct ks_db_result *ks_db_result_new();
void ks_db_result_free(struct ks_db_result *r);

static inline my_ulonglong ks_db_result_affected_rows(struct ks_db_result *result)
{
	return result->affected_rows;
}

static  inline my_ulonglong ks_db_result_insert_id(struct ks_db_result *result)
{
	return result->insert_id;
}

static inline my_ulonglong ks_db_result_num_rows(struct ks_db_result *result)
{
	return result->num_rows;
}

static inline struct ks_db_column** ks_db_result_rows(struct ks_db_result *result)
{
	return result->rows;
}

static inline my_ulonglong ks_db_result_cursor_index(struct ks_db_result *result)
{
	return result->cursor_index;
}

static inline bool ks_db_result_move(struct ks_db_result *result, my_ulonglong index)
{
	if(index < result->num_rows)
	{
		result->cursor_index = index;
		result->cursor = result->rows[result->cursor_index];
		return true;
	}

	return false;
}

bool ks_db_result_move_first(struct ks_db_result *result);
bool ks_db_result_move_next(struct ks_db_result *result);
bool ks_db_result_is_eof(struct ks_db_result *result);

//found = index else -1
unsigned int ks_db_result_find_field_index(struct ks_db_result *result, const char *name);

//force type check functions
bool ks_db_result_field_is_null(struct ks_db_result *result, unsigned int index);
bool ks_db_result_get_field_int32(struct ks_db_result *result, unsigned int index, int32_t *v);
bool ks_db_result_get_field_uint32(struct ks_db_result *result, unsigned int index, uint32_t *v);
bool ks_db_result_get_field_int64(struct ks_db_result *result, unsigned int index, int64_t *v);
bool ks_db_result_get_field_uint64(struct ks_db_result *result, unsigned int index, uint64_t *v);
bool ks_db_result_get_field_float(struct ks_db_result *result, unsigned int index, float *v);
bool ks_db_result_get_field_double(struct ks_db_result *result, unsigned int index, double *v);
const char* ks_db_result_get_field_string(struct ks_db_result *result, unsigned int index);
const void* ks_db_result_get_field_blob(struct ks_db_result *result, unsigned int index );
bool ks_db_result_get_field_timestamp(struct ks_db_result *result, unsigned int index, time_t *v);
bool ks_db_result_get_field_date(struct ks_db_result *result, unsigned int index, 
	unsigned int  *year, unsigned int *month, unsigned int *day,
	unsigned int *hour, unsigned int *minute, unsigned int *second);

bool ks_db_result_field_is_null_with_name(struct ks_db_result *result, const char *name);
bool ks_db_result_get_field_int32_with_name(struct ks_db_result *result, const char * name, int32_t *v);
bool ks_db_result_get_field_uint32_with_name(struct ks_db_result *result, const char * name, uint32_t *v);
bool ks_db_result_get_field_int64_with_name(struct ks_db_result *result, const char * name, int64_t *v);
bool ks_db_result_get_field_uint64_with_name(struct ks_db_result *result, const char * name, uint64_t *v);
bool ks_db_result_get_field_float_with_name(struct ks_db_result *result, const char * name, float *v);
bool ks_db_result_get_field_double_with_name(struct ks_db_result *result, const char * name, double *v);
const char* ks_db_result_get_field_string_with_name(struct ks_db_result *result, const char * name );
const void* ks_db_result_get_field_blob_with_name(struct ks_db_result *result, const char * name);
bool ks_db_result_get_field_timestamp_with_name(struct ks_db_result *result, const char * name, time_t *v);
bool ks_db_result_get_field_date_with_name(struct ks_db_result *result, const char * name, 
	unsigned int  *year, unsigned int *month, unsigned int *day,
	unsigned int *hour, unsigned int *minute, unsigned int *second);


//auto convert type functions
bool ks_db_result_retrieve_field_string(struct ks_db_result *result, unsigned int index, char *value, size_t value_size);
uint32_t ks_db_result_retrieve_field_uint32(struct ks_db_result *result, unsigned int index);
uint64_t ks_db_result_retrieve_field_uint64(struct ks_db_result *result, unsigned int index);

bool ks_db_result_retrieve_field_string_with_name(struct ks_db_result *result, const char *name, char *value, size_t value_size);
uint32_t ks_db_result_retrieve_field_uint32_with_name(struct ks_db_result *result, const char *name);
uint64_t ks_db_result_retrieve_field_uint64_with_name(struct ks_db_result *result, const char *name);


//=============================================================================================
struct ks_db_resultset *ks_db_resultset_new();
void ks_db_resultset_free(struct ks_db_resultset *rs);

static inline int ks_db_resultset_num_results(struct ks_db_resultset *rs){
	return rs->num_results;
}

#define ks_db_resultset_is_empty(rs) list_empty(&rs->results)

bool ks_db_resultset_move_first_result(struct ks_db_resultset *rs);
bool ks_db_resultset_move_next_result(struct ks_db_resultset *rs);
bool ks_db_resultset_is_eof_result(struct ks_db_resultset *rs);
struct ks_db_result* ks_db_resultset_get_result(struct ks_db_resultset *rs, int index);
static inline struct ks_db_result* ks_db_resultset_current_result(struct ks_db_resultset *rs) {
	return rs->result;
}

bool ks_db_resultset_move_first(struct ks_db_resultset *rs);
bool ks_db_resultset_move_next(struct ks_db_resultset *rs);
bool ks_db_resultset_is_eof(struct ks_db_resultset *rs);

//found = index else -1
unsigned int ks_db_resultset_find_field_index(struct ks_db_resultset *rs, const char *name);

//force type check functions
bool ks_db_resultset_field_is_null(struct ks_db_resultset *rs ,unsigned int index);
bool ks_db_resultset_get_field_int32(struct ks_db_resultset *rs, unsigned int index, int32_t *v);
bool ks_db_resultset_get_field_uint32(struct ks_db_resultset *rs, unsigned int index, uint32_t *v);
bool ks_db_resultset_get_field_int64(struct ks_db_resultset *rs, unsigned int index, int64_t *v);
bool ks_db_resultset_get_field_uint64(struct ks_db_resultset *rs, unsigned int index, uint64_t *v);
bool ks_db_resultset_get_field_float(struct ks_db_resultset *rs, unsigned int index, float *v);
bool ks_db_resultset_get_field_double(struct ks_db_resultset *rs, unsigned int index, double *v);
const char* ks_db_resultset_get_field_string(struct ks_db_resultset *rs, unsigned int index);
const void* ks_db_resultset_get_field_blob(struct ks_db_resultset *rs, unsigned int index );
bool ks_db_resultset_get_field_timestamp(struct ks_db_resultset *rs, unsigned int index, time_t *v);
bool ks_db_resultset_get_field_date(struct ks_db_resultset *rs, unsigned int index, 
	unsigned int  *year, unsigned int *month, unsigned int *day,
	unsigned int *hour, unsigned int *minute, unsigned int *second);

bool ks_db_resultset_field_is_null_with_name(struct ks_db_resultset *rs, const char *name);
bool ks_db_resultset_get_field_int32_with_name(struct ks_db_resultset *rs, const char * name, int32_t *v);
bool ks_db_resultset_get_field_uint32_with_name(struct ks_db_resultset *rs, const char * name, uint32_t *v);
bool ks_db_resultset_get_field_int64_with_name(struct ks_db_resultset *rs, const char * name, int64_t *v);
bool ks_db_resultset_get_field_uint64_with_name(struct ks_db_resultset *rs, const char * name, uint64_t *v);
bool ks_db_resultset_get_field_float_with_name(struct ks_db_resultset *rs, const char * name, float *v);
bool ks_db_resultset_get_field_double_with_name(struct ks_db_resultset *rs, const char * name, double *v);
const char* ks_db_resultset_get_field_string_with_name(struct ks_db_resultset *rs, const char * name );
const void* ks_db_resultset_get_field_blob_with_name(struct ks_db_resultset *rs, const char * name);
bool ks_db_resultset_get_field_timestamp_with_name(struct ks_db_resultset *rs, const char * name, time_t *v);
bool ks_db_resultset_get_field_date_with_name(struct ks_db_resultset *rs, const char * name, 
	unsigned int  *year, unsigned int *month, unsigned int *day,
	unsigned int *hour, unsigned int *minute, unsigned int *second);


//auto convert type functions
bool ks_db_resultset_retrieve_field_string(struct ks_db_resultset *rs, unsigned int index, char *value, size_t value_size);
uint32_t ks_db_resultset_retrieve_field_uint32(struct ks_db_resultset *rs, unsigned int index);
uint64_t ks_db_resultset_retrieve_field_uint64(struct ks_db_resultset *rs, unsigned int index);

bool ks_db_resultset_retrieve_field_string_with_name(struct ks_db_resultset *rs, const char *name, char *value, size_t value_size);
uint32_t ks_db_resultset_retrieve_field_uint32_with_name(struct ks_db_resultset *rs, const char *name);
uint64_t ks_db_resultset_retrieve_field_uint64_with_name(struct ks_db_resultset *rs, const char *name);






void INIT_KS_DB(struct ks_db *database, const char *host, const char *user, const char *passwd, const char *db, unsigned int port, const char *unix_socket, const char *charset);
void ks_db_destroy(struct ks_db *database);
void ks_db_set_connect_timeout(struct ks_db *database, int timeout);
bool ks_db_connect(struct ks_db *database);

//transaction
bool ks_db_begintransaction(struct ks_db *database);
bool ks_db_commit(struct ks_db *database);
bool ks_db_rollback(struct ks_db *database);

//execute only
bool ks_db_execute(struct ks_db *database, const char *sql, struct ks_db_value *params, int param_count);

struct ks_db_resultset* ks_db_execute_query(struct ks_db *database, const char *sql, struct ks_db_value *params, int param_count);

#endif