#include "LayerStack.h"

#include <ranges>

namespace Luna
{
    void LayerStack::PushLayer(std::unique_ptr<Layer> layer)
    {
        layer->OnAttach();
        m_LayerStack.push_back(std::move(layer));
    }

    void LayerStack::OnUpdate(float ts) const
    {
        for (const auto& layer : m_LayerStack)
        {
            layer->OnUpdate(ts);
        }
    }

    void LayerStack::OnUIRender() const
    {
        for (const auto& layer : m_LayerStack)
        {
            layer->OnUIRender();
        }
    }

    void LayerStack::OnRender() const
    {
        for (const auto& layer : m_LayerStack)
        {
            layer->OnRender();
        }
    }

    void LayerStack::OnEvent(Event& event)
    {
        for (auto& layer : std::views::reverse(m_LayerStack))
        {
            layer->OnEvent(event);
            if (event.Handled)
                break;
        }
    }
}