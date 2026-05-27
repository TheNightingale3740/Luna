#pragma once

#include "Luna/Window.h"
#include "Luna/LayerStack.h"

#include "Luna/Events/Event.h"

#include <string>

namespace Luna
{
    struct ApplicationSpecification
    {
        std::string Name = "Application";
        WindowSpecification WindowSpec;
    };

    class Application
    {
    public:
        Application(const ApplicationSpecification& specification = ApplicationSpecification());
        ~Application();

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

        void Run();
        void Stop();

        void RaiseEvent(Event& event);

        void Init();
        void Update();
        void Shutdown();

        static Application& Get();
        static float GetTime();
    private:
        ApplicationSpecification m_Specification;

        std::shared_ptr<Window> m_Window = nullptr;
        LayerStack m_LayerStack;

        bool m_Running = true;

        friend class Layer;
    };
}