#pragma once

#include "Radiance/Events/Event.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <QuartzCore/CAMetalLayer.hpp>

#include <string>

namespace Radiance
{
    struct WindowSpecification
    {
        std::string Title = "Window";
        uint32_t Width = 1280;
        uint32_t Height = 720;

        bool CustomTitlebar = false;

        using EventCallbackFn = std::function<void(Event&)>;
		EventCallbackFn EventCallback;
    };

    class Window
    {
    public:
        Window(const WindowSpecification& spec = WindowSpecification());
        ~Window();

        void Create();
        void Destroy();
        
        void OnUpdate();
        void RaiseEvent(Event& event);

        bool ShouldClose() const { return glfwWindowShouldClose(m_WindowHandle); };
        void BeginWindowDrag() const;

        CA::MetalDrawable* GetCurrentDrawable() const { return m_MetalLayer->nextDrawable(); }
        
        glm::vec2 GetWindowPosition() const;
        glm::vec2 GetMousePos() const;

        GLFWwindow* GetHandle() { return m_WindowHandle; }
        const GLFWwindow* GetHandle() const { return m_WindowHandle; }
    protected:
        void ResizeDrawable(uint32_t width, uint32_t height);
    private:
        void SetWindowEventCallbacks();
    private:
        GLFWwindow* m_WindowHandle = nullptr;
        CA::MetalLayer* m_MetalLayer = nullptr;
        
        WindowSpecification m_Specification;
    };
}