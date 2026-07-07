#pragma once

#include <Metal/Metal.hpp>
#include <cstdint>

namespace Radiance
{

    struct FrameConstants
    {
        float Time = 0.0f;
    };

    class Renderer
    {
    public:
        Renderer();
        ~Renderer();

        void Resize(uint32_t width, uint32_t height);
        void Render();

        MTL::Texture* GetFinalImage() { return m_FinalImage; }
        const MTL::Texture* GetFinalImage() const { return m_FinalImage; }
    private:
        MTL::Texture* m_FinalImage = nullptr;

        MTL::Library* m_ShaderLibrary = nullptr;
        MTL::ComputePipelineState* m_PipelineState = nullptr;

        MTL::Buffer* m_UniformBuffer = nullptr;
    };

}