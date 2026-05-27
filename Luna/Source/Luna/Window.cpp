#include "Luna/Window.h"

namespace Luna
{
    Window::Window(const WindowSpecification& spec)
        : m_Specification(spec)
    {
    }

    Window::~Window()
    {
        glfwDestroyWindow(m_WindowHandle);
    }

    void Window::Create()
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_WindowHandle = glfwCreateWindow((int)m_Specification.Width, (int)m_Specification.Height, m_Specification.Title.c_str(), nullptr, nullptr);
    }

    void Window::Update()
    {
        glfwPollEvents();
    }
}