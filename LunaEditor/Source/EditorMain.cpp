#include <Luna.h>

#include <print>

class EditorLayer : public Luna::Layer
{
public:
    EditorLayer()
        : Layer()
    {}
    
    ~EditorLayer()
    {}

    void OnEvent(Luna::Event& event) override
    {
        std::println("Event: {}", event.ToString());
    }

    void OnAttach() override
    {
    }

    void OnDetach() override
    {
    }

    void OnUpdate(float ts) override
    {
    }

    void OnUIRender() override
    {
    }
};

Luna::Application* Luna::CreateApplication()
{
    Luna::ApplicationSpecification spec;
    spec.Name = "Luna Editor";
    spec.WindowSpec.Title = "Luna Editor";
    spec.WindowSpec.Width = 1280;
    spec.WindowSpec.Height = 720;

    Luna::Application *app = new Luna::Application(spec);
    app->PushLayer<EditorLayer>();

    return app;
}