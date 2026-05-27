#pragma once

#include <string>

#include "Luna/Window.h"

namespace Luna
{
    struct ApplicationSpecification
    {
        std::string Name = "Untitled";
        WindowSpecification WindowSpec;
    };

    class Application
    {
    public:
        Application(const ApplicationSpecification& specification = ApplicationSpecification());
        ~Application();

        void Run();

        void Init();
        void Update();
        void Shutdown();
    private:
        ApplicationSpecification m_Specification;
        Window m_Window;
    };
}