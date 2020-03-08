//
// Created by 朱少鹏 on 2020/3/6.
//

#include <iostream>
#include <sstream>

#include "rocksdb/db.h"
#include "rocksdb/options.h"
#include "rocksdb/utilities/checkpoint.h"

std::string kDBPath="/tmp/rocksdb_checkpoint";
std::string kCKPath="/tmp/rocksdb_checkpoint_path";

int main()
{
    std::istringstream iStr("0000000");
    std::ostringstream oStr;

    oStr << "output string stream";
    std::cout << oStr.str() << std::endl;

    rocksdb::DB *db;
    rocksdb::Checkpoint *checkpoint_ptr;
    rocksdb::Options options;

    options.IncreaseParallelism();
    options.OptimizeLevelStyleCompaction();
    options.create_if_missing = true;

    rocksdb::Status s = rocksdb::DB::Open(options, kDBPath, &db);
    assert(s.ok());

    s = db->Put(rocksdb::WriteOptions(), "name", "Danial");
    assert(s.ok());

    {
        rocksdb::WriteBatch batch;
        batch.Put("age", "23");
        batch.Put("gender", "Male");
        batch.Put("habit", "running");
        batch.Put("love", "女人");
        s = db->Write(rocksdb::WriteOptions(), &batch);
    }

    s = rocksdb::Checkpoint::Create(db, &checkpoint_ptr);
    assert(s.ok());
    std::cout << "Checkpoint create is ok\n";

    s = checkpoint_ptr->CreateCheckpoint(kCKPath);
    assert(s.ok());
    std::cout << "CreateCheckpoint is ok\n";

    {
        rocksdb::WriteBatch batch;
        batch.Put("book", "Amazon");
        batch.Put("computer", "iMac");
        s = db->Write(rocksdb::WriteOptions(), &batch);
    }


    delete db;
    delete checkpoint_ptr;

    return 0;
}

