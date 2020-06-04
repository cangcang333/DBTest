/*
 * glog using test 
 * g++ glog_test1.cpp -lglog
 */



#include <iostream>
#include <string>

#include "glog/logging.h"
#include "glog/raw_logging.h"

int main(int argc, char *argv[])
{
	FLAGS_log_dir = ".";
//	FLAGS_logtostderr = 1;
	google::InitGoogleLogging(argv[0]);

	std::string test = "This is shanghai";
	int i = 2, number = 8;

	LOG(INFO) << "info log";
	LOG_IF(INFO, number > 10) << "number > 10";
	LOG_IF(INFO, number < 10) << "number < 10";

	for (i = 0; i < 20; i++)
	{
		LOG_EVERY_N(INFO, 5) << "log i = " << i;
	}

	LOG(WARNING) << "warning log";
	LOG(ERROR) << "error log";

	DLOG(INFO) << "debug mode";
	DLOG_IF(INFO, number < 10) << "debug number < 10";

	RAW_LOG(INFO, "pthread log");

	return 0;
}
