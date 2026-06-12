#pragma once

#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>
#include <QuartzCore/CAMetalLayer.hpp>

#include <GLFW/glfw3.h>

namespace Luna
{

    void AttachMetalLayerToWindow(GLFWwindow* windowHandle, CA::MetalLayer* metalLayer, bool titlebar = false);
    void BeginWindowDrag(GLFWwindow* windowHandle);

}