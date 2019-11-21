#pragma once

// Log only in debug mode
#ifdef PR_DEBUG
#define SPDLOG_DEBUG_ON
#define SPDLOG_TRACE_ON
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#else
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_ERROR
#endif

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Prism {
	class Log
	{
	public:
		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; };
		inline static std::shared_ptr<spdlog::logger>& GetLuaLogger() { return s_LuaLogger; };
		inline static std::shared_ptr<spdlog::logger>& GetAppLogger() { return s_AppLogger; };

		/* must be called before Logger is used */
		static void Init();

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_LuaLogger;
		static std::shared_ptr<spdlog::logger> s_AppLogger;
	};
}

#define PR_LOG_HEADING_TEXT(text) "========== {0} ====================", text
#define PR_CORE_TRACE(...)		SPDLOG_LOGGER_TRACE(Prism::Log::GetCoreLogger(),__VA_ARGS__)
#define PR_CORE_INFO(...)		SPDLOG_LOGGER_INFO(Prism::Log::GetCoreLogger(),__VA_ARGS__)
#define PR_CORE_WARN(...)		SPDLOG_LOGGER_WARN(Prism::Log::GetCoreLogger(),__VA_ARGS__)
#define PR_CORE_ERROR(...)		SPDLOG_LOGGER_ERROR(Prism::Log::GetCoreLogger(),__VA_ARGS__)
#define PR_CORE_CRITICAL(...)	SPDLOG_LOGGER_CRITICAL(Prism::Log::GetCoreLogger(),__VA_ARGS__)
#define PR_CORE_HEAD(text)		PR_CORE_INFO(PR_LOG_HEADING_TEXT(text))

#define PR_LUA_TRACE(...)		SPDLOG_LOGGER_TRACE(Prism::Log::GetLuaLogger(),__VA_ARGS__)
#define PR_LUA_INFO(...)		SPDLOG_LOGGER_INFO(Prism::Log::GetLuaLogger(),__VA_ARGS__)
#define PR_LUA_WARN(...)		SPDLOG_LOGGER_WARN(Prism::Log::GetLuaLogger(),__VA_ARGS__)
#define PR_LUA_ERROR(...)		SPDLOG_LOGGER_ERROR(Prism::Log::GetLuaLogger(),__VA_ARGS__)
#define PR_LUA_CRITICAL(...)	SPDLOG_LOGGER_CRITICAL(Prism::Log::GetLuaLogger(),__VA_ARGS__)
#define PR_LUA_HEAD(text)		PR_LUA_INFO(PR_LOG_HEADING_TEXT(text))

#define PR_LOG_TRACE(...)		SPDLOG_LOGGER_TRACE(Prism::Log::GetAppLogger(),__VA_ARGS__)
#define PR_LOG_INFO(...)		SPDLOG_LOGGER_INFO(Prism::Log::GetAppLogger(),__VA_ARGS__)
#define PR_LOG_WARN(...)		SPDLOG_LOGGER_WARN(Prism::Log::GetAppLogger(),__VA_ARGS__)
#define PR_LOG_ERROR(...)		SPDLOG_LOGGER_ERROR(Prism::Log::GetAppLogger(),__VA_ARGS__)
#define PR_LOG_CRITICAL(...)	SPDLOG_LOGGER_CRITICAL(Prism::Log::GetAppLogger(),__VA_ARGS__)
#define PR_LOG_HEAD(text)		PR_LOG_INFO(PR_LOG_HEADING_TEXT(text))

// Asserts
#define PR_CORE_ASSERT(x, ...) {if(!(x)) {PR_CORE_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak();}}
#define PR_ASSERT(x, ...) {if(!(x)) {PR_LOG_ERROR("Assertion failed: {0}", __VA_ARGS__); __debugbreak();}}