#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>

namespace Luna
{
    struct WindowSpecification
    {
        std::string Title = "Window";
        uint32_t Width = 1280;
        uint32_t Height = 720;
    };

    class Window
    {
    public:
        Window(const WindowSpecification& spec = WindowSpecification());
        ~Window();

        void Create();
        void Destroy();
        
        void Update();

        glm::vec2 GetMousePos() const;

        bool ShouldClose() const { return glfwWindowShouldClose(m_WindowHandle); };

        GLFWwindow* GetHandle() { return m_WindowHandle; }
        const GLFWwindow* GetHandle() const { return m_WindowHandle; }
    private:
        WindowSpecification m_Specification;
        GLFWwindow* m_WindowHandle = nullptr;
    };
}