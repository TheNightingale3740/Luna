#include "MetalContext.h"

#import <Cocoa/Cocoa.h>
#import <Metal/Metal.h>
#import <QuartzCore/QuartzCore.h>
#import <QuartzCore/CAMetalLayer.h>

#define GLFW_EXPOSE_NATIVE_COCOA
#import <GLFW/glfw3.h>
#import <GLFW/glfw3native.h>

#import <glm/glm.hpp>

namespace Luna
{
    void AttachMetalLayerToWindow(GLFWwindow* windowHandle, CA::MetalLayer* metalLayer, bool titlebar)
    {
        NSWindow *nsWindow = glfwGetCocoaWindow(windowHandle);
        nsWindow.contentView.layer = (__bridge CAMetalLayer*)metalLayer;
        nsWindow.contentView.wantsLayer = YES;
        if (!titlebar)
        {
            [nsWindow setTitlebarAppearsTransparent:YES];
            [nsWindow setTitleVisibility:NSWindowTitleHidden];
            nsWindow.styleMask |= NSWindowStyleMaskFullSizeContentView;
        }
    }

    void BeginWindowDrag(GLFWwindow* windowHandle)
    {
        NSWindow *nsWindow = glfwGetCocoaWindow(windowHandle);
        NSEvent *event = [NSApp currentEvent];
        [nsWindow performWindowDragWithEvent:event];
    }
}