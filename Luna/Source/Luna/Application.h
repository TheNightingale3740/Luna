#pragma once

#include <string>

namespace Luna
{
    struct ApplicationSpecification
    {
        std::string Name = "Untitled";
    };

    class Application
    {
    public:
        Application();
        ~Application();

        void Run();
    };
}