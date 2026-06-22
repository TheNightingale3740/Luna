#pragma once

#include <Metal/Metal.hpp>

namespace Radiance
{

    struct TextureSpecification
    {
        uint32_t Width;
        uint32_t Height;

        MTL::PixelFormat PixelFormat;
    };

    class Texture
    {
    public:
        Texture(const TextureSpecification& specification = TextureSpecification());
        ~Texture();

        void Create();
        void Destroy();

        void Reisze(uint32_t newWidth, uint32_t newHeight);

        MTL::Texture* GetHandle() { return m_TextureHandle; }
        const MTL::Texture* GetHandle() const { return m_TextureHandle; }
    private:
        MTL::Texture* m_TextureHandle;
    };

}