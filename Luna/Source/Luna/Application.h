#pragma once

#include "Luna/Window.h"
#include "Luna/LayerStack.h"

#include <string>
#include <iostream>

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

        void Init();
        void Update();
        void Shutdown();

        static Application& Get();
    private:
        ApplicationSpecification m_Specification;

        std::shared_ptr<Window> m_Window = nullptr;
        LayerStack m_LayerStack;

        friend class Layer;
    };
}