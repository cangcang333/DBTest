all:
#	g++ rocksdb_rw_speed_test.cpp ../librocksdb.a -I./include -ldl -lpthread -o rocksdb_rw_speed_test
#	g++ redis_rw_speed_test.cpp /home/zhushaopeng/Github/hiredis/libhiredis.a -I/home/zhushaopeng/Github/hiredis -o redis_rw_speed_test
	gcc aerospike_simple_test.c /home/spzhu/HardWordTest/aerospike-client-c/target/Linux-x86_64/lib/libaerospike.a -lcrypto -ldl -lssl -lpthread -lz -lm -I/home/spzhu/HardWordTest/aerospike-client-c/target/Linux-x86_64/include/ -o aerospike_simple_test

clean:
	rm -f *.o *_rw_speed_test
