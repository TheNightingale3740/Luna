#include "ImGuiBackend.h"

#include "Luna/Application.h"

#include <GLFW/glfw3.h>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_metal.h>

#import <Metal/Metal.h>
#import <QuartzCore/QuartzCore.h>

namespace Luna
{
    void ImGuiBackend::Init()
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking

        ImGui::StyleColorsDark();
        ImVec4* colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_WindowBg]               = ImVec4(0.14f, 0.14f, 0.14f, 1.0f);
        colors[ImGuiCol_PopupBg]                = ImVec4(0.11f, 0.06f, 0.12f, 0.94f);
        colors[ImGuiCol_FrameBg]                = ImVec4(0.28f, 0.28f, 0.28f, 0.54f);
        colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.31f, 0.31f, 0.31f, 0.40f);
        colors[ImGuiCol_FrameBgActive]          = ImVec4(0.76f, 0.59f, 1.00f, 0.67f);
        colors[ImGuiCol_TitleBg]                = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
        colors[ImGuiCol_TitleBgActive]          = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
        colors[ImGuiCol_MenuBarBg]              = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
        colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.42f, 0.42f, 0.42f, 1.00f);
        colors[ImGuiCol_CheckboxSelectedBg]     = ImVec4(0.08f, 0.08f, 0.08f, 0.45f);
        colors[ImGuiCol_SliderGrab]             = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
        colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
        colors[ImGuiCol_Button]                 = ImVec4(0.70f, 0.70f, 0.70f, 0.40f);
        colors[ImGuiCol_ButtonHovered]          = ImVec4(0.72f, 0.43f, 1.00f, 1.00f);
        colors[ImGuiCol_ButtonActive]           = ImVec4(0.72f, 0.46f, 1.00f, 1.00f);
        colors[ImGuiCol_Header]                 = ImVec4(0.35f, 0.35f, 0.35f, 0.31f);
        colors[ImGuiCol_HeaderHovered]          = ImVec4(0.31f, 0.31f, 0.31f, 0.80f);
        colors[ImGuiCol_HeaderActive]           = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
        colors[ImGuiCol_Separator]              = ImVec4(0.27f, 0.27f, 0.27f, 0.50f);
        colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.31f, 0.31f, 0.31f, 0.78f);
        colors[ImGuiCol_SeparatorActive]        = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
        colors[ImGuiCol_ResizeGrip]             = ImVec4(0.57f, 0.57f, 0.57f, 0.20f);
        colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.54f, 0.54f, 0.54f, 0.67f);
        colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.62f, 0.62f, 0.62f, 0.95f);
        colors[ImGuiCol_TabHovered]             = ImVec4(0.38f, 0.38f, 0.38f, 0.80f);
        colors[ImGuiCol_Tab]                    = ImVec4(0.26f, 0.26f, 0.26f, 0.86f);
        colors[ImGuiCol_TabSelected]            = ImVec4(0.44f, 0.44f, 0.44f, 1.00f);
        colors[ImGuiCol_TabSelectedOverline]    = ImVec4(0.67f, 0.67f, 0.67f, 1.00f);
        colors[ImGuiCol_TabDimmed]              = ImVec4(0.28f, 0.28f, 0.28f, 0.97f);
        colors[ImGuiCol_TabDimmedSelected]      = ImVec4(0.23f, 0.23f, 0.23f, 1.00f);
        colors[ImGuiCol_DockingPreview]         = ImVec4(0.24f, 0.24f, 0.24f, 0.70f);
        colors[ImGuiCol_NavCursor]              = ImVec4(0.33f, 0.34f, 0.35f, 1.00f);

        float main_scale = ImGui_ImplGlfw_GetContentScaleForMonitor(glfwGetPrimaryMonitor()); // Valid on GLFW 3.3+ only

        // Setup scaling
        ImGuiStyle& style = ImGui::GetStyle();
        style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
        style.FontScaleDpi = main_scale;        // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)
        io.ConfigDpiScaleFonts = true;          // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.
        io.ConfigDpiScaleViewports = true;      // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.

        io.Fonts->AddFontFromFileTTF("Luna/Source/Luna/Res/Fonts/Inter.ttf", 16.0f);

        GLFWwindow *window = Application::Get().GetMainWindow()->GetHandle();
        MTL::Device *device = Application::Get().GetDevice();

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplMetal_Init((__bridge id<MTLDevice>) device);
    }

    void ImGuiBackend::NewFrame(MTL::RenderPassDescriptor* renderPassDescriptor)
    {
        ImGui_ImplMetal_NewFrame((__bridge MTLRenderPassDescriptor*)renderPassDescriptor);
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void ImGuiBackend::Render(ImDrawData* drawData, MTL::CommandBuffer* commandBuffer, MTL::RenderCommandEncoder* renderEncoder)
    {
        ImGui::Render();
        ImGui_ImplMetal_RenderDrawData(ImGui::GetDrawData(), (__bridge id<MTLCommandBuffer>)commandBuffer, (__bridge id<MTLRenderCommandEncoder>)renderEncoder);
    }

    void ImGuiBackend::Shutdown()
    {
        // Cleanup
        ImGui_ImplMetal_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
}