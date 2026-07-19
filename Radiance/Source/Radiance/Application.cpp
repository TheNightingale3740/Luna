#include "Application.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_metal.h>

#include <filesystem>

namespace Radiance
{
    static Application *s_Application = nullptr;

    Application::Application(const ApplicationSpecification& specification)
        : m_Specification(specification)
    {
        std::filesystem::current_path(GetExecutableDir().parent_path());

        s_Application = this;
        Log::Init();
        
        m_Device = MTL::CreateSystemDefaultDevice();
        m_CommandQueue = m_Device->newCommandQueue();

        glfwInitHint(GLFW_COCOA_CHDIR_RESOURCES, GLFW_FALSE);
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
            renderPassDescriptor->colorAttachments()->object(0)->setClearColor(MTL::ClearColor(1, 0, 1, 1));
            renderPassDescriptor->colorAttachments()->object(0)->setStoreAction(MTL::StoreActionStore);

            MTL::RenderCommandEncoder *encoder = commandBuffer->renderCommandEncoder(renderPassDescriptor);
            
            ImGuiBackend::NewFrame(renderPassDescriptor);

            if (m_Specification.WindowSpec.CustomTitlebar)
            {
                DrawTitlebar();
            }
            else
            {
                ImGui::DockSpaceOverViewport();
            }

            m_LayerStack.OnUpdate(ts);
            m_LayerStack.OnUIRender();
            m_LayerStack.OnRender();

            m_Window->OnUpdate();

            ImGuiBackend::Render(ImGui::GetDrawData(), commandBuffer, encoder);


            ImGuiIO& io = ImGui::GetIO();
            
            // // Update and Render additional Platform Windows
            // if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            // {
            //     ImGui::UpdatePlatformWindows();
            //     ImGui::RenderPlatformWindowsDefault();
            // }

            encoder->endEncoding();
            commandBuffer->presentDrawable(drawable);
            commandBuffer->commit();

            pool->release();
        }

        Shutdown();
    }

    void Application::DrawTitlebar()
    {
        constexpr uint32_t titlebarHeight = 50;
     
        ImGuiWindowFlags window_flags = 0;
        ImVec2 titlebarSize = ImVec2(ImGui::GetIO().DisplaySize.x, titlebarHeight);

        ImDrawList *drawList = ImGui::GetBackgroundDrawList();
        drawList->AddRectFilled(ImVec2(0, 0), titlebarSize, ImColor(15, 15, 15, 255), 0.0f);
        
        ImVec2 textSize = ImGui::CalcTextSize(m_Specification.Name.c_str());
        drawList->AddText(ImVec2(ImGui::GetIO().DisplaySize.x / 2 - (textSize.x / 2), titlebarHeight / 2 - (textSize.y / 2)), ImColor(255, 255, 255, 255), m_Specification.Name.c_str(), nullptr);

        // Draw an invisible button for custom window dragging
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(titlebarSize);
        ImGui::SetNextWindowBgAlpha(0.0f);
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | 
                                ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs & ~ImGuiWindowFlags_NoInputs |
                                ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNav;
        ImGui::Begin("##TitlebarDragZone", nullptr, flags);
        ImGui::InvisibleButton("Drag Zone", titlebarSize);

        // Handle titlenbar drag
        if (ImGui::IsItemActive())
        {
            m_Window->BeginWindowDrag();
        }
        ImGui::End();

        // Fullscreen dockspace: practically the same as calling DockSpaceOverViewport();
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos({viewport->WorkPos.x, viewport->WorkPos.y + titlebarHeight});
        ImGui::SetNextWindowSize({viewport->WorkSize.x, viewport->WorkSize.y - titlebarHeight});
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        window_flags |= ImGuiWindowFlags_NoBackground;
        window_flags |= ImGuiWindowFlags_NoDecoration;
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));

        ImGui::Begin("DockSpace", nullptr, window_flags);
        ImGui::DockSpace(ImGui::GetID("DockSpace"));
        ImGui::End();
        ImGui::PopStyleVar(3);
    }

    void Application::Init()
    {
    }

    void Application::Update()
    {
        
    }

    void Application::Shutdown()
    {
        Log::Shutdown();
    }

    Application* Application::Get()
    {
        return s_Application;
    }

    float Application::GetTime()
    {
        return static_cast<float>(glfwGetTime());
    }
}