#include <cassert>
#include <exception>
#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <regex>
#include <sys/time.h>
#include <vector>

#include <stdio.h>

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
#define MAX_KEY_STR_SIZE  1024
#define MAX_NAMESPACE_SIZE 32
#define MAX_SET_SIZE 64

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

std::vector<std::string> split(const std::string &line, const std::string& delimiter)
{
	std::vector<std::string> ret;
	std::regex rgx(delimiter);
	std::sregex_token_iterator p(line.begin(), line.end(), rgx, -1);
	std::sregex_token_iterator end;
	while (p != end)
	{
		ret.emplace_back(*p++);
	}

	return std::move(ret);
}

void read_file_line(const std::string &path, const std::function<void(const std::string &)>& callback)
{
	try
	{
		std::ifstream in;
		in.open(path, std::ios::in);
		std::string line;
		while (in && getline(in, line))
		{
			callback(line);
		}
		in.close();
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}

long get_time_usec()
{
	struct timeval t;
	gettimeofday(&t, NULL);
	return (long)((long)t.tv_sec * 1000000 + t.tv_usec);
}

enum cafe_key_e
{
	trans_date = 0,
	trans_time = 1,
	ref_nbr = 2,
	customer_id = 3
};

void LOG(std::string msg)
{
	std::cout << msg << std::endl;
}

#define t(x) #x

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "Usage: %s cafe_file_path\n", argv[0]);
		exit(-1);
	}

	const char *path = argv[1];
	std::cout << "path: " << path << std::endl;

	as_config config;
	as_config_init(&config);

	if (! as_config_add_hosts(&config, "127.0.0.1", 3000))
	{
		std::cout << "Invalid host(s)\n";
		as_event_close_loops();
		exit(-1);
	}

	// Connect to the aerospike database cluster.
	aerospike as;
	aerospike_init(&as, &config);

	as_error err;

	if (aerospike_connect(&as, &err) != AEROSPIKE_OK)
	{
		std::cout << "aerospike_connect() returned " << err.code << " - " <<  err.message << std::endl;
		as_event_close_loops();
		aerospike_destroy(&as);
		exit(-1);
	}
	std::cout << "aerospike_connect() successful\n";

	strcpy(g_namespace, "test");
	strcpy(g_set, "eg-set");
	strcpy(g_key_str, "eg-key");
	as_key_init_str(&g_key, g_namespace, g_set, g_key_str);

	//
	// Start clean.
	remove_test_record(&as);
	std::cout << "remove_test_record() successful\n";



	as_record rec;
	as_record_inita(&rec, 2);




	size_t n = 0;
	size_t usetime1 = 0;
	size_t usetime2 = 0;
	read_file_line(path, [&](const std::string& line)
	{
		auto ret = split(line, ",");
		std::string key = ret[cafe_key_e::customer_id] + ":" + ret[cafe_key_e::customer_id] + ":" + ret[cafe_key_e::trans_date] + "_" + ret[cafe_key_e::trans_time] + "_" + ret[cafe_key_e::ref_nbr];


//		std::cout << "key is [" << key << "]" << std::endl;

		struct block_t
		{
			uint16_t off;
			uint16_t size;
		};

		std::string buffer;
		size_t count = ret.size();
		buffer.append((char*)&(count), sizeof(count));
		size_t boff = 0;
		for (auto it = ret.begin(); it != ret.end(); it++)
		{
			block_t block;
			block.off = boff;
			block.size = it->size();
			boff += block.size;
			buffer.append((char *)&block, sizeof(block));
		}

		for (auto it = ret.begin(); it != ret.end(); it++)
		{
			buffer.append(*it);
		}

//		std::cout << "buffer is [" << buffer << "]" << std::endl;

		auto t0 = get_time_usec();
		auto t1 = get_time_usec();
		as_record_set_str(&rec, key.c_str(), buffer.c_str());

		// Log its contents.
		LOG("as_record object to write to database:");
		dump_record(&rec);

		// Write the record to the database.
		if (aerospike_key_put(&as, &err, NULL, &g_key, &rec) != AEROSPIKE_OK)
		{
			std::cout << "aerospike_key_put() returned " <<  err.code  << " - " << err.message << std::endl;
			cleanup(&as);
			exit(-1);
		}	
		LOG("write successed");

		auto t2 = get_time_usec();
		auto off1 = t2 - t1;
		usetime1 += off1;

		auto t3 = get_time_usec();
		std::string out;
		if (! read_test_record(&as))
		{
			cleanup(&as);
			exit(-1);
		}
		auto t4 = get_time_usec();
		auto off2 = t4 - t3;
		usetime2 += off2;

		count = *((size_t *)out.c_str());
		auto blocks = (block_t *)(out.c_str() + sizeof(size_t));
		auto blk = blocks[cafe_key_e::customer_id];
		auto pvalue = (const char *)(out.c_str() + sizeof(size_t) + sizeof(block_t) * count + blk.off);
		std::string value(pvalue, blk.size);

		++n;
		if (n % 10000 == 0)
		{
			std::cout << "-----------------------------------------" << std::endl;
			std::cout << n << std::endl;
			std::cout << "usetime: " << (get_time_usec() - t0) / 1000000.f * 1000.f << " ms" << std::endl;
			std::cout << "usetime1(write): " << usetime1 / 10000.f / 1000.f << " ms" << std::endl;
			std::cout << "usetime1(read): " << usetime2 / 10000.f / 1000.f << " ms" << std::endl;
			t0 = get_time_usec();
			usetime1 = 0;
			usetime2 = 0;
		}

	});



}

static void dump_bin(const as_bin *p_bin)
{
	if (! p_bin)
	{
		std::cout << " null as_bin object\n";
		return;
	}
	char *val_as_str = as_val_tostring(as_bin_get_value(p_bin));
	std::cout << as_bin_get_name(p_bin) << " : " <<  val_as_str << std::endl;
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
		std::cout << "aerospike_key_get() returned " <<  err.code << " - " <<  err.message << std::endl;
		return false;
	}

	// If we didn't get an as_record object back, something's wrong.
	if (! p_rec)
	{
		std::cout << "aerospike_key_get() retrieved null as_record object\n";
		return false;
	}

	// Log the result
	std::cout << "record was successfully read from databases\n";
	dump_record(p_rec);

	// Destroy the as_record object.
	as_record_destroy(p_rec);

	return true;
}

