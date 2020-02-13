#include <cassert>
#include <exception>
#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <regex>
#include <sys/time.h>

#include "rocksdb/db.h"
//#include "rocksdb/options.h"

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

#define t(x) #x

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		exit(-1);
	}

	const char *path = argv[1];
	std::cout << "path: " << path << std::endl;

	rocksdb::DB* db;
	rocksdb::Options options;
  	options.IncreaseParallelism();
	options.OptimizeLevelStyleCompaction();
	options.create_if_missing = true;

	auto s = rocksdb::DB::Open(options, "./db", &db);
	assert(s.ok());

	rocksdb::WriteOptions wop;
	rocksdb::ReadOptions rop;

	size_t n = 0;
	size_t usetime1 = 0;
	size_t usetime2 = 0;
	read_file_line(path, [&](const std::string& line)
	{
//		auto ret = split(line, "\u0001");
		auto ret = split(line, ",");
//		std::cout << "trans_time is " << ret[cafe_key_e::trans_time] << ", customer_id is " << ret[cafe_key_e::customer_id] << std::endl;
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
		db->Put(wop, key, buffer);
		auto t2 = get_time_usec();
		auto off1 = t2 - t1;
		usetime1 += off1;

		auto t3 = get_time_usec();
		std::string out;
		db->Get(rop, key, &out);
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

	delete db;
}

