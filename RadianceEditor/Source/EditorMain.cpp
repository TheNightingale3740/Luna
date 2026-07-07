#include <Radiance.h>
#include <Radiance/Core/Log.h>
#include <Radiance/Utils/Timer.h>
#include <Radiance/Renderer/Renderer.h>

class EditorLayer : public Radiance::Layer
{
public:
    EditorLayer()
        : Layer()
    {
        RD_LOG_INFO("EditorLayer created");
    }

    void OnUIRender() override
    {

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("Viewport");

        ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        m_ViewportWidth = viewportSize.x;
        m_ViewportHeight = viewportSize.y;

        Render();

        if (m_Renderer.GetFinalImage())
            ImGui::Image(m_Renderer.GetFinalImage(), ImVec2((float)m_Renderer.GetFinalImage()->width(), (float)m_Renderer.GetFinalImage()->height()),
                ImVec2(0, 1), ImVec2(1, 0));

        ImGui::End();
        ImGui::PopStyleVar();

        ImGui::Begin("Settings");
        ImGui::Text("Last Render Time: %.3f ms", m_LastRenderTime);
        ImGui::Text("Viewport Size: %d x %d", m_ViewportWidth, m_ViewportHeight);
        ImGui::Text("You suck -_-");
        ImGui::End();
    }

    void OnEvent(Radiance::Event& event) override
    {
    }
private:
    void Render()
    {
        Radiance::Timer timer;
        m_Renderer.Resize(m_ViewportWidth, m_ViewportHeight);
        m_Renderer.Render();

        m_LastRenderTime = timer.ElapsedMillis();
    }
private:
    Radiance::Renderer m_Renderer;

    uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

    float m_LastRenderTime = 0.0f;
};

std::unique_ptr<Radiance::Application> Radiance::CreateApplication()
{
    Radiance::ApplicationSpecification spec;
    spec.Name = "Radiance";
    spec.Dockspace = true;
    spec.WindowSpec.CustomTitlebar = true;
    spec.WindowSpec.Title = "Radiance Editor";
    spec.WindowSpec.Width = 1280;
    spec.WindowSpec.Height = 720;

    std::unique_ptr<Radiance::Application> app = std::make_unique<Radiance::Application>(spec);
    app->PushLayer<EditorLayer>();
    return app;
}