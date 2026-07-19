#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace Radiance
{

    static Log* s_Instance = nullptr;

    Log::Log()
    {
        s_Instance = this;

        spdlog::set_pattern("%^[%T] %n: %v%$");
        m_AppLogger = spdlog::stdout_color_mt("RADIANCE");
        m_AppLogger->set_level(spdlog::level::trace);
    }

    Log::~Log()
    {
        s_Instance = nullptr;
    }

    Log& Log::Get()
    {
        return *s_Instance;
    }

    void Log::Init()
    {
        if (!s_Instance)
            s_Instance = new Log();
    }

    void Log::Shutdown()
    {
        delete s_Instance;
    }

}