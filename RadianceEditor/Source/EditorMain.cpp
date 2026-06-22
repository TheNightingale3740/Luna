#include <Radiance.h>

#include <print> // TODO: Need a proper logging system

class EditorLayer : public Radiance::Layer
{
public:
    EditorLayer()
        : Layer()
    {}
    
    ~EditorLayer()
    {}

    void OnEvent(Radiance::Event& event) override
    {
    }

    void OnAttach() override
    {

    }

    void OnDetach() override
    {
        if (m_Texture)
        {
            m_Texture->release();
            m_Texture = nullptr;
        }

        delete[] m_ImageData;
    }

    void OnUpdate(float ts) override
    {
    }

    void OnUIRender() override
    {
        ImGui::Begin("Viewport");

        ImGui::Image(m_Texture, ImVec2((float)m_Texture->width(), (float)m_Texture->height()));

        ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        m_ViewportWidth = viewportSize.x;
        m_ViewportHeight = viewportSize.y;

        ImGui::End();

        ImGui::Begin("Settings");
        if (ImGui::Button("Render"))
        {
            Render();
        }
        ImGui::End();
    }
private:
    void Render()
    {
        // Rendering goes here
        MTL::TextureDescriptor* textureDescriptor = MTL::TextureDescriptor::alloc()->init();
        textureDescriptor->setWidth(m_ViewportWidth);
        textureDescriptor->setHeight(m_ViewportHeight);
        textureDescriptor->setPixelFormat(MTL::PixelFormat(MTL::PixelFormatBGRA8Unorm));
        textureDescriptor->setTextureType(MTL::TextureType2D);
        textureDescriptor->setUsage(MTL::TextureUsageShaderWrite | MTL::TextureUsageShaderRead);
        textureDescriptor->setStorageMode(MTL::StorageModeShared);

        m_Texture = Radiance::Application::Get()->GetDevice()->newTexture(textureDescriptor);

        m_ImageData = new uint32_t[m_ViewportWidth * m_ViewportHeight];

        for (uint32_t y = 0; y < m_ViewportHeight; y++)
        {
            for (uint32_t x = 0; x < m_ViewportWidth; x++)
            {
                m_ImageData[x + y * m_ViewportWidth] = 0xffff0000;
            }
        }

        m_Texture->replaceRegion(
            MTL::Region(0, 0, m_ViewportWidth, m_ViewportHeight),
            0, m_ImageData, m_ViewportWidth * sizeof(uint32_t)
        );
    }
private:
    MTL::Texture* m_Texture = nullptr;
    uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

    uint32_t *m_ImageData = nullptr;
};

std::unique_ptr<Radiance::Application> Radiance::CreateApplication()
{
    Radiance::ApplicationSpecification spec;
    spec.Name = "Radiance Editor";
    spec.Dockspace = true;
    spec.WindowSpec.CustomTitlebar = true;
    spec.WindowSpec.Title = "Radiance Editor";
    spec.WindowSpec.Width = 1280;
    spec.WindowSpec.Height = 720;

    std::unique_ptr<Radiance::Application> app = std::make_unique<Radiance::Application>(spec);
    app->PushLayer<EditorLayer>();
    return std::move(app);
}