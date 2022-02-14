#include "pch.h"
#include "log.h"

namespace Quartz
{

std::shared_ptr<spdlog::logger> Log::s_log;

Log::Log()
{

}

void Log::initialize()
{
	s_log = spdlog::stdout_color_mt("log");
	s_log->set_pattern("[%H:%M:%S][%^%l%$][%s:%#] %v");
	spdlog::set_default_logger(s_log);
}
}