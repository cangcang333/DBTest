
APP_DIR:=$(shell sh -c 'pwd')
DEV_HOME=$(APP_DIR)/..
#APP_DIR:=$(shell sh -c 'readlink -f $${APPDIR}')
#APP_DIR:=$(shell sh -c '"$(dirname "$APPDIR")"')

INCL=-I$(DEV_HOME)/include
LDFLAGS=-L/usr/local/lib64

AEROSPIKE_LIB=$(DEV_HOME)/lib/libaerospike.a 
HIREDIS_LIB=$(DEV_HOME)/lib/libhiredis.a
ROCKSDB_LIB=$(DEV_HOME)/lib/librocksdb.a
DYNAMIC_LIB=-lcrypto -lssl -ldl -lpthread -lz -lm
ROCKSDB_DYNAMIC_LIB=-ldl -lpthread -lm -lrt



CC = gcc
# Need g++ version >= 4.9.0, we can using command 
# $ scl enable devtoolset-6 bash
# Got new version of gcc/g++ 6.3.1 
CXX = g++
CXXFLAGS = -fno-rtti -g -O2 -Wall -std=c++11

PLATFORM_CXXFLAGS=-std=c++11  -DROCKSDB_PLATFORM_POSIX -DROCKSDB_LIB_IO_POSIX  -DOS_LINUX -fno-builtin-memcmp -DROCKSDB_FALLOCATE_PRESENT -DROCKSDB_MALLOC_USABLE_SIZE -DROCKSDB_PTHREAD_ADAPTIVE_MUTEX -DROCKSDB_BACKTRACE -DROCKSDB_RANGESYNC_PRESENT -DROCKSDB_SCHED_GETCPU_PRESENT -march=native   -DHAVE_SSE42  -DHAVE_PCLMUL  -DHAVE_AVX2 -DHAVE_UINT128_EXTENSION -DROCKSDB_SUPPORT_THREAD_LOCAL

all:
	@echo "$$DEV_HOME"
#	$(CXX) $(CXXFLAGS) rocksdb_rw_speed_test.cpp $(ROCKSDB_LIB) $(DYNAMIC_LIB) $(INCL) -o rocksdb_rw_speed_test
#	$(CXX) $(CXXFLAGS) redis_rw_speed_test.cpp   $(HIREDIS_LIB) $(INCL) -o redis_rw_speed_test
#	$(CXX) $(CXXFLAGS) aerospike_simple_test.cpp $(AEROSPIKE_LIB) $(DYNAMIC_LIB) $(INCL) -o aerospike_simple_test
#	$(CXX) $(CXXFLAGS) aerospike_rw_speed_test.cpp $(AEROSPIKE_LIB) $(LDFLAGS) $(DYNAMIC_LIB) $(INCL) -o aerospike_rw_speed_test


#	$(CXX) $(CXXFLAGS) optimistic_transaction_spzhu.cpp $(ROCKSDB_LIB) $(ROCKSDB_DYNAMIC_LIB) $(INCL) -o optimistic_transaction_spzhu 
	$(CXX) $(CXXFLAGS) transaction_spzhu.cpp $(ROCKSDB_LIB) $(ROCKSDB_DYNAMIC_LIB) $(INCL) -o transaction_spzhu 

clean:
	rm -f *.o *_rw_speed_test aerospike_simple_test
