#include "pch.h"
#include "Log.h"

#include "spdlog/sinks/stdout_color_sinks.h"

namespace Prism {

	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_LuaLogger;
	std::shared_ptr<spdlog::logger> Log::s_AppLogger;

	void Log::Init()
	{
		spdlog::set_pattern("%^[%T] %n: %v (%s:%#)%$");

		s_CoreLogger = spdlog::stdout_color_mt("Core");
		s_LuaLogger = spdlog::stdout_color_mt("Lua");
		s_AppLogger = spdlog::stdout_color_mt("App");

		s_CoreLogger->set_level(spdlog::level::trace);
		s_LuaLogger->set_level(spdlog::level::trace);
		s_AppLogger->set_level(spdlog::level::trace);

		spdlog::set_default_logger(s_AppLogger);
	}
}