
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



CC = gcc
# Need g++ version >= 4.9.0, we can using command 
# $ scl enable devtoolset-6 bash
# Got new version of gcc/g++ 6.3.1 
CXX = g++
CXXFLAGS = -g -O2 -Wall -std=c++11

all:
	@echo "$$DEV_HOME"
#	$(CXX) $(CXXFLAGS) rocksdb_rw_speed_test.cpp $(ROCKSDB_LIB) $(DYNAMIC_LIB) $(INCL) -o rocksdb_rw_speed_test
#	$(CXX) $(CXXFLAGS) redis_rw_speed_test.cpp   $(HIREDIS_LIB) $(INCL) -o redis_rw_speed_test
#	$(CXX) $(CXXFLAGS) aerospike_simple_test.cpp $(AEROSPIKE_LIB) $(DYNAMIC_LIB) $(INCL) -o aerospike_simple_test
	$(CXX) $(CXXFLAGS) aerospike_rw_speed_test.cpp $(AEROSPIKE_LIB) $(LDFLAGS) $(DYNAMIC_LIB) $(INCL) -o aerospike_rw_speed_test

clean:
	rm -f *.o *_rw_speed_test aerospike_simple_test
