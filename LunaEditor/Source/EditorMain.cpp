#include <Luna.h>

#include <print> // TODO: Need a proper logging system

class EditorLayer : public Luna::Layer
{
public:
    EditorLayer()
        : Layer()
    {
    }
    
    ~EditorLayer()
    {}

    void OnEvent(Luna::Event& event) override
    {
        //std::println("Event: {}", event.ToString());
    }

    void OnAttach() override
    {
        m_CommandQueue = Luna::Application::Get().GetDevice()->newCommandQueue();
    }

    void OnDetach() override
    {
    }

    void OnUpdate(float ts) override
    {
        NS::AutoreleasePool *pool = NS::AutoreleasePool::alloc()->init();

        // Start rendering here
        CA::MetalDrawable* drawable = Luna::Application::Get().GetMainWindow()->GetCurrentDrawable();
        MTL::CommandBuffer* commandBuffer = m_CommandQueue->commandBuffer();

        MTL::RenderPassDescriptor *renderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();

        MTL::RenderPassColorAttachmentDescriptor* colorAttachment = renderPassDescriptor->colorAttachments()->object(0);
        colorAttachment->setTexture(drawable->texture());
        colorAttachment->setLoadAction(MTL::LoadActionClear);
        colorAttachment->setClearColor(MTL::ClearColor(1, 0, 1, 1));
        colorAttachment->setStoreAction(MTL::StoreActionStore);

        MTL::RenderCommandEncoder *encoder = commandBuffer->renderCommandEncoder(renderPassDescriptor);
        encoder->endEncoding();

        commandBuffer->presentDrawable(drawable);
        commandBuffer->commit();
        commandBuffer->waitUntilCompleted();

        renderPassDescriptor->release();

        pool->release();
    }

    void OnUIRender() override
    {
    }
private:
    MTL::CommandQueue* m_CommandQueue = nullptr;
};

std::unique_ptr<Luna::Application> Luna::CreateApplication()
{
    Luna::ApplicationSpecification spec;
    spec.Name = "Luna Editor";
    spec.WindowSpec.Title = "Luna Editor";
    spec.WindowSpec.Width = 1280;
    spec.WindowSpec.Height = 720;

    std::unique_ptr<Luna::Application> app = std::make_unique<Luna::Application>(spec);
    app->PushLayer<EditorLayer>();
    return std::move(app);
}