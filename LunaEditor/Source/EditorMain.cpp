#include <Luna.h>

#include <print> // TODO: Need a proper logging system

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
        ImGui::ShowDemoWindow();

        ImGui::Begin("Hello, ImGui!");
        ImGui::Text("This is a simple text in the editor layer.");

        if (ImGui::Button("Click me"))
            m_Text = true;
        if (m_Text)
            ImGui::Text("Hello :P!");

        ImGui::End();
    }
private:
    bool m_Text = false;
};

std::unique_ptr<Luna::Application> Luna::CreateApplication()
{
    Luna::ApplicationSpecification spec;
    spec.Name = "Luna Editor";
    spec.Dockspace = true;
    spec.WindowSpec.CustomTitlebar = true;
    spec.WindowSpec.Title = "Luna Editor";
    spec.WindowSpec.Width = 1280;
    spec.WindowSpec.Height = 720;

    std::unique_ptr<Luna::Application> app = std::make_unique<Luna::Application>(spec);
    app->PushLayer<EditorLayer>();
    return std::move(app);
}