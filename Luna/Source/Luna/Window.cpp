#include "Luna/Window.h"

namespace Luna
{
    Window::Window(const WindowSpecification& spec)
        : m_Specification(spec)
    {
    }

    Window::~Window()
    {
        Destroy();
    }

    void Window::Create()
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_WindowHandle = glfwCreateWindow((int)m_Specification.Width, (int)m_Specification.Height, m_Specification.Title.c_str(), nullptr, nullptr);
    }

    void Window::Destroy()
    {
        if (m_WindowHandle)
            glfwDestroyWindow(m_WindowHandle);
        
        m_WindowHandle = nullptr;
    }

    void Window::Update()
    {
        glfwPollEvents();
    }

    glm::vec2 Window::GetMousePos() const
    {
        double x, y;
        glfwGetCursorPos(m_WindowHandle, &x, &y);
        return { static_cast<float>(x), static_cast<float>(y) };
    }
}