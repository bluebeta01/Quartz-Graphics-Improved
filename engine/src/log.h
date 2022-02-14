#pragma once
#include <spdlog/spdlog.h>

namespace Quartz
{
class Log
{
public:
	static std::shared_ptr<spdlog::logger> s_log;
	static void initialize();

private:
	Log();
};
}

#define QZINFO(...)SPDLOG_INFO(__VA_ARGS__)
#define QZWARN(...)SPDLOG_WARN(__VA_ARGS__)
#define QZERROR(...)SPDLOG_ERROR(__VA_ARGS__)
#define QZFATAL(...)SPDLOG_CRITICAL(__VA_ARGS__)