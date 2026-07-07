#include "Renderer.h"

#include "Radiance/Application.h"

namespace Radiance
{

    Renderer::Renderer()
    {
        NS::String* shaderLibraryPath = NS::String::string("Resources/Shaders/PathTracingShader.metallib", NS::StringEncoding::UTF8StringEncoding);

        NS::Error* error = nullptr;
        m_ShaderLibrary = Application::Get()->GetDevice()->newLibrary(shaderLibraryPath, &error);

        if (error)
        {
            error->release();
        }
        shaderLibraryPath->release();

        NS::String* computeFunctionName = NS::String::string("PerPixel", NS::StringEncoding::UTF8StringEncoding);
        MTL::Function* computeFunction = m_ShaderLibrary->newFunction(computeFunctionName);

        error = nullptr;

        m_PipelineState = Application::Get()->GetDevice()->newComputePipelineState(computeFunction, &error);
        if (error)
        {
            error->release();
        }

        computeFunctionName->release();
        computeFunction->release();

        m_UniformBuffer = Application::Get()->GetDevice()->newBuffer(sizeof(FrameConstants), MTL::ResourceOptions(MTL::ResourceStorageModeShared));

        FrameConstants constants;
        constants.Time = Application::GetTime();
        memcpy(m_UniformBuffer->contents(), &constants, sizeof(FrameConstants));
    }

    Renderer::~Renderer()
    {
        if (m_FinalImage)
        {
            m_FinalImage->release();
        }

        if (m_PipelineState)
        {
            m_PipelineState->release();
        }

        if (m_ShaderLibrary)
        {
            m_ShaderLibrary->release();
        }

        if (m_UniformBuffer)
        {
            m_UniformBuffer->release();
        }
    }

    void Renderer::Resize(uint32_t width, uint32_t height)
    {
        if (!m_FinalImage || m_FinalImage->width() != width || m_FinalImage->height() != height)
        {
            MTL::TextureDescriptor *textureDescriptor = MTL::TextureDescriptor::alloc()->init();
            textureDescriptor->setWidth(width);
            textureDescriptor->setHeight(height);
            textureDescriptor->setPixelFormat(MTL::PixelFormat(MTL::PixelFormatRGBA8Unorm));
            textureDescriptor->setTextureType(MTL::TextureType2D);
            textureDescriptor->setUsage(MTL::TextureUsageShaderWrite | MTL::TextureUsageShaderRead);
            textureDescriptor->setStorageMode(MTL::StorageModePrivate);
            textureDescriptor->setMipmapLevelCount(1);

            if (m_FinalImage)
            {
                m_FinalImage->release();
            }
            m_FinalImage = Application::Get()->GetDevice()->newTexture(textureDescriptor);

            textureDescriptor->release();
        }
    }

    void Renderer::Render()
    {
        const MTL::Size gridSize = MTL::Size(m_FinalImage->width(), m_FinalImage->height(), 1);
        const MTL::Size threadGroupSize = MTL::Size(8, 8, 1);

        FrameConstants constants;
        constants.Time = Application::GetTime();
        memcpy(m_UniformBuffer->contents(), &constants, sizeof(FrameConstants));

        // Render stuff
        MTL::CommandBuffer* commandBuffer = Application::Get()->GetCommandQueue()->commandBuffer();
        MTL::ComputeCommandEncoder* computeEncoder = commandBuffer->computeCommandEncoder();
        computeEncoder->setComputePipelineState(m_PipelineState);
        computeEncoder->setTexture(m_FinalImage, 0);
        computeEncoder->setBuffer(m_UniformBuffer, 0, 0);
        computeEncoder->dispatchThreads(gridSize, threadGroupSize);

        computeEncoder->endEncoding();
        commandBuffer->commit();
    }

}