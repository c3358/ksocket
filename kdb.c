#include "kdb.h"

void ks_db_value_null(struct ks_db_value *value)
{
	value->type = KS_DB_VALUE_TYPE_NULL;
	value->v_int32 = 0;
	value->alloc = 0;
	value->length = 0;
	value->is_null = 1;
}

void ks_db_value_int32(struct ks_db_value *value, int32_t v)
{
	value->type = KS_DB_VALUE_TYPE_INT32;
	value->v_int32 = v;
	value->alloc = 0;
	value->is_null = 0;
	value->length = sizeof(v);

}

void ks_db_value_uint32(struct ks_db_value *value, uint32_t v)
{
	value->type = KS_DB_VALUE_TYPE_UINT32;
	value->v_uint32 = v;
	value->alloc = 0;
	value->is_null = 0;
	value->length = sizeof(v);
}

void ks_db_value_int64(struct ks_db_value *value, int64_t v)
{
	value->type = KS_DB_VALUE_TYPE_INT64;
	value->v_int64 = v;
	value->alloc = 0;
	value->is_null = 0;
	value->length = sizeof(v);
}

void ks_db_value_uint64(struct ks_db_value *value, uint64_t v)
{
	value->type = KS_DB_VALUE_TYPE_UINT64;
	value->v_uint64 = v;
	value->alloc = 0;
	value->is_null = 0;
	value->length = sizeof(v);
}

void ks_db_value_float(struct ks_db_value *value, float v)
{
	value->type = KS_DB_VALUE_TYPE_FLOAT;
	value->v_float = v;
	value->alloc = 0;
	value->is_null = 0;
	value->length = sizeof(v);
}

void ks_db_value_double(struct ks_db_value *value, double v)
{
	value->type = KS_DB_VALUE_TYPE_DOUBLE;
	value->v_double = v;
	value->alloc = 0;
	value->is_null = 0;
	value->length = sizeof(v);
}

void ks_db_value_string(struct ks_db_value *value, const char *v)
{
	value->type = KS_DB_VALUE_TYPE_STRING;
	value->v_str = v;
	value->alloc = 0;
	value->is_null = 0;
	value->length = strlen(v);
}

void ks_db_value_blob(struct ks_db_value *value, const void *v, size_t length)
{
	value->type = KS_DB_VALUE_TYPE_BLOB;
	value->v_data = v;
	value->alloc = 0;
	value->is_null = 0;
	value->length = length;
}

void ks_db_value_string2(struct ks_db_value *value, const char *v)
{
	value->type = KS_DB_VALUE_TYPE_STRING;
	value->v_str = strdup(v);
	value->alloc = 1;
	value->is_null = 0;
	value->length = strlen(v);
}

void ks_db_value_blob2(struct ks_db_value *value, const void *v, size_t length)
{
	void *tmp;
	value->type = KS_DB_VALUE_TYPE_STRING;
	tmp = malloc(length);
	memcpy(tmp, v, length);
	value->v_data = tmp;
	value->alloc = 1;
	value->is_null = 0;
	value->length = length;
}

void ks_db_value_timestamp(struct ks_db_value *value, time_t ts)
{
	struct tm *ts2;
	ts2 = localtime(&ts);

	value->type = KS_DB_VALUE_TYPE_DATETIME;

	value->v_time.time_type = MYSQL_TIMESTAMP_DATE;
	value->v_time.neg = 0;
	value->v_time.second_part = 0;

	value->v_time.year = (ts2->tm_year + 1900);
	value->v_time.month = (ts2->tm_mon + 1);
	value->v_time.day = ts2->tm_mday;
	value->v_time.hour = ts2->tm_hour;
	value->v_time.minute = ts2->tm_min;
	value->v_time.second = ts2->tm_sec;

	value->alloc = 0;
	value->is_null = 0;

	value->length = sizeof(value->v_time);
}

void ks_db_value_date(struct ks_db_value *value, unsigned int  year, unsigned int month, unsigned int day, unsigned int hour, unsigned int minute, unsigned int second)
{
	value->type = KS_DB_VALUE_TYPE_DATETIME;

	value->v_time.time_type = MYSQL_TIMESTAMP_DATE;
	value->v_time.neg = 0;
	value->v_time.second_part = 0;

	value->v_time.year = year;
	value->v_time.month = month;
	value->v_time.day = day;
	value->v_time.hour = hour;
	value->v_time.minute = minute;
	value->v_time.second = second;

	value->alloc = 0;
	value->is_null = 0;

	value->length = sizeof(value->v_time);
}

void ks_db_value_destroy(struct ks_db_value *value)
{
	if(value->type == KS_DB_VALUE_TYPE_STRING || value->type == KS_DB_VALUE_TYPE_BLOB)
	{
		if(value->alloc)
		{
			if(value->type == KS_DB_VALUE_TYPE_STRING && value->v_str != NULL)
			{
				free((void *)value->v_str);
				value->v_str = NULL;

			}

			if(value->type == KS_DB_VALUE_TYPE_BLOB && value->v_data != NULL)
			{
				free((void *)value->v_data);
				value->v_data = NULL;
			}
		}
	}
}

static void *ks_db_value_ptr(struct ks_db_value *value)
{
	void *ptr = NULL;
	switch(value->type)
	{
		case KS_DB_VALUE_TYPE_NULL:
		case KS_DB_VALUE_TYPE_INT32:
			ptr = &value->v_int32;
			break;
		case KS_DB_VALUE_TYPE_UINT32:
			ptr = &value->v_uint32;
			break;
		case KS_DB_VALUE_TYPE_INT64:
			ptr = &value->v_int64;
			break;
		case KS_DB_VALUE_TYPE_UINT64:
			ptr = &value->v_uint64;
			break;
		case KS_DB_VALUE_TYPE_FLOAT:
			ptr = &value->v_float;
			break;
		case KS_DB_VALUE_TYPE_DOUBLE:
			ptr = &value->v_double;
			break;
		case KS_DB_VALUE_TYPE_STRING:
			ptr = (void *)value->v_str;
			break;
		case KS_DB_VALUE_TYPE_BLOB:
			ptr = (void *)value->v_data;
			break;
		case KS_DB_VALUE_TYPE_DATETIME:
			ptr = &value->v_time;
			break;
	}

	return ptr;
}

size_t ks_db_value_length(const struct ks_db_value *value)
{
	return value->length;
}

bool ks_db_value_isnull(const struct ks_db_value *value)
{
	if(value->type != KS_DB_VALUE_TYPE_NULL) { return 0; }
	return 1;
}

bool ks_db_value_get_int32(const struct ks_db_value *value, int32_t *v)
{
	if(value->type != KS_DB_VALUE_TYPE_INT32) { return 0; }
	*v = value->v_int32;
	return 1;
}
bool ks_db_value_get_uint32(const struct ks_db_value *value, uint32_t *v)
{
	if(value->type != KS_DB_VALUE_TYPE_UINT32) { return 0; }
	*v = value->v_uint32;
	return 1;
}

bool ks_db_value_get_int64(const struct ks_db_value *value, int64_t *v)
{
	if(value->type != KS_DB_VALUE_TYPE_INT64){
		return 0;
	}
	*v = value->v_int64;
	return 1;
}
bool ks_db_value_get_uint64(const struct ks_db_value *value, uint64_t *v){
	if(value->type != KS_DB_VALUE_TYPE_UINT64) {
		return 0;
	}

	*v = value->v_uint64;
	return 1;
}
bool ks_db_value_get_float(const struct ks_db_value *value, float *v){
	if(value->type != KS_DB_VALUE_TYPE_FLOAT){
		return 0;
	}
	*v = value->v_float;
	return 1;
}
bool ks_db_value_get_double(const struct ks_db_value *value, double *v)
{
	if(value->type != KS_DB_VALUE_TYPE_DOUBLE){
		return 0;
	}
	*v = value->v_double;
	return 1;
}

const char* ks_db_value_get_string(const struct ks_db_value *value)
{
	if(value->type != KS_DB_VALUE_TYPE_STRING)
		return NULL;

	return value->v_str;
}

const void* ks_db_value_get_blob(const struct ks_db_value *value)
{
	if(value->type != KS_DB_VALUE_TYPE_BLOB)
		return NULL;

	return value->v_data;
}

bool ks_db_value_get_timestamp(const struct ks_db_value *value, time_t *v)
{
	struct tm _time;

	if(value->type != KS_DB_VALUE_TYPE_DATETIME)
	{
		return 0;
	}

	bzero(&_time, sizeof(_time));
	_time.tm_year = value->v_time.year - 1900;
	_time.tm_mon = value->v_time.month - 1;
	_time.tm_mday = value->v_time.day;
	_time.tm_hour = value->v_time.hour;
	_time.tm_min = value->v_time.minute;
	_time.tm_sec = value->v_time.second;
	*v = mktime(&_time);

	return 1;
}

bool ks_db_value_get_date(const struct ks_db_value *value, 
	unsigned int  *year, unsigned int *month, unsigned int *day,
	 unsigned int *hour, unsigned int *minute, unsigned int *second)
{
	if(value->type != KS_DB_VALUE_TYPE_DATETIME){
		return 0;
	}

	if(year) {*year = value->v_time.year;}
	if(month) {*month = value->v_time.month;}
	if(day) {*day = value->v_time.day;}
	if(hour) {*hour = value->v_time.hour;}
	if(minute) {*minute = value->v_time.minute;}
	if(second) {*second = value->v_time.second;}
	return 1;
}

static void _serialize_as_bytes(char *buffer, size_t buffer_size, struct ks_db_value *value)
{
	size_t i;
	char *pos = buffer;
	size_t lastsize = buffer_size;
	unsigned char v;

	static char chartable[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

	for(i = 0; i < value->length; i++)
	{
		v = ((unsigned char *)value->v_data)[i];
		if(lastsize < 3) {
			return;
		}

		pos[0] = chartable[ v / 0x10 ];
		pos[1] = chartable[(v % 0x10)];
		pos[2] = 0;

		pos += 3;
		lastsize -= 3;
	}
}

void ks_db_value2string(struct ks_db_value *value, char *buffer, size_t buffer_size)
{
	switch(value->type)
	{
		case KS_DB_VALUE_TYPE_NULL:
			snprintf(buffer, buffer_size, "%s", "NULL");
		case KS_DB_VALUE_TYPE_INT32:
			snprintf(buffer, buffer_size, "%d", value->v_int32);
			break;
		case KS_DB_VALUE_TYPE_UINT32:
			snprintf(buffer, buffer_size, "%u", value->v_uint32);
			break;
		case KS_DB_VALUE_TYPE_INT64:
			snprintf(buffer, buffer_size, "%ld", value->v_int64);
			break;
		case KS_DB_VALUE_TYPE_UINT64:
			snprintf(buffer, buffer_size, "%lu", value->v_uint64);
			break;
		case KS_DB_VALUE_TYPE_FLOAT:
			snprintf(buffer, buffer_size, "%f", value->v_float);
			break;
		case KS_DB_VALUE_TYPE_DOUBLE:
			snprintf(buffer, buffer_size, "%lf", value->v_double);
			break;
		case KS_DB_VALUE_TYPE_STRING:
			snprintf(buffer, buffer_size, "%s", value->v_str);
			break;
		case KS_DB_VALUE_TYPE_BLOB:
			_serialize_as_bytes(buffer, buffer_size, value);
			break;
		case KS_DB_VALUE_TYPE_DATETIME:
			snprintf(buffer, buffer_size, "%04d-%02d-%02d %02d:%02d:%02d", 
				value->v_time.year,
				value->v_time.month,
				value->v_time.day, 
				value->v_time.hour, 
				value->v_time.minute, 
				value->v_time.second
				);
			break;
	}
}

uint32_t ks_db_value2uint32(struct ks_db_value *value)
{
	time_t timestamp;
	if(value->type == KS_DB_VALUE_TYPE_INT32)
	{
		return value->v_int32;
	}

	if(value->type == KS_DB_VALUE_TYPE_UINT32)
	{
		return value->v_uint32;
	}

	if(value->type == KS_DB_VALUE_TYPE_INT64)
	{
		return (uint32_t)value->v_int64;
	}

	if(value->type == KS_DB_VALUE_TYPE_UINT64)
	{
		return (uint32_t)value->v_uint64;
	}

	if(value->type == KS_DB_VALUE_TYPE_DATETIME)
	{
		if(!ks_db_value_get_timestamp(value, &timestamp))
			return 0;

		return timestamp;
	}

	return 0;
}

uint64_t ks_db_value2uint64(struct ks_db_value *value)
{
	time_t timestamp;

	if(value->type == KS_DB_VALUE_TYPE_INT32)
	{
		return value->v_int32;
	}

	if(value->type == KS_DB_VALUE_TYPE_UINT32)
	{
		return value->v_uint32;
	}

	if(value->type == KS_DB_VALUE_TYPE_INT64)
	{
		return value->v_int64;
	}

	if(value->type == KS_DB_VALUE_TYPE_UINT64)
	{
		return value->v_uint64;
	}

	if(value->type == KS_DB_VALUE_TYPE_DATETIME)
	{
		if(!ks_db_value_get_timestamp(value, &timestamp))
			return 0;

		return timestamp;
	}


	return 0;
}


static int ks_db_value_bind_type(struct ks_db_value *value)
{
	int type = MYSQL_TYPE_INT24;
	switch(value->type)
	{
		case KS_DB_VALUE_TYPE_NULL:
		case KS_DB_VALUE_TYPE_INT32:
		case KS_DB_VALUE_TYPE_UINT32:
			type = MYSQL_TYPE_LONG;
			break;
		case KS_DB_VALUE_TYPE_INT64:
		case KS_DB_VALUE_TYPE_UINT64:
			type = MYSQL_TYPE_LONGLONG;
			break;
		case KS_DB_VALUE_TYPE_FLOAT:
			type = MYSQL_TYPE_FLOAT;
			break;
		case KS_DB_VALUE_TYPE_DOUBLE:
			type = MYSQL_TYPE_DOUBLE;
			break;
		case KS_DB_VALUE_TYPE_STRING:
			type = MYSQL_TYPE_STRING;
			break;
		case KS_DB_VALUE_TYPE_BLOB:
			type = MYSQL_TYPE_BLOB;
			break;
		case KS_DB_VALUE_TYPE_DATETIME:
			type = MYSQL_TYPE_DATETIME;
			break;
		default:
			type = MYSQL_TYPE_NULL;
	}

	return type;
} 

static void ks_db_value2bindparams(MYSQL_BIND *bind_params, struct ks_db_value *params, int param_count)
{
	int i;

	for(i = 0; i < param_count; i++)
	{
		bind_params[i].buffer_length = params[i].length;
		bind_params[i].length = &bind_params[i].buffer_length;
		bind_params[i].is_null = &params[i].is_null;

		if(params[i].type == KS_DB_VALUE_TYPE_UINT32 || params[i].type == KS_DB_VALUE_TYPE_UINT64)
		{ bind_params[i].is_unsigned = 1; }
		else
		{ bind_params[i].is_unsigned = 0; }

		bind_params[i].buffer = ks_db_value_ptr(&params[i]);
		bind_params[i].buffer_type = ks_db_value_bind_type(&params[i]);
	}
}

static inline void INIT_KS_DB_FIELD(struct ks_db_field *field, const char *name, enum enum_field_types type, unsigned int flags, unsigned long length)
{
	field->name = strdup(name);
	field->type = type;
	field->flags = flags;
	field->length = length;
}

static inline  void ks_db_field_destroy(struct ks_db_field *field)
{
	if(field->name)
	{
		free(field->name);
		field->name = NULL;
	}
}

struct ks_db_result *ks_db_result_new()
{
	struct ks_db_result *r = calloc(1, sizeof(struct ks_db_result));

	return r;
}

static void _columns_free(struct ks_db_column *columns, my_ulonglong num_rows , unsigned int field_count)
{
	my_ulonglong i;
	my_ulonglong total;

	total = num_rows *field_count;

	for(i =0; i < total; i++)
	{
		ks_db_value_destroy(&columns[i].value);
	}

	free(columns);
}

void ks_db_result_free(struct ks_db_result *r)
{
	unsigned int i;

	if(r->fields)
	{
		for( i = 0; i < r->field_count; i++)
		{
			ks_db_field_destroy(&r->fields[i]);
		}
		
		free(r->fields);
		r->fields = NULL;
	}

	if(r->columns)
	{
		_columns_free(r->columns, r->num_rows, r->field_count);
		r->columns = NULL;
	}

	if(r->rows)
	{
		free(r->rows);
		r->rows = NULL;
	}

	free(r);
}


bool ks_db_result_move_first(struct ks_db_result *result)
{
	if(result->rows == NULL)
		return 0;

	result->cursor_index = 0;
	result->cursor = result->rows[result->cursor_index];

	return 1;
}

bool ks_db_result_move_next(struct ks_db_result *result)
{
	my_ulonglong max_index = result->num_rows - 1;

	if(result->rows[max_index] == result->cursor)
		return 0;

	result->cursor_index++;
	result->cursor = result->rows[result->cursor_index];

	return 1;
}

bool ks_db_result_is_eof(struct ks_db_result *result)
{
	my_ulonglong max_index = result->num_rows - 1;

	if(result->rows[max_index] == result->cursor)
		return 1;

	return 0;
}

unsigned int ks_db_result_find_field_index(struct ks_db_result *result, const char *name)
{
	unsigned int i;

	if(result->fields == NULL)
		return -1;

	for(i = 0; i < result->field_count; i++)
	{
		if(!strcmp(result->fields[i].name, name))
		{
			return i;
		}
	}

	return -1;
}

bool ks_db_result_field_is_null(struct ks_db_result *result, unsigned int index)
{
	if(index >= result->field_count)
		return 1;

	return result->cursor[index].is_null;
}


bool ks_db_result_get_field_int32(struct ks_db_result *result, unsigned int index, int32_t *v)
{
	if(index >= result->field_count)
		return 0;

	return ks_db_value_get_int32(&result->cursor[index].value, v);
}

bool ks_db_result_get_field_uint32(struct ks_db_result *result, unsigned int index, uint32_t *v)
{
	if(index >= result->field_count)
		return 0;

	return ks_db_value_get_uint32(&result->cursor[index].value, v);
}

bool ks_db_result_get_field_int64(struct ks_db_result *result, unsigned int index, int64_t *v)
{
	if(index >= result->field_count)
		return 0;

	return ks_db_value_get_int64(&result->cursor[index].value, v);
}

bool ks_db_result_get_field_uint64(struct ks_db_result *result, unsigned int index, uint64_t *v)
{
	if(index >= result->field_count)
		return 0;

	return ks_db_value_get_uint64(&result->cursor[index].value, v);
}
bool ks_db_result_get_field_float(struct ks_db_result *result, unsigned int index, float *v)
{
	if(index >= result->field_count)
		return 0;

	return ks_db_value_get_float(&result->cursor[index].value, v);
}

bool ks_db_result_get_field_double(struct ks_db_result *result, unsigned int index, double *v)
{
	if(index >= result->field_count)
		return 0;

	return ks_db_value_get_double(&result->cursor[index].value, v);
}

const char* ks_db_result_get_field_string(struct ks_db_result *result, unsigned int index)
{
	if(index >= result->field_count)
		return NULL;

	return ks_db_value_get_string(&result->cursor[index].value);
}

const void* ks_db_result_get_field_blob(struct ks_db_result *result, unsigned int index )
{
	if(index >= result->field_count)
		return NULL;

	return ks_db_value_get_blob(&result->cursor[index].value);
}

bool ks_db_result_get_field_timestamp(struct ks_db_result *result, unsigned int index, time_t *v)
{
	if(index >= result->field_count)
		return 0;

	return ks_db_value_get_timestamp(&result->cursor[index].value, v);
}

bool ks_db_result_get_field_date(struct ks_db_result *result, unsigned int index, 
	unsigned int  *year, unsigned int *month, unsigned int *day,
	unsigned int *hour, unsigned int *minute, unsigned int *second)
{
	if(index >= result->field_count)
		return 0;

	return ks_db_value_get_date(&result->cursor[index].value, year, month, day, hour, minute, second);
}

bool ks_db_result_field_is_null_with_name(struct ks_db_result *result, const char *name)
{
	unsigned int index = ks_db_result_find_field_index(result ,name);
	if(index == -1)
		return 0;

	return ks_db_result_field_is_null(result, index);
}

bool ks_db_result_get_field_int32_with_name(struct ks_db_result *result, const char * name, int32_t *v)
{
	unsigned int index = ks_db_result_find_field_index(result ,name);
	return ks_db_result_get_field_int32(result, index, v);
}

bool ks_db_result_get_field_uint32_with_name(struct ks_db_result *result, const char * name, uint32_t *v)
{
	unsigned int index = ks_db_result_find_field_index(result ,name);
	return ks_db_result_get_field_uint32(result, index, v);
}

bool ks_db_result_get_field_int64_with_name(struct ks_db_result *result, const char * name, int64_t *v)
{
	unsigned int index = ks_db_result_find_field_index(result ,name);
	return ks_db_result_get_field_int64(result, index, v);
}

bool ks_db_result_get_field_uint64_with_name(struct ks_db_result *result, const char * name, uint64_t *v)
{
	unsigned int index = ks_db_result_find_field_index(result ,name);

	return ks_db_result_get_field_uint64(result, index, v);
}

bool ks_db_result_get_field_float_with_name(struct ks_db_result *result, const char * name, float *v)
{
	unsigned int index = ks_db_result_find_field_index(result ,name);
	return ks_db_result_get_field_float(result, index, v);
}

bool ks_db_result_get_field_double_with_name(struct ks_db_result *result, const char * name, double *v)
{
	unsigned int index = ks_db_result_find_field_index(result ,name);
	return ks_db_result_get_field_double(result, index, v);
}

const char* ks_db_result_get_field_string_with_name(struct ks_db_result *result, const char * name )
{
	unsigned int index = ks_db_result_find_field_index(result ,name);

	return ks_db_result_get_field_string(result, index);
}

const void* ks_db_result_get_field_blob_with_name(struct ks_db_result *result, const char * name)
{
	unsigned int index = ks_db_result_find_field_index(result ,name);
	return ks_db_result_get_field_blob(result, index);
}

bool ks_db_result_get_field_timestamp_with_name(struct ks_db_result *result, const char * name, time_t *v)
{
	unsigned int index = ks_db_result_find_field_index(result ,name);
	return ks_db_result_get_field_timestamp(result, index, v);
}

bool ks_db_result_get_field_date_with_name(struct ks_db_result *result, const char * name,
	unsigned int  *year, unsigned int *month, unsigned int *day,
	unsigned int *hour, unsigned int *minute, unsigned int *second)
{
	unsigned int index = ks_db_result_find_field_index(result ,name);
	return ks_db_result_get_field_date(result, index, year, month, day, hour, minute, second);
}


bool ks_db_result_retrieve_field_string(struct ks_db_result *result, unsigned int index, char *value, size_t value_size)
{
	if(index >= result->field_count)
		return false;

	ks_db_value2string(&result->cursor[index].value, value, value_size);

	return true;
}

uint32_t ks_db_result_retrieve_field_uint32(struct ks_db_result *result, unsigned int index)
{
	if(index >= result->field_count)
		return 0;


	return ks_db_value2uint32(&result->cursor[index].value);
}

uint64_t ks_db_result_retrieve_field_uint64(struct ks_db_result *result, unsigned int index)
{
	if(index >= result->field_count)
		return 0;

	return ks_db_value2uint64(&result->cursor[index].value);
}


bool ks_db_result_retrieve_field_string_with_name(struct ks_db_result *result, const char *name, char *value, size_t value_size)
{
	unsigned int index = ks_db_result_find_field_index(result ,name);

	return ks_db_result_retrieve_field_string(result, index, value, value_size);
}

uint32_t ks_db_result_retrieve_field_uint32_with_name(struct ks_db_result *result, const char *name)
{
	unsigned int index = ks_db_result_find_field_index(result ,name);

	return ks_db_result_retrieve_field_uint32(result, index);
}

uint64_t ks_db_result_retrieve_field_uint64_with_name(struct ks_db_result *result, const char *name)
{
	unsigned int index = ks_db_result_find_field_index(result ,name);

	return ks_db_result_retrieve_field_uint64(result, index);
}



struct ks_db_resultset *ks_db_resultset_new()
{
	struct ks_db_resultset *rs = calloc(1, sizeof(struct ks_db_resultset));

	INIT_LIST_HEAD(&rs->results);

	return rs;
}

void ks_db_resultset_free(struct ks_db_resultset *rs)
{
	struct ks_db_result *r;

	while(!list_empty(&rs->results))
	{
		r = list_first_entry(&rs->results, struct ks_db_result, entry);
		list_del(&r->entry);

		ks_db_result_free(r);
	}

	if(rs->results_array)
	{
		free(rs->results_array);
		rs->results_array = NULL;
	}

	rs->result = NULL;

	free(rs);
}

bool ks_db_resultset_move_first_result(struct ks_db_resultset *rs)
{
	if(ks_db_resultset_num_results(rs) == 0)
		return false;

	rs->result = list_first_entry(&rs->results, struct ks_db_result, entry);

	return true;
}

bool ks_db_resultset_move_next_result(struct ks_db_resultset *rs)
{
	if(ks_db_resultset_is_eof_result(rs)){
		return false;
	}

	rs->result = container_of(rs->result->entry.next, struct ks_db_result, entry);

	return true;
}

bool ks_db_resultset_is_eof_result(struct ks_db_resultset *rs)
{
	if(ks_db_resultset_num_results(rs) == 0)
		return true;

	if(rs->result == NULL) {
		return true;
	}

	if(rs->result->entry.next == &rs->results)
	{
		return true;
	}

	return false;
}

struct ks_db_result* ks_db_resultset_get_result(struct ks_db_resultset *rs, int index)
{
	if(index < 0) {
		return NULL;
	}

	if(index >= rs->num_results && index < rs->num_results){
		return rs->results_array[index];
	}

	return NULL;
}

bool ks_db_resultset_move_first(struct ks_db_resultset *rs)
{
	if(rs->result == NULL) {
		if(ks_db_resultset_is_empty(rs)) {
			return false;
		}

		if(!ks_db_resultset_move_first_result(rs)) {
			return false;
		}

	}
	return ks_db_result_move_first(rs->result);
}

bool ks_db_resultset_move_next(struct ks_db_resultset *rs)
{
	if(rs->result == NULL) return false;
	return ks_db_result_move_next(rs->result);
}

bool ks_db_resultset_is_eof(struct ks_db_resultset *rs)
{
	if(rs->result == NULL) return true;
	return ks_db_result_is_eof(rs->result);
}


//found = index else -1
unsigned int ks_db_resultset_find_field_index(struct ks_db_resultset *rs, const char *name)
{
	if(rs->result == NULL) return -1;
	return ks_db_result_find_field_index(rs->result, name);
}

//force type check functions
bool ks_db_resultset_field_is_null(struct ks_db_resultset *rs, unsigned int index)
{
	if(rs->result == NULL) return false;
	return ks_db_result_field_is_null(rs->result, index);
}

bool ks_db_resultset_get_field_int32(struct ks_db_resultset *rs, unsigned int index, int32_t *v)
{
	if(rs->result == NULL) return false;
	return ks_db_result_get_field_int32(rs->result, index, v);
}

bool ks_db_resultset_get_field_uint32(struct ks_db_resultset *rs, unsigned int index, uint32_t *v)
{
	if(rs->result == NULL) return false;
	return ks_db_result_get_field_uint32(rs->result, index, v);
}

bool ks_db_resultset_get_field_int64(struct ks_db_resultset *rs, unsigned int index, int64_t *v)
{
	if(rs->result == NULL) return false;
	return ks_db_result_get_field_int64(rs->result, index, v);
}

bool ks_db_resultset_get_field_uint64(struct ks_db_resultset *rs, unsigned int index, uint64_t *v)
{
	if(rs->result == NULL) return false;
	return ks_db_result_get_field_uint64(rs->result, index, v);
}

bool ks_db_resultset_get_field_float(struct ks_db_resultset *rs, unsigned int index, float *v)
{
	if(rs->result == NULL) return false;
	return ks_db_result_get_field_float(rs->result, index, v);
}

bool ks_db_resultset_get_field_double(struct ks_db_resultset *rs, unsigned int index, double *v)
{
	if(rs->result == NULL) return false;
	return ks_db_result_get_field_double(rs->result, index, v);
}

const char* ks_db_resultset_get_field_string(struct ks_db_resultset *rs, unsigned int index)
{
	if(rs->result == NULL) return NULL;

	return ks_db_result_get_field_string(rs->result, index);
}

const void* ks_db_resultset_get_field_blob(struct ks_db_resultset *rs, unsigned int index )
{
	if(rs->result == NULL) return NULL;

	return ks_db_result_get_field_blob(rs->result, index);
}

bool ks_db_resultset_get_field_timestamp(struct ks_db_resultset *rs, unsigned int index, time_t *v)
{
	if(rs->result == NULL) return false;

	return ks_db_result_get_field_timestamp(rs->result, index, v);
}

bool ks_db_resultset_get_field_date(struct ks_db_resultset *rs, unsigned int index, 
	unsigned int  *year, unsigned int *month, unsigned int *day,
	unsigned int *hour, unsigned int *minute, unsigned int *second)
{
	if(rs->result == NULL) return false;

	return ks_db_result_get_field_date(rs->result, index, year, month,day, hour, minute, second);
}


bool ks_db_resultset_field_is_null_with_name(struct ks_db_resultset *rs, const char *name)
{
	if(rs->result == NULL) return false;

	return ks_db_result_field_is_null_with_name(rs->result, name);
}

bool ks_db_resultset_get_field_int32_with_name(struct ks_db_resultset *rs, const char * name, int32_t *v)
{
	if(rs->result == NULL) return false;

	return ks_db_result_get_field_int32_with_name(rs->result, name, v);
}
bool ks_db_resultset_get_field_uint32_with_name(struct ks_db_resultset *rs, const char * name, uint32_t *v)
{
	if(rs->result == NULL) return false;

	return ks_db_result_get_field_uint32_with_name(rs->result, name, v);
}

bool ks_db_resultset_get_field_int64_with_name(struct ks_db_resultset *rs, const char * name, int64_t *v)
{
	if(rs->result == NULL) return false;

	return ks_db_result_get_field_int64_with_name(rs->result, name, v);
}

bool ks_db_resultset_get_field_uint64_with_name(struct ks_db_resultset *rs, const char * name, uint64_t *v)
{
	if(rs->result == NULL) return false;

	return ks_db_result_get_field_uint64_with_name(rs->result, name, v);
}

bool ks_db_resultset_get_field_float_with_name(struct ks_db_resultset *rs, const char * name, float *v)
{
	if(rs->result == NULL) return false;

	return ks_db_result_get_field_float_with_name(rs->result, name, v);
}

bool ks_db_resultset_get_field_double_with_name(struct ks_db_resultset *rs, const char * name, double *v)
{
	if(rs->result == NULL) return false;

	return ks_db_result_get_field_double_with_name(rs->result, name, v);
}

const char* ks_db_resultset_get_field_string_with_name(struct ks_db_resultset *rs, const char * name )
{
	if(rs->result == NULL) return NULL;

	return ks_db_result_get_field_string_with_name(rs->result, name);
}

const void* ks_db_resultset_get_field_blob_with_name(struct ks_db_resultset *rs, const char * name)
{
	if(rs->result == NULL) return NULL;

	return ks_db_result_get_field_blob_with_name(rs->result, name);
}

bool ks_db_resultset_get_field_timestamp_with_name(struct ks_db_resultset *rs, const char * name, time_t *v)
{
	if(rs->result == NULL) return false;

	return ks_db_result_get_field_timestamp_with_name(rs->result, name, v);
}

bool ks_db_resultset_get_field_date_with_name(struct ks_db_resultset *rs, const char * name, 
	unsigned int  *year, unsigned int *month, unsigned int *day,
	unsigned int *hour, unsigned int *minute, unsigned int *second)
{
	if(rs->result == NULL) return false;

	return ks_db_result_get_field_date_with_name(rs->result, name, year, month, day, hour, minute, second);
}


// //auto convert type functions
bool ks_db_resultset_retrieve_field_string(struct ks_db_resultset *rs, unsigned int index, char *value, size_t value_size)
{
	if(rs->result == NULL) return false;

	return ks_db_result_retrieve_field_string(rs->result, index, value, value_size);
}

uint32_t ks_db_resultset_retrieve_field_uint32(struct ks_db_resultset *rs, unsigned int index)
{
	if(rs->result == NULL) return 0;

	return ks_db_result_retrieve_field_uint32(rs->result, index);
}

uint64_t ks_db_resultset_retrieve_field_uint64(struct ks_db_resultset *rs, unsigned int index)
{
	if(rs->result == NULL) return 0;

	return ks_db_result_retrieve_field_uint64(rs->result, index);
}


bool ks_db_resultset_retrieve_field_string_with_name(struct ks_db_resultset *rs, const char *name, char *value, size_t value_size)
{
	if(rs->result == NULL) return false;

	return ks_db_result_retrieve_field_string_with_name(rs->result, name, value, value_size);
}

uint32_t ks_db_resultset_retrieve_field_uint32_with_name(struct ks_db_resultset *rs, const char *name)
{
	if(rs->result == NULL) return 0;

	return ks_db_result_retrieve_field_uint32_with_name(rs->result, name);
}

uint64_t ks_db_resultset_retrieve_field_uint64_with_name(struct ks_db_resultset *rs, const char *name)
{
	if(rs->result == NULL) return 0;
	return ks_db_result_retrieve_field_uint64_with_name(rs->result, name);
}



void INIT_KS_DB(struct ks_db *database, const char *host, const char *user, const char *passwd, const char *db, unsigned int port, const char *unix_socket, const char *charset)
{
	bzero(database, sizeof(struct ks_db));

	if(host) { database->serverinfo.host = strdup(host); }
	if(user) { database->serverinfo.user = strdup(user); }
	if(passwd) { database->serverinfo.passwd = strdup(passwd); }
	if(db) { database->serverinfo.db = strdup(db); }
	if(unix_socket) { database->serverinfo.unix_socket = strdup(unix_socket); }
	if(charset) { database->serverinfo.charset = strdup(charset); }
	database->serverinfo.port = port;

	database->serverinfo.connect_timeout = 10;
}


void ks_db_destroy(struct ks_db *database)
{
	if(database->connected)
	{
		mysql_close(&database->db);
		database->connected = 0;
	}

	if(database->serverinfo.host) { free(database->serverinfo.host); database->serverinfo.host = NULL; }
	if(database->serverinfo.user) { free(database->serverinfo.user); database->serverinfo.user = NULL; }
	if(database->serverinfo.passwd) { free(database->serverinfo.passwd); database->serverinfo.passwd = NULL; }
	if(database->serverinfo.db) { free(database->serverinfo.db); database->serverinfo.db = NULL; }
	if(database->serverinfo.unix_socket) { free(database->serverinfo.unix_socket); database->serverinfo.unix_socket = NULL; }
	if(database->serverinfo.charset) { free(database->serverinfo.charset); database->serverinfo.charset = NULL; }
}

void ks_db_set_connect_timeout(struct ks_db *database, int timeout)
{
	database->serverinfo.connect_timeout = timeout;
}

bool ks_db_connect(struct ks_db *database)
{
	my_bool yes = 1;
	my_bool no = 0;

	unsigned int clientFlags = CLIENT_BASIC_FLAGS;
	
	if(database->connected)
	{
		return database->connected;
	}

	database->last_errno = 0;

	mysql_init(&database->db);

	mysql_options(&database->db, MYSQL_OPT_CONNECT_TIMEOUT, (const char*)&database->serverinfo.connect_timeout);
	if(database->serverinfo.charset) mysql_options(&database->db, MYSQL_SET_CHARSET_NAME, database->serverinfo.charset);

	mysql_options(&database->db, MYSQL_OPT_RECONNECT, (const char*)&yes);

	if (mysql_real_connect(&database->db, database->serverinfo.host, database->serverinfo.user, 
		database->serverinfo.passwd, database->serverinfo.db, database->serverinfo.port, database->serverinfo.unix_socket, clientFlags))
	{
		mysql_autocommit(&database->db, no);
		database->connected = 1;
		return 1;
	}

	database->last_errno = mysql_errno(&database->db);
	strcpy(database->last_error, mysql_error(&database->db));

	mysql_close(&database->db);

	return 0;
}

bool ks_db_begintransaction(struct ks_db *database)
{
	database->last_errno = mysql_query(&database->db, "START TRANSACTION;");

	if(database->last_errno != 0)
	{
		strcpy(database->last_error, mysql_error(&database->db));
	}

	return database->last_errno == 0;
}

bool ks_db_commit(struct ks_db *database)
{
	database->last_errno = mysql_commit(&database->db);
	if(database->last_errno != 0)
	{
		strcpy(database->last_error, mysql_error(&database->db));
	}

	return database->last_errno == 0;
}

bool ks_db_rollback(struct ks_db *database)
{
	database->last_errno = mysql_rollback(&database->db);

	if(database->last_errno != 0)
	{
		strcpy(database->last_error, mysql_error(&database->db));
	}

	return database->last_errno == 0;
}


bool ks_db_execute(struct ks_db *database, const char *sql, struct ks_db_value *params, int param_count)
{
	MYSQL_STMT *stmt;
	MYSQL_BIND *bind_params;
	unsigned long stmt_param_count;

	stmt = mysql_stmt_init(&database->db);
	if(stmt == NULL)
	{
		database->last_errno = 2008;
		return 0;
	}

	database->last_errno = mysql_stmt_prepare(stmt, sql, strlen(sql));

	if(database->last_errno != 0)
	{
		goto StmtException;
	}

	stmt_param_count = mysql_stmt_param_count(stmt);

	if(stmt_param_count != param_count)
	{
		database->last_errno = 2031;
		goto StmtExceptionNP;
	}

	if(param_count > 0)
	{
		bind_params = alloca(sizeof(MYSQL_BIND) * param_count);
		bzero(bind_params, sizeof(MYSQL_BIND) * param_count);

		ks_db_value2bindparams(bind_params, params, param_count);

		database->last_errno = mysql_stmt_bind_param(stmt, bind_params);
		if(database->last_errno != 0)
		{
			goto StmtException;
		}
	}

	database->last_errno = mysql_stmt_execute(stmt);
	if(database->last_errno != 0)
	{
		goto StmtException;
	}

	database->insert_id = mysql_stmt_insert_id(stmt);
	database->affected_rows = mysql_stmt_affected_rows(stmt);

	mysql_stmt_reset(stmt);
	mysql_stmt_close(stmt);
	return 1;
StmtException:
	database->last_errno = mysql_stmt_errno(stmt);
	strcpy(database->last_error, mysql_stmt_error(stmt));
	strcpy(database->sqlstate, mysql_stmt_sqlstate(stmt));

StmtExceptionNP:
	mysql_stmt_reset(stmt);
	mysql_stmt_close(stmt);

	return 0;
}

bool ks_db_fetch_fields(MYSQL_STMT *stmt, struct ks_db_result *rp)
{
	MYSQL_RES *res;
	unsigned int field_count;
	struct ks_db_field *fields;
	unsigned int i;

	res = mysql_stmt_result_metadata(stmt);
	if(res == NULL)
	{
		return 0;
	}

	field_count = mysql_stmt_field_count(stmt);
	fields = calloc(field_count, sizeof(struct ks_db_field));

	for(i = 0; i < field_count; i++)
	{
		INIT_KS_DB_FIELD(&fields[i], res->fields[i].name, res->fields[i].type, res->fields[i].flags, res->fields[i].length);
	}

	rp->fields = fields;
	rp->field_count = field_count;

	mysql_free_result(res);
	return 1;
}

static int _fieldtype_to_buffertype(int type)
{
	switch(type)
	{
		case MYSQL_TYPE_TINY:
		case MYSQL_TYPE_SHORT:
		case MYSQL_TYPE_LONG:
		case MYSQL_TYPE_INT24:
		case MYSQL_TYPE_DECIMAL:
		case MYSQL_TYPE_NEWDECIMAL:
		case MYSQL_TYPE_BIT:
		case MYSQL_TYPE_YEAR:
			return MYSQL_TYPE_LONG;
		case MYSQL_TYPE_LONGLONG:
			return MYSQL_TYPE_LONGLONG;

		case MYSQL_TYPE_FLOAT:
			return MYSQL_TYPE_FLOAT;
		case MYSQL_TYPE_DOUBLE:
			return MYSQL_TYPE_DOUBLE;
		
		case MYSQL_TYPE_TIMESTAMP:
		case MYSQL_TYPE_DATE:
		case MYSQL_TYPE_TIME:
		case MYSQL_TYPE_DATETIME:
			return MYSQL_TYPE_DATETIME;
		
		case MYSQL_TYPE_SET:
		case MYSQL_TYPE_ENUM:
		case MYSQL_TYPE_GEOMETRY:
		case MYSQL_TYPE_STRING:
		case MYSQL_TYPE_VAR_STRING:
			return MYSQL_TYPE_STRING;

		case MYSQL_TYPE_BLOB:
			return MYSQL_TYPE_BLOB;

		case MYSQL_TYPE_NULL:
			return MYSQL_TYPE_NULL;
	}

	return MYSQL_TYPE_STRING;
}

static void _bind_column(MYSQL_BIND *bind, struct ks_db_field *field, struct ks_db_column *column)
{
	bool is_unsigned = ((field->flags & UNSIGNED_FLAG) != 0);

	bind->buffer = NULL;

	bind->length = &column->length;
	bind->is_null = &column->is_null;
	bind->error = &column->error;

	if(bind->buffer_type == MYSQL_TYPE_LONG)
	{
		*bind->length = sizeof(column->value.v_uint32);
		bind->buffer_length = *bind->length;

		if(is_unsigned){ bind->buffer = &column->value.v_uint32; ks_db_value_uint32(&column->value, 0);}
		else { bind->buffer = &column->value.v_int32; ks_db_value_int32(&column->value, 0);}
		return;
	}

	if(bind->buffer_type == MYSQL_TYPE_LONGLONG)
	{
		*bind->length = sizeof(column->value.v_uint64);
		bind->buffer_length = *bind->length;

		if(is_unsigned){ bind->buffer = &column->value.v_uint64; ks_db_value_uint64(&column->value, 0);}
		else { bind->buffer = &column->value.v_int64; ks_db_value_int64(&column->value, 0);}
		return;
	}

	if(bind->buffer_type == MYSQL_TYPE_FLOAT){
		*bind->length = sizeof(column->value.v_float);
		bind->buffer_length = *bind->length;

		bind->buffer = &column->value.v_float;
		ks_db_value_float(&column->value, 0.0f);
		return;
	}

	if(bind->buffer_type == MYSQL_TYPE_DOUBLE){
		*bind->length = sizeof(column->value.v_double);
		bind->buffer_length = *bind->length;

		bind->buffer = &column->value.v_double;
		ks_db_value_double(&column->value, 0.0);
		return;
	}
	if(bind->buffer_type == MYSQL_TYPE_DATETIME)
	{
		*bind->length = sizeof(column->value.v_time);
		bind->buffer_length = *bind->length;

		bind->buffer = &column->value.v_time;
		ks_db_value_timestamp(&column->value, time(NULL));
		return;
	}
}

static void _column_dynamic_value(MYSQL_BIND *bind, struct ks_db_column *column)
{
	if(bind->buffer_type == MYSQL_TYPE_STRING)
	{
		column->value.type = KS_DB_VALUE_TYPE_STRING;
		column->value.v_str = calloc(*bind->length + 1, sizeof(char));
		column->value.alloc = 1;
		column->value.is_null = 0;
		column->value.length = *bind->length;

		bind->buffer = (char *)column->value.v_str;
		bind->buffer_length = column->value.length;
	}

	if(bind->buffer_type == MYSQL_TYPE_BLOB)
	{
		column->value.type = KS_DB_VALUE_TYPE_BLOB;
		column->value.v_data = malloc(*bind->length);
		column->value.alloc = 1;
		column->value.is_null = 0;
		column->value.length = *bind->length;

		bind->buffer = (char *)column->value.v_data;
		bind->buffer_length = column->value.length;
	}
}




bool ks_db_fetch_rows(struct ks_db *database, MYSQL_STMT *stmt, struct ks_db_result *rp)
{
	MYSQL_BIND *stmt_results;
	//unsigned long *stmt_result_length;
	unsigned int k;
	my_ulonglong index;
	struct ks_db_column *row;


	stmt_results = alloca(rp->field_count * sizeof(MYSQL_BIND));
	bzero(stmt_results, rp->field_count * sizeof(MYSQL_BIND));

	rp->num_rows = mysql_stmt_num_rows(stmt);

	rp->rows = calloc(rp->num_rows, sizeof(struct ks_db_column *));
	rp->columns = calloc(rp->num_rows * rp->field_count, sizeof(struct ks_db_column));

	index = 0;
	while(index < rp->num_rows)
	{
		row = &rp->columns[rp->field_count * index];		//current row
		rp->rows[index] = row;									//set current row pointer

		index++;

		bzero(stmt_results, rp->field_count * sizeof(MYSQL_BIND));


		for(k = 0; k < rp->field_count; k++) 
		{ 
			stmt_results[k].buffer_type = _fieldtype_to_buffertype(rp->fields[k].type);
			_bind_column(&stmt_results[k], &rp->fields[k], &row[k]);
		}


		if(mysql_stmt_bind_result(stmt, stmt_results)) { goto FetchException; }

		database->last_errno = mysql_stmt_fetch(stmt);

		if(database->last_errno == MYSQL_NO_DATA)
		{
			goto FetchOK;
		}

		if(database->last_errno != MYSQL_DATA_TRUNCATED && database->last_errno != 0)
		{
			goto FetchException;
		}

		for(k = 0; k < rp->field_count ; k++)
		{
			_column_dynamic_value(&stmt_results[k], &row[k]);
			if(mysql_stmt_fetch_column(stmt, &stmt_results[k], k, 0)) {
				goto FetchException;
			}
		}
	}

	if(mysql_stmt_fetch(stmt) != MYSQL_NO_DATA)
	{
		goto FetchException;
	}

FetchOK:

	return 1;

FetchException:
	if(rp->rows)
	{
		free(rp->rows);
		rp->rows = NULL;
	}

	if(rp->columns)
	{
		_columns_free(rp->columns, rp->num_rows, rp->field_count);
		rp->columns = NULL;
	}

	return 0;
}

bool ks_db_fetch_resultset(struct ks_db *database, MYSQL_STMT *stmt, struct ks_db_resultset **rsp)
{
	struct ks_db_resultset *resultset = NULL;
	my_ulonglong num_rows;
	unsigned int field_count;
	struct ks_db_result *result = NULL;
	unsigned int i;

	resultset = ks_db_resultset_new();

	do
	{
		database->last_errno = mysql_stmt_store_result(stmt);
		if(database->last_errno != 0)
			goto FetchException;
		
		result = ks_db_result_new();

		field_count = mysql_stmt_field_count(stmt);
		num_rows = mysql_stmt_num_rows(stmt);

		result->insert_id = mysql_stmt_insert_id(stmt);
		result->affected_rows = mysql_stmt_affected_rows(stmt);

		if(field_count > 0)
		{
			if(ks_db_fetch_fields(stmt, result) == 0)
			{
				goto FetchException;
			}
		}

		if(field_count > 0 && num_rows > 0)
		{
			if(ks_db_fetch_rows(database, stmt, result) == 0)
			{
				goto FetchException;
			}
		}
		
		list_add_tail(&result->entry, &resultset->results);
		resultset->num_results++;
		result = NULL;

		database->last_errno = mysql_stmt_next_result(stmt);
		if(database->last_errno == -1)
			break;
		if(database->last_errno > 0)
			goto FetchException;
	}while(1);

	resultset->results_array = calloc(resultset->num_results, sizeof(struct ks_db_result *));

	i = 0;

	list_for_each_entry(result, &resultset->results, entry)
	{
		resultset->results_array[i] = result;
		i++;
	}

	//out pointer
	*rsp = resultset;

	return 1;
FetchException:
	if(result) { ks_db_result_free(result); result = NULL; }
	if(resultset) { ks_db_resultset_free(resultset); resultset = NULL; }
	return 0;
}

struct ks_db_resultset* ks_db_execute_query(struct ks_db *database, const char *sql, struct ks_db_value *params, int param_count)
{
	MYSQL_STMT *stmt;
	MYSQL_BIND *bind_params;
	unsigned long stmt_param_count;
	struct ks_db_resultset *rs;

	rs = NULL;
	stmt = mysql_stmt_init(&database->db);
	if(stmt == NULL)
	{
		database->last_errno = 2008;
		return NULL;
	}

	database->last_errno = mysql_stmt_prepare(stmt, sql, strlen(sql));

	if(database->last_errno != 0)
	{
		goto StmtException;
	}

	stmt_param_count = mysql_stmt_param_count(stmt);

	if(stmt_param_count != param_count)
	{
		database->last_errno = 2031;
		goto StmtExceptionNP;
	}

	if(param_count > 0)
	{
		bind_params = alloca(sizeof(MYSQL_BIND) * param_count);
		bzero(bind_params, sizeof(MYSQL_BIND) * param_count);

		ks_db_value2bindparams(bind_params, params, param_count);

		database->last_errno = mysql_stmt_bind_param(stmt, bind_params);
		if(database->last_errno != 0)
		{
			goto StmtException;
		}
	}

	database->last_errno = mysql_stmt_execute(stmt);
	if(database->last_errno != 0)
	{
		goto StmtException;
	}

	if(!ks_db_fetch_resultset(database, stmt, &rs))
	{
		goto StmtException;
	}

	mysql_stmt_reset(stmt);
	mysql_stmt_close(stmt);
	return rs;

StmtException:
	database->last_errno = mysql_stmt_errno(stmt);
	strcpy(database->last_error, mysql_stmt_error(stmt));
	strcpy(database->sqlstate, mysql_stmt_sqlstate(stmt));

StmtExceptionNP:
	mysql_stmt_reset(stmt);
	mysql_stmt_close(stmt);

	return NULL;
}