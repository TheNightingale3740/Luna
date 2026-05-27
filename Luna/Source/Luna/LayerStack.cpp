#include "LayerStack.h"

namespace Luna
{
    void LayerStack::PushLayer(std::unique_ptr<Layer> layer)
    {
        layer->OnAttach();
        m_LayerStack.push_back(std::move(layer));
    }

    void LayerStack::Update(float ts)
    {
        for (const auto& layer : m_LayerStack)
        {
            layer->OnUpdate(ts);
        }
    }
}