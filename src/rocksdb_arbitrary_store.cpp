// Created by 朱少鹏 on 2020/4/6.
//

#include <iostream>
#include "rocksdb/db.h"
#include "rocksdb/options.h"
#include "rocksdb/iterator.h"
#include "rocksdb/status.h"
#include "rocksdb/sst_file_writer.h"

#include <cstring>
#include <memory>

template <typename T>
std::string Pack(const T* data)
{
	std::string d(sizeof(T), L'\0');
	memcpy(&d[0], data, d.size());
	return d;
}

template <typename T>
std::unique_ptr<T> Unpack(const std::string& data)
{
	if (data.size() != sizeof(T))
	{
		return nullptr;
	}
	auto d = std::make_unique<T>();
	memcpy(d.get(), data.data(), data.size());

	return d;
}

struct Bob
{
	int a;
	int b;
	char c[10];
};

std::string kDBPath = "/tmp/rocksdb_arbitrary_store";

int main()
{
    rocksdb::DB* db;
    rocksdb::Options options;

    options.IncreaseParallelism();
    options.create_if_missing = true;
//    options.wal_dir = "/tmp/wal/";


    // Open DB
    rocksdb::Status s = rocksdb::DB::Open(options, kDBPath, &db);
    if (!s.ok())
    {
	    printf("Open DB failed\n");
        return 1;
    }

    std::string key = "key1";
    // Test structure
    Bob b = {};
    b.a = 12;
    b.b = 144;
    b.c[0] = 's';
    b.c[1] = 'm';
    b.c[2] = '\0';
    s = db->Put(rocksdb::WriteOptions(), key, Pack(&b));

    // Read from db with same key
    std::string result;
    s = db->Get(rocksdb::ReadOptions(), key, &result);
    std::unique_ptr<Bob> pBob = Unpack<Bob>(result);
    if (b.a == pBob->a &&
	b.b == pBob->b &&
	b.c[0] == pBob->c[0] &&
	b.c[1] == pBob->c[1] )
    {
	    printf("Structure matches\n");
    }
    else
    {
	    printf("Structure doesn't matches\n");
    }

    // Manually flush, trying to store data in sst file
    db->Flush(rocksdb::FlushOptions());

    delete db;

    return 0;
}


