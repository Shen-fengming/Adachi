#pragma once

#include <memory>
#include <spdlog/spdlog.h>

namespace Adachi{

class Log{
public:
    static void Init();

    static const std::shared_ptr<spdlog::logger>& GetCoreLogger(){return s_CoreLogger;}
    static const std::shared_ptr<spdlog::logger>& GetAppLogger(){return s_AppLogger;}
private:
    inline static std::shared_ptr<spdlog::logger> s_CoreLogger;
    inline static std::shared_ptr<spdlog::logger> s_AppLogger;
};

}

// Core log macros
#define ADACHI_CORE_TRACE(...)    ::Adachi::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define ADACHI_CORE_INFO(...)     ::Adachi::Log::GetCoreLogger()->info(__VA_ARGS__)
#define ADACHI_CORE_WARN(...)     ::Adachi::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define ADACHI_CORE_ERROR(...)    ::Adachi::Log::GetCoreLogger()->error(__VA_ARGS__)
#define ADACHI_CORE_CRITICAL(...) ::Adachi::Log::GetCoreLogger()->critical(__VA_ARGS__)



// Client log macros
#define ADACHI_APP_TRACE(...)         ::Adachi::Log::GetAppLogger()->trace(__VA_ARGS__)
#define ADACHI_APP_INFO(...)          ::Adachi::Log::GetAppLogger()->info(__VA_ARGS__)
#define ADACHI_APP_WARN(...)          ::Adachi::Log::GetAppLogger()->warn(__VA_ARGS__)
#define ADACHI_APP_ERROR(...)         ::Adachi::Log::GetAppLogger()->error(__VA_ARGS__)
#define ADACHI_APP_CRITICAL(...)      ::Adachi::Log::GetAppLogger()->critical(__VA_ARGS__)