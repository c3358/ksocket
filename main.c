#include "kdb.h"
#include <mcheck.h>

struct ks_db g_dbConnTest;


void transaction()
{
	struct ks_db_value params[1];


	if(!ks_db_begintransaction(&g_dbConnTest))
	{
		printf("transaction start failed. %s\n", g_dbConnTest.last_error);
		exit(1);
	}

	ks_db_value_timestamp(&params[0], time(NULL));

	if(!ks_db_execute(&g_dbConnTest, "DELETE FROM userinfo WHERE  bindsteamdate < ?", params , 1))
	{
		printf("stmt execute failed. %s\n", g_dbConnTest.last_error);
		exit(1);
	}

	printf("stmt execute successful.  affected_rows:%llu\n", g_dbConnTest. affected_rows);

	if(!ks_db_execute(&g_dbConnTest, "SELECT * FROM userinfo", NULL , 0))
	{
		printf("stmt execute failed. %s\n", g_dbConnTest.last_error);
		exit(1);
	}

	printf("stmt execute successful.\n");

	if(!ks_db_execute(&g_dbConnTest, "SELECT * FROM userinfo", NULL , 0))
	{
		printf("stmt execute failed. %s\n", g_dbConnTest.last_error);
		exit(1);
	}

	printf("stmt execute successful.\n");

	ks_db_rollback(&g_dbConnTest);
}

const char *get_type_name(unsigned int type)
{
	switch(type)
	{
		case MYSQL_TYPE_NULL:
			return "MYSQL_TYPE_NULL";
		case MYSQL_TYPE_DECIMAL:
			return "MYSQL_TYPE_DECIMAL";
		case MYSQL_TYPE_TINY:
			return "MYSQL_TYPE_TINY";
		case MYSQL_TYPE_SHORT:
			return "MYSQL_TYPE_SHORT";
		case MYSQL_TYPE_LONG:
			return "MYSQL_TYPE_LONG";
		case MYSQL_TYPE_FLOAT:
			return "MYSQL_TYPE_FLOAT";
		case MYSQL_TYPE_DOUBLE:
			return "MYSQL_TYPE_DOUBLE";
		case MYSQL_TYPE_TIMESTAMP:
			return "MYSQL_TYPE_TIMESTAMP";
		case MYSQL_TYPE_LONGLONG:
			return "MYSQL_TYPE_LONGLONG";
		case MYSQL_TYPE_INT24:
			return "MYSQL_TYPE_INT24";
		case MYSQL_TYPE_DATE:
			return "MYSQL_TYPE_DATE";
		case MYSQL_TYPE_TIME:
			return "MYSQL_TYPE_TIME";
		case MYSQL_TYPE_DATETIME:
			return "MYSQL_TYPE_DATETIME";
		case MYSQL_TYPE_YEAR:
			return "MYSQL_TYPE_YEAR";
		case MYSQL_TYPE_NEWDATE:
			return "MYSQL_TYPE_NEWDATE";
		case MYSQL_TYPE_VARCHAR:
			return "MYSQL_TYPE_VARCHAR";
		case MYSQL_TYPE_BIT:
			return "MYSQL_TYPE_BIT";
		case MYSQL_TYPE_NEWDECIMAL:
			return "MYSQL_TYPE_NEWDECIMAL";
		case MYSQL_TYPE_ENUM:
			return "MYSQL_TYPE_ENUM";
		case MYSQL_TYPE_SET:
			return "MYSQL_TYPE_SET";
		case MYSQL_TYPE_TINY_BLOB:
			return "MYSQL_TYPE_TINY_BLOB";
		case MYSQL_TYPE_MEDIUM_BLOB:
			return "MYSQL_TYPE_MEDIUM_BLOB";
		case MYSQL_TYPE_LONG_BLOB:
			return "MYSQL_TYPE_LONG_BLOB";
		case MYSQL_TYPE_BLOB:
			return "MYSQL_TYPE_BLOB";
		case MYSQL_TYPE_VAR_STRING:
			return "MYSQL_TYPE_VAR_STRING";
		case MYSQL_TYPE_STRING:
			return "MYSQL_TYPE_STRING";
		case MYSQL_TYPE_GEOMETRY:
			return "MYSQL_TYPE_GEOMETRY";
	}
	abort();
}

void print_field(const char *name, unsigned int type, unsigned int flags)
{
	printf("field:%s   type:%s   PRI_KEY:%d  NOT_NULL:%d BLOB:%d UNSIGNED:%d\n", name, get_type_name(type), IS_PRI_KEY(flags), IS_NOT_NULL(flags), IS_BLOB(flags), flags & UNSIGNED_FLAG);
}

void dump_field(struct ks_db_field *fields, unsigned int field_count)
{
	unsigned int i;
	printf("print %u fields.\n", field_count);

	if(fields)
	{
		for(i = 0; i < field_count; i++)
		{
			print_field(fields[i].name,fields[i].type, fields[i].flags);
		}
	}
}

const char *string_value(struct ks_db_value *value)
{
	static char value_string[1024];

	strcpy(value_string, "");

	switch(value->type)
	{
		case KS_DB_VALUE_TYPE_NULL:
			return "NULL";
		case KS_DB_VALUE_TYPE_INT32:
			snprintf(value_string, sizeof(value_string), "%d", value->v_int32);
			break;
		case KS_DB_VALUE_TYPE_UINT32:
			snprintf(value_string, sizeof(value_string), "%u", value->v_uint32);
			break;
		case KS_DB_VALUE_TYPE_INT64:
			snprintf(value_string, sizeof(value_string), "%ld", value->v_int64);
			break;
		case KS_DB_VALUE_TYPE_UINT64:
			snprintf(value_string, sizeof(value_string), "%lu", value->v_uint64);
			break;
		case KS_DB_VALUE_TYPE_FLOAT:
			snprintf(value_string, sizeof(value_string), "%f", value->v_float);
			break;
		case KS_DB_VALUE_TYPE_DOUBLE:
			snprintf(value_string, sizeof(value_string), "%lf", value->v_double);
			break;
		case KS_DB_VALUE_TYPE_STRING:
			snprintf(value_string, sizeof(value_string), "%s", value->v_str);
			break;
		case KS_DB_VALUE_TYPE_BLOB:
			return "KS_DB_VALUE_TYPE_BLOB";
			break;
		case KS_DB_VALUE_TYPE_DATETIME:
			snprintf(value_string, sizeof(value_string), "%04d-%02d-%02d %02d:%02d:%02d", 
				value->v_time.year,
				value->v_time.month,
				value->v_time.day, 
				value->v_time.hour, 
				value->v_time.minute, 
				value->v_time.second
				);
			break;
	}


	return value_string;
}

void dump_row(struct ks_db_field *fields, unsigned int field_count, struct ks_db_column *row)
{
	unsigned int i;
	const char *value;

	printf("current row :%p\n", row);

	for(i = 0; i < field_count; i++)
	{
		value = string_value(&row[i].value);

		printf("column name:%s  value:%s\n", fields[i].name, value);
	}
}

void dump_rows(my_ulonglong num_rows, struct ks_db_column **rows, struct ks_db_field *fields, unsigned int field_count)
{
	my_ulonglong i;
	printf("print %llu rows.\n", num_rows);

	for(i = 0; i < num_rows; i++)
	{
		printf("--------------------------------------\n");
		dump_row(fields, field_count, rows[i]);
	}
}

void dump_result(struct ks_db_result *result)
{
	printf("affected_rows:%llu\n", result->affected_rows);
	printf("insert_id:%llu\n", result->insert_id);
	printf("--------------------------------------\n");
	dump_field(result->fields, result->field_count);
	printf("--------------------------------------\n");

	dump_rows(result->num_rows, result->rows, result->fields, result->field_count);
}


void result_cursor(struct ks_db_result *result)
{
	if(ks_db_result_move_first(result))
	{
		printf("ks_db_result_move_first ok.\n");
		do
		{
			uint32_t user_id;
			const char *username;
			char username_buffer[64];

			printf("user_id index:%u\n", ks_db_result_find_field_index(result, "user_id"));
			printf("nickname index:%u\n", ks_db_result_find_field_index(result, "nickname"));

			printf("user_id is null:%d\n", ks_db_result_field_is_null(result, 0));
			printf("nickname is null:%d\n", ks_db_result_field_is_null(result, 1));

			if(!ks_db_result_get_field_uint32(result, 0, &user_id))
			{
				printf("ks_db_result_get_field_uint32 retrieve user_id failed.\n");
			}
			else
			{
				printf("ks_db_result_get_field_uint32 retrieve user_id:%u\n", user_id);
			}

			if(!ks_db_result_get_field_uint32_with_name(result, "nickname", &user_id))
			{
				printf("ks_db_result_get_field_uint32_with_name retrieve nickname failed.\n");
			}
			else
			{
				printf("ks_db_result_get_field_uint32 retrieve nickname:%u\n", user_id);
			}

			username = ks_db_result_get_field_string_with_name(result, "nickname");
			if(username == NULL)
			{
				printf("ks_db_result_get_field_string_with_name retrieve nickname failed.\n");
			}
			else
			{
				printf("retrieve nickname:%s\n", username);
			}

			if(ks_db_result_retrieve_field_string_with_name(result, "nickname", username_buffer, sizeof(username_buffer)))
			{
				printf("ks_db_result_retrieve_field_string_with_name retrieve nickname %s\n", username_buffer);	
			}
			else
			{
				printf("ks_db_result_retrieve_field_string_with_name failed.\n");
			}

			printf("current cursor:%llu    %p\n", result->cursor_index, result->cursor);
		}while(ks_db_result_move_next(result));

		printf("cursor is eof:%d\n", ks_db_result_is_eof(result));
	}
	else
	{
		printf("ks_db_result_move_first failed.\n");
	}
}


void execute_procedure()
{
	struct ks_db_resultset *resultset;
	int index;

	resultset = ks_db_execute_query(&g_dbConnTest, "call p_myquery;", NULL, 0);
	if(resultset)
	{
		printf("ks_db_execute_query successful.\n");
	}
	else
	{
		printf("ks_db_execute_query failed. %s\n", g_dbConnTest.last_error);
		abort();
	}
	
	index = 0;

	if(ks_db_resultset_move_first_result(resultset))
	{
		do
		{
			printf("FIELD BEGIN ================================================================.\n");
			dump_field(resultset->result->fields, resultset->result->field_count);
			printf("FIELD END ================================================================.\n");

			printf("resultset index %d\n", index++);
		}
		while(ks_db_resultset_move_next_result(resultset));
	}

	ks_db_resultset_move_first_result(resultset);

	if(!ks_db_resultset_move_first(resultset))
	{
		printf("move first failed.\n");
		abort();
	}


	do
	{
		uint32_t user_id;
		const char *nickname;
		int32_t frozen;
		uint32_t certified;
		const char * certified_info;
		uint64_t steamid;
		time_t bindsteamdate;
		const char *ip;
		time_t created_at;
		time_t updated_at;


		printf("FIELD BEGIN ================================================================.\n");
		dump_field(resultset->result->fields, resultset->result->field_count);
		printf("FIELD END ================================================================.\n");

		assert(ks_db_resultset_get_field_uint32_with_name(resultset, "user_id", &user_id));
		nickname = ks_db_resultset_get_field_string_with_name(resultset, "nickname");
		assert(nickname != NULL);
		assert(ks_db_resultset_get_field_int32_with_name(resultset, "frozen", &frozen));
		assert(ks_db_resultset_get_field_uint32_with_name(resultset, "certified", &certified));
		certified_info = ks_db_resultset_get_field_string_with_name(resultset, "certified_info");
		assert(certified_info != NULL);
		assert(ks_db_resultset_get_field_uint64_with_name(resultset, "steamid", &steamid));
		assert(ks_db_resultset_get_field_timestamp_with_name(resultset, "bindsteamdate", &bindsteamdate));
		assert(ks_db_resultset_get_field_timestamp_with_name(resultset, "created_at", &created_at));
		assert(ks_db_resultset_get_field_timestamp_with_name(resultset, "updated_at", &updated_at));
		ip = ks_db_resultset_get_field_string_with_name(resultset, "ip");
		assert(ip != NULL);

		printf("DATA BEGIN ==============================================================.\n");


		char str_bindsteamdate[100];
		char str_created_at[100];
		char str_updated_at[100];

		strftime(str_bindsteamdate, sizeof(str_bindsteamdate), "%Y-%m-%d %H:%M:%S", localtime(&bindsteamdate));
		strftime(str_created_at,  sizeof(str_created_at), "%Y-%m-%d %H:%M:%S", localtime(&created_at));
		strftime(str_updated_at,  sizeof(str_updated_at), "%Y-%m-%d %H:%M:%S", localtime(&updated_at));

		printf("user_id:%u\n", user_id);
		printf("nickname:%s\n", nickname);
		printf("frozen:%d\n", frozen);
		printf("certified:%u\n", certified);
		printf("certified_info:%s\n", certified_info);
		printf("steamid:%lu\n", steamid);
		printf("bindsteamdate:%s\n", str_bindsteamdate);
		printf("ip:%s\n", ip);
		printf("created_at:%s\n", str_created_at);
		printf("updated_at:%s\n", str_updated_at);

		printf("DATA END ==============================================================.\n");
	}while(ks_db_resultset_move_next(resultset));

	ks_db_resultset_free(resultset);
}


int main()
{
	setenv("MALLOC_TRACE", "memory_leak.txt", 1);  
	mtrace();
	INIT_KS_DB(&g_dbConnTest, "localhost", "root", "", "ksocket_test_db", 3306, NULL, "utf8");

	if(!ks_db_connect(&g_dbConnTest))
	{
		printf("connect to database failed.  %s\n", g_dbConnTest.last_error);
		return 1;
	}

	printf("connect to database successful.\n");

	transaction();
	execute_procedure();


	ks_db_destroy(&g_dbConnTest);

	return 0;
}