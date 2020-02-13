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

// The test key used by all basic examples.
// Created using command line options:
// -n <namespace>
// -s <set name>
// -k <key string>
as_key g_key;


static void dump_bin(const as_bin *p_bin);
void dump_record(const as_record* p_rec);
void remove_test_record(aerospike *p_as);
void cleanup(aerospike *p_as);
bool read_test_record(aerospike *p_as);

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

	as_key_init_str(&g_key, g_namespace, g_set, g_key_str);

	//
	// Start clean.
	remove_test_record(&as);
	LOG("remove_test_record() successful");



	as_record rec;
	as_record_inita(&rec, 2);
	as_record_set_int64(&rec, "test-bin-1", 33124);
	as_record_set_str(&rec, "test-bin-2", "test-bin-2-data");

	// Log its contents.
	LOG("as_record object to write to database:");
	dump_record(&rec);

	// Write the record to the database.
	if (aerospike_key_put(&as, &err, NULL, &g_key, &rec) != AEROSPIKE_OK)
	{
		LOG("aerospike_key_put() returned %d - %s", err.code, err.message);
		cleanup(&as);
		exit(-1);
	}	
	LOG("write successed");

	if (! read_test_record(&as))
	{
		cleanup(&as);
		exit(-1);
	}
	//

	return 0;
}

void dump_record(const as_record* p_rec)
{
	if (! p_rec)
	{
		LOG(" null as_record object");
		return;
	}

	if (p_rec->key.valuep)
	{
		char *key_val_as_str = as_val_tostring(p_rec->key.valuep);
		LOG(" key: %s", key_val_as_str);
		free(key_val_as_str);
	}

	uint16_t num_bins = as_record_numbins(p_rec);
	LOG(" generation %u, ttl %u, %u bin%s", p_rec->gen, p_rec->ttl, num_bins, num_bins == 0 ? "s" : (num_bins == 1 ? "1" : "s:"));

	as_record_iterator it;
	as_record_iterator_init(&it, p_rec);

	while (as_record_iterator_has_next(&it))
	{
		dump_bin(as_record_iterator_next(&it));
	}

	as_record_iterator_destroy(&it);
}

static void dump_bin(const as_bin *p_bin)
{
	if (! p_bin)
	{
		LOG(" null as_bin object");
		return;
	}
	char *val_as_str = as_val_tostring(as_bin_get_value(p_bin));
	LOG(" %s : %s", as_bin_get_name(p_bin), val_as_str);
	free(val_as_str);

}

void remove_test_record(aerospike *p_as)
{
	as_error err;
	aerospike_key_remove(p_as, &err, NULL, &g_key);
}

void cleanup(aerospike *p_as)
{
	remove_test_record(p_as);
	as_error err;

	aerospike_close(p_as, &err);
	aerospike_destroy(p_as);
}

bool read_test_record(aerospike *p_as)
{
	as_error err;
	as_record *p_rec = NULL;

	// Read the test record from the database.
	if (aerospike_key_get(p_as, &err, NULL, &g_key, &p_rec) != AEROSPIKE_OK)
	{
		LOG("aerospike_key_get() returned %d - %s", err.code, err.message);
		return false;
	}

	// If we didn't get an as_record object back, something's wrong.
	if (! p_rec)
	{
		LOG("aerospike_key_get() retrieved null as_record object");
		return false;
	}

	// Log the result
	LOG("record was successfully read from databases:");
	dump_record(p_rec);

	// Destroy the as_record object.
	as_record_destroy(p_rec);

	return true;
}

