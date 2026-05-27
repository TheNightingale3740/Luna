#pragma once

#include "Layer.h"

#include <vector>
#include <memory>

namespace Luna
{
    class LayerStack
    {
    public:
        LayerStack() = default;
        ~LayerStack() = default;

        void PushLayer(std::unique_ptr<Layer> layer);
        
        template <typename TLayer>
        requires std::is_base_of_v<Layer, TLayer>
        TLayer* GetLayer()
        {
            for (const auto& layer : m_LayerStack)
            {
                if (auto castedLayer = dynamic_cast<TLayer*>(layer.get()))
                    return castedLayer;
            }

            return nullptr;
        }

        void Update(float ts);
    private:
        std::vector<std::unique_ptr<Layer>> m_LayerStack;
    };
}