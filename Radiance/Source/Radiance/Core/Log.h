#pragma once

#include <spdlog/spdlog.h>

namespace Radiance
{

    class Log
    {
    public:
        Log();
        ~Log();

        static void Init();
        static Log& Get();

        std::shared_ptr<spdlog::logger>& GetLogger() { return m_AppLogger; }
    private:
        std::shared_ptr<spdlog::logger> m_AppLogger;
    };

}

#ifndef RD_DIST
    #define RD_LOG_TRACE(...)      ::Radiance::Log::Get().GetLogger()->trace(__VA_ARGS__)
    #define RD_LOG_INFO(...)       ::Radiance::Log::Get().GetLogger()->info(__VA_ARGS__)
    #define RD_LOG_WARN(...)       ::Radiance::Log::Get().GetLogger()->warn(__VA_ARGS__)
    #define RD_LOG_ERROR(...)      ::Radiance::Log::Get().GetLogger()->error(__VA_ARGS__)
    #define RD_LOG_CRITICAL(...)   ::Radiance::Log::Get().GetLogger()->critical(__VA_ARGS__)
#else
    #define RD_LOG_TRACE(...)      
    #define RD_LOG_INFO(...)       
    #define RD_LOG_WARN(...)       
    #define RD_LOG_ERROR(...)      
    #define RD_LOG_CRITICAL(...)   
#endif