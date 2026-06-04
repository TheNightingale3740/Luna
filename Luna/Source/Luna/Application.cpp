#include "Application.h"

#include <print>

namespace Luna
{
    static Application *s_Application = nullptr;

    Application::Application(const ApplicationSpecification& specification)
        : m_Specification(specification)
    {
        s_Application = this;
        
        m_Device = MTL::CreateSystemDefaultDevice();

        glfwInit();

        m_Specification.WindowSpec.EventCallback = [this](Event& event) { RaiseEvent(event); };

        m_Window = std::make_shared<Window>(m_Specification.WindowSpec);
        m_Window->Create();
    }

    Application::~Application()
    {
        s_Application = nullptr;
        glfwTerminate();
    }

    void Application::RaiseEvent(Event& event)
    {
        m_LayerStack.OnEvent(event);
    }

    void Application::Stop()
    {
        m_Running = false;
    }

    void Application::Run()
    {
        Init();

        float lastTime = GetTime();

        while (m_Running)
        {
            glfwPollEvents();

            if (m_Window->ShouldClose())
            {
                Stop();
                break;
            }

            float currentTime = GetTime();
            float ts = glm::clamp(currentTime - lastTime, 0.001f, 0.1f);
            lastTime = currentTime;

            m_LayerStack.OnUpdate(ts);
            m_LayerStack.OnUIRender();
            m_LayerStack.OnRender();

            m_Window->OnUpdate();
        }

        Shutdown();
    }

    void Application::Init()
    {
    }

    void Application::Update()
    {
        
    }

    void Application::Shutdown()
    {
    }

    Application& Application::Get()
    {
        if (s_Application)
            return *s_Application;
    }

    float Application::GetTime()
    {
        return static_cast<float>(glfwGetTime());
    }
}