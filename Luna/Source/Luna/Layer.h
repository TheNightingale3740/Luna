#pragma once

#include "Luna/Events/Event.h"

namespace Luna
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