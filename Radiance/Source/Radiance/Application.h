#pragma once

#include <Metal/Metal.hpp>

#include "Radiance/Window.h"
#include "Radiance/Core/LayerStack.h"
#include "Radiance/Events/Event.h"
#include "Radiance/ImGui/ImGuiBackend.h"
#include "Radiance/Utils/FileSystem.h"
#include "Radiance/Core/Log.h"

#include <string>

namespace Radiance
{
    struct ApplicationSpecification
    {
        std::string Name = "Application";
        bool Dockspace = true;
        WindowSpecification WindowSpec;
    };

    class Application
    {
    public:
        Application(const ApplicationSpecification& specification = ApplicationSpecification());
        ~Application();
        
        void Init();
        void Update();
        void Shutdown();

        void Run();
        void Stop();

        void RaiseEvent(Event& event);

        template <typename TLayer>
        requires std::is_base_of_v<Layer, TLayer>
        void PushLayer()
        {
            m_LayerStack.PushLayer(std::make_unique<TLayer>());
        }

        template <typename TLayer>
        requires std::is_base_of_v<Layer, TLayer>
        TLayer* GetLayer()
        {
            return m_LayerStack.GetLayer<TLayer>();
        }

        std::shared_ptr<Window> GetMainWindow() const { return m_Window; }
        
        MTL::Device* GetDevice() { return m_Device; }
        const MTL::Device* GetDevice() const { return m_Device; }
        
        MTL::CommandQueue* GetCommandQueue() const { return m_CommandQueue; }
        
        static Application* Get();
        static float GetTime();
    private:
        void DrawTitlebar();
    private:
        std::shared_ptr<Window> m_Window = nullptr;

        MTL::Device* m_Device = nullptr;
        MTL::CommandQueue *m_CommandQueue = nullptr;

        LayerStack m_LayerStack;
        ApplicationSpecification m_Specification;

        bool m_Running = true;
    };
}