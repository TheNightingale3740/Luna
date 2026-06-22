#pragma once

#include <Metal/Metal.hpp>

#include <imgui.h>

namespace Radiance
{
    class ImGuiBackend
    {
    public:
        static void Init();
        static void Shutdown();

        static void NewFrame(MTL::RenderPassDescriptor* renderPassDescriptor);
        static void Render(ImDrawData* drawData, MTL::CommandBuffer* commandBuffer, MTL::RenderCommandEncoder* renderEncoder);
    };
}