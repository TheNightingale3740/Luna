#include "Application.h"

namespace Luna
{
    Application::Application(const ApplicationSpecification& specification)
        : m_Specification(specification), m_Window(specification.WindowSpec)
    {
        glfwInit();
        
        m_Window.Create();
    }

    Application::~Application()
    {
        glfwTerminate();
    }

    void Application::Run()
    {
        Init();
        while (!m_Window.ShouldClose())
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
        m_Window.Update();
    }

    void Application::Shutdown()
    {
    }
}