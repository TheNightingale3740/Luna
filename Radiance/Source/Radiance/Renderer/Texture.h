#pragma once

#include <Metal/Metal.hpp>

namespace Radiance
{

    enum class TextureType
    {
        Texture2D = 0,
        Texture3D
    };

    struct TextureSpecification
    {
        uint32_t Width = 1280;
        uint32_t Height = 720;

        MTL::PixelFormat PixelFormat = MTL::PixelFormat(MTL::PixelFormatBGRA8Unorm);
        TextureType TextrueType = TextureType::Texture2D;
    };

    class Texture
    {
    public:
        Texture() = default;
        virtual ~Texture() = default;

        virtual void Create() = 0;
        virtual void Destroy() = 0;

        virtual void Reisze(uint32_t newWidth, uint32_t newHeight) = 0;
        const MTL::Texture* GetHandle() const { return m_TextureHandle; }
    private:
        MTL::Texture* m_TextureHandle;
    };

    class Texture2D : public Texture
    {
    public:
        Texture2D(const TextureSpecification& Specification = TextureSpecification());
        ~Texture2D();

        void Create() override;
    };

}