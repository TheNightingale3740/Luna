#include "Application.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_metal.h>

#include <print>

namespace Luna
{
    static Application *s_Application = nullptr;

    Application::Application(const ApplicationSpecification& specification)
        : m_Specification(specification)
    {
        s_Application = this;
        
        m_Device = MTL::CreateSystemDefaultDevice();
        m_CommandQueue = m_Device->newCommandQueue();

        glfwInit();

        m_Specification.WindowSpec.EventCallback = [this](Event& event) { RaiseEvent(event); };

        m_Window = std::make_shared<Window>(m_Specification.WindowSpec);
        m_Window->Create();

        ImGuiBackend::Init();
    }

    Application::~Application()
    {   
        if (m_Device)
        {
            m_Device->release();
            m_Device = nullptr;
        }
        if (m_CommandQueue)
        {
            m_CommandQueue->release();
            m_CommandQueue = nullptr;
        }

        ImGuiBackend::Shutdown();

        s_Application = nullptr;
        glfwTerminate();
    }

    void Application::RaiseEvent(Event& event)
    {
        m_LayerStack.OnEvent(event);
    }

    void Application::Stop()
    {
        m_Running = false;
    }

    void Application::Run()
    {
        Init();

        float lastTime = GetTime();

        MTL::RenderPassDescriptor *renderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();

        while (m_Running)
        {
            NS::AutoreleasePool *pool = NS::AutoreleasePool::alloc()->init();
            
            glfwPollEvents();

            if (m_Window->ShouldClose())
            {
                Stop();
                break;
            }

            float currentTime = GetTime();
            float ts = glm::clamp(currentTime - lastTime, 0.001f, 0.1f);
            lastTime = currentTime;

            CA::MetalDrawable* drawable = m_Window->GetCurrentDrawable();

            MTL::CommandBuffer *commandBuffer = m_CommandQueue->commandBuffer();
            renderPassDescriptor->colorAttachments()->object(0)->setTexture(drawable->texture());
            renderPassDescriptor->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionClear);
            renderPassDescriptor->colorAttachments()->object(0)->setClearColor(MTL::ClearColor(0, 0, 0, 1));
            renderPassDescriptor->colorAttachments()->object(0)->setStoreAction(MTL::StoreActionStore);

            MTL::RenderCommandEncoder *encoder = commandBuffer->renderCommandEncoder(renderPassDescriptor);
            
            ImGuiBackend::NewFrame(renderPassDescriptor);

            if (m_Specification.Dockspace)
                ImGui::DockSpaceOverViewport();

            m_LayerStack.OnUpdate(ts);
            m_LayerStack.OnUIRender();
            m_LayerStack.OnRender();

            m_Window->OnUpdate();

            ImGuiBackend::Render(ImGui::GetDrawData(), commandBuffer, encoder);


            ImGuiIO& io = ImGui::GetIO();
            
            // Update and Render additional Platform Windows
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
            }

            encoder->endEncoding();
            commandBuffer->presentDrawable(drawable);
            commandBuffer->commit();

            pool->release();
        }

        Shutdown();
    }

    void Application::Init()
    {
    }

    void Application::Update()
    {
        
    }

    void Application::Shutdown()
    {
    }

    Application& Application::Get()
    {
        if (s_Application)
            return *s_Application;
    }

    float Application::GetTime()
    {
        return static_cast<float>(glfwGetTime());
    }
}