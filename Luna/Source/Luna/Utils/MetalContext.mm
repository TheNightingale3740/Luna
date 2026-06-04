#include "MetalContext.h"

#import <Cocoa/Cocoa.h>
#import <Metal/Metal.h>
#import <QuartzCore/QuartzCore.h>
#import <QuartzCore/CAMetalLayer.h>

#define GLFW_EXPOSE_NATIVE_COCOA
#import <GLFW/glfw3.h>
#import <GLFW/glfw3native.h>

namespace Luna
{
    void AttachMetalLayerToWindow(GLFWwindow* windowHandle, CA::MetalLayer* metalLayer)
    {
        NSWindow *nsWindow = glfwGetCocoaWindow(windowHandle);
        nsWindow.contentView.layer = (__bridge CAMetalLayer*)metalLayer;
        nsWindow.contentView.wantsLayer = YES;
    }
}