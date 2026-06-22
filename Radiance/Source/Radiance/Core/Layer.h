#pragma once

#include "Radiance/Events/Event.h"

namespace Radiance
{
    class Layer
    {
    public:
        Layer() = default;
        virtual ~Layer() = default;

        virtual void OnAttach() {}
        virtual void OnDetach() {}

        virtual void OnEvent(Event& event) {}

        virtual void OnUpdate(float ts) {}
        virtual void OnUIRender() {}

        virtual void OnRender() {}
    };
}