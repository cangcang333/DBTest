// Created by 朱少鹏 on 2020/4/6.
//

#include <iostream>
#include "rocksdb/db.h"
#include "rocksdb/options.h"
#include "rocksdb/iterator.h"
#include "rocksdb/status.h"
#include "rocksdb/sst_file_writer.h"


int main()
{
    rocksdb::Options options;

    options.create_if_missing = true;
    options.wal_dir = "/tmp/wal/";
//    options.comparator = nullptr;

    rocksdb::SstFileWriter sst_file_writer(rocksdb::EnvOptions(), options);
    std::string file_path = "/tmp/file1.sst";

    // Open the file for writing
    rocksdb::Status s = sst_file_writer.Open(file_path);
    if (!s.ok())
    {
        printf("Error while opening file %s, Error: %s\n", file_path.c_str(), s.ToString().c_str());
        return 1;
    }

    // Insert rows into the SST file, note that inserted keys must be
    // strictly increasing (based on options.comparator)
    {
        s = sst_file_writer.Put("23", "Danial");

	std::string key = "26"; 
	std::string value = "Baseball"; 
	s = sst_file_writer.Put(key, value); 
	if (!s.ok())
        {
            printf("Error while adding Key: %s, Error: %s\n", key.c_str(), s.ToString().c_str());
            return 1;
        }

    }

    // Close the file
    s = sst_file_writer.Finish();
    if (!s.ok())
    {
        printf("Error while finishing file : %s, Error: %s\n", file_path.c_str(), s.ToString().c_str());
        return 1;
    }

    rocksdb::IngestExternalFileOptions ifo;
    // Ingest the 2 passed sst files into the db
    rocksdb::DB *db;
    rocksdb::DB::Open(options, "/tmp/rocksdb_ingest_sst_files", &db);
    s = db->IngestExternalFile({"/tmp/rocksdb_checkpoint/000007.sst"}, ifo);
    if (!s.ok())
    {
	    printf("Error while adding file %s, Error %s\n", "/tmp/rocksdb_checkpoint/000007.sst", s.ToString().c_str());
	    return 1;
    }


    return 0;
}


