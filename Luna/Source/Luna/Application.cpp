#include "Application.h"

namespace Luna
{
    static Application *s_Application = nullptr;

    Application::Application(const ApplicationSpecification& specification)
        : m_Specification(specification)
    {
        s_Application = this;
        
        glfwInit();

        m_Window = std::make_shared<Window>(specification.WindowSpec);
        m_Window->Create();
    }

    Application::~Application()
    {
        s_Application = nullptr;
        glfwTerminate();
    }

    void Application::Run()
    {
        Init();
        while (!m_Window->ShouldClose())
        {
            Update();
        }

        Shutdown();
    }

    void Application::Init()
    {
    }

    void Application::Update()
    {
        m_Window->Update();
        m_LayerStack.Update(1.0f);
    }

    void Application::Shutdown()
    {
    }

    Application& Application::Get()
    {
        return *s_Application;
    }
}