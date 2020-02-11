//==============================================================
// Includes
//
#include <stdio.h>
#include <string.h>

#include <aerospike/aerospike.h>
#include <aerospike/aerospike_index.h>
#include <aerospike/aerospike_key.h>
#include <aerospike/aerospike_udf.h>
#include <aerospike/as_bin.h>
#include <aerospike/as_bytes.h>
#include <aerospike/as_dir.h>
#include <aerospike/as_error.h>
#include <aerospike/as_config.h>
#include <aerospike/as_key.h>
#include <aerospike/as_operations.h>
#include <aerospike/as_password.h>
#include <aerospike/as_record.h>
#include <aerospike/as_record_iterator.h>
#include <aerospike/as_sleep.h>
#include <aerospike/as_status.h>
#include <aerospike/as_string.h>
#include <aerospike/as_val.h>

//==============================================================
// Constants
//
#define MAX_HOST_SIZE  1024
#define MAX_KEY_STR_SIZE  1024

#define MAX_NAMESPACE_SIZE 32
#define MAX_SET_SIZE 64

static char g_host[MAX_HOST_SIZE];
static int g_port;

char g_namespace[MAX_NAMESPACE_SIZE];
char g_set[MAX_SET_SIZE];
static char g_key_str[MAX_KEY_STR_SIZE];
uint32_t g_n_keys;

//==============================================================
// Example Logging Macros
//

#define LOG(_fmt, ...) {printf(_fmt "\n", ##__VA_ARGS__); fflush(stdout);}

int main(int argc, char *argv[])
{
	const char DEFAULT_HOST[] = "127.0.0.1";
	const int DEFAULT_PORT = 3000;
	const char DEFAULT_NAMESPACE[] = "test";
	const char DEFAULT_SET[] = "eg-set";	
	const char DEFAULT_KEY_STR[] = "eg-key";	
	const uint32_t DEFAULT_NUM_KEYS = 20;

	strcpy(g_host, DEFAULT_HOST);
	g_port = DEFAULT_PORT;
	strcpy(g_namespace, DEFAULT_NAMESPACE);
	strcpy(g_set, DEFAULT_SET);
	strcpy(g_key_str, DEFAULT_KEY_STR);
	g_n_keys = DEFAULT_NUM_KEYS;
	
	as_config config;
	as_config_init(&config);
	
	if (! as_config_add_hosts(&config, g_host, g_port))
	{
		printf("Invalid host(s) %s\n", g_host);
		as_event_close_loops();  // ??? need or useless ???
		exit(-1);
	}
	
	// Connect to the aerospike database cluster.
	aerospike as;
	aerospike_init(&as, &config);

	as_error err;

	if (aerospike_connect(&as, &err) != AEROSPIKE_OK)
	{
		LOG("aerospike_connect() returned %d - %s", err.code, err.message);
		as_event_close_loops();
		aerospike_destroy(&as);
		exit(-1);
	}
	LOG("aerospike_connect() successful");

	/*
	// Start clean.
	example_remove_test_record(&as);


	as_record rec;
	as_record_inita(&rec, 2);
	as_record_set_int64(&rec, "test-bin-1", 33124);
	as_record_set_str(&rec, "test-bin-2", "test-bin-2-data");

	// Log its contents.
	LOG("as_record object to write to database:");
	example_dump_record(&rec);

	// Write the record to the database.
	if (aerospike_key_put(&as, &err, NULL, &g_key, &rec) != AEROSPIKE_OK)
	{
		LOG("aerospike_key_put() returned %d - %s", err.code, err.message);
		example_cleanup(&as);
		exit(-1);
	}	
	LOG("write successed");

	if (! example_read_test_record(&as))
	{
		example_cleanup(&as);
		exit(-1);
	}
	*/

	return 0;
}
