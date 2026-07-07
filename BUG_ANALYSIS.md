# Radiance Codebase Bug Analysis Report

## Executive Summary
Found **6 significant bugs** ranging from critical (crashes) to medium severity (design issues). The most critical issues involve unsafe casts, null pointer dereferences, and memory management problems.

---

## Critical Bugs

### 1. ⚠️ CRITICAL: Unsafe Cast in EventDispatcher::Dispatch
**Location:** [Radiance/Source/Radiance/Events/Event.h](Radiance/Source/Radiance/Events/Event.h#L43)  
**Severity:** CRITICAL - Undefined Behavior / Crash

**Problem:**
```cpp
template<typename T>
bool Dispatch(EventFn<T> func)
{
    if (m_Event.GetEventType() == T::GetStaticType() && !m_Event.Handled)
    {
        m_Event.Handled = func(*(T*)&m_Event);  // ❌ UNSAFE CAST
        return true;
    }
    return false;
}
```

The cast `(T*)&m_Event` is not safe because:
- If `m_Event` is not actually of type `T`, the reinterpret_cast will interpret the memory incorrectly
- While the type check `m_Event.GetEventType() == T::GetStaticType()` provides runtime safety, a C-style cast defeats the compiler's safety mechanisms
- This is vulnerable to future modifications where the type check might be removed

**Recommendation:**
```cpp
// Use dynamic_cast for safety
m_Event.Handled = func(static_cast<T&>(m_Event));  // Safer with runtime type guarantee
```

**Impact:** Any event dispatching could cause crashes, memory corruption, or undefined behavior if the type system is bypassed.

---

### 2. ⚠️ CRITICAL: Null Pointer Dereference in Application::Get()
**Location:** [Radiance/Source/Radiance/Application.cpp](Radiance/Source/Radiance/Application.cpp#L54)  
**Severity:** CRITICAL - Crash/Undefined Behavior

**Problem:**
```cpp
static Application *s_Application = nullptr;

Application& Application::Get()
{
    return *s_Application;  // ❌ CRASH if s_Application is nullptr
}
```

The function can be called in these dangerous scenarios:
- Before any Application instance is created
- After Application is destroyed (destructor sets `s_Application = nullptr`)
- From a different thread

**Recommendation:**
```cpp
Application& Application::Get()
{
    if (!s_Application)
        throw std::runtime_error("Application not initialized!");
    return *s_Application;
}
```

Or use a safer pattern like `std::optional` or assert during development.

**Impact:** Immediate crash if `Get()` is called at wrong time in application lifecycle.

---

### 3. ⚠️ CRITICAL: Memory Leak with Manual new/delete
**Location:** [RadianceEditor/Source/EditorMain.cpp](RadianceEditor/Source/EditorMain.cpp#L36)  
**Severity:** CRITICAL - Memory Leak / Exception Safety

**Problem:**
```cpp
int main()
{
    Radiance::Application* app = new Radiance::Application(spec);  // ❌ Manual allocation
    app->PushLayer<EditorLayer>();
    app->Run();
    delete app;  // ❌ Never reached if Run() throws exception
}
```

Issues:
- If `PushLayer()` or `Run()` throws an exception, the `delete` is never called
- If `Run()` terminates the application abnormally, memory leaks
- Manual memory management is error-prone

**Recommendation:**
```cpp
int main()
{
    Radiance::ApplicationSpecification spec;
    spec.Name = "Radiance Editor";
    spec.WindowSpec.Title = "Radiance Editor";
    spec.WindowSpec.Width = 1280;
    spec.WindowSpec.Height = 720;

    auto app = std::make_unique<Radiance::Application>(spec);
    app->PushLayer<EditorLayer>();
    app->Run();
    // Automatically deleted when app goes out of scope
    return 0;
}
```

**Impact:** Memory leaks on exit, exception safety violations.

---

## High Severity Bugs

### 4. ⚠️ HIGH: Potential Null Pointer in Window Callbacks
**Location:** [Radiance/Source/Radiance/Window.cpp](Radiance/Source/Radiance/Window.cpp#L23-L95)  
**Severity:** HIGH - Crash/Undefined Behavior

**Problem:**
```cpp
glfwSetWindowCloseCallback(m_WindowHandle, [](GLFWwindow* handle)
{
    Window& window = *((Window*)glfwGetWindowUserPointer(handle));  // ❌ Unsafe cast
    WindowClosedEvent event;
    window.RaiseEvent(event);
});
```

Issues:
- `glfwGetWindowUserPointer()` could return nullptr
- Multiple lambdas use the same unsafe pattern (lines 28, 36, 44, 71, 77)
- Window could be destroyed while a callback is pending

**Recommendation:**
```cpp
glfwSetWindowCloseCallback(m_WindowHandle, [](GLFWwindow* handle)
{
    Window* pWindow = static_cast<Window*>(glfwGetWindowUserPointer(handle));
    if (!pWindow) return;  // Safety check
    
    WindowClosedEvent event;
    pWindow->RaiseEvent(event);
});
```

**Impact:** Crashes when window callbacks are triggered in edge cases.

---

### 5. ⚠️ HIGH: Missing Null Check in GetMousePos()
**Location:** [Radiance/Source/Radiance/Window.cpp](Radiance/Source/Radiance/Window.cpp#L113)  
**Severity:** HIGH - Undefined Behavior

**Problem:**
```cpp
glm::vec2 Window::GetMousePos() const
{
    double x, y;
    glfwGetCursorPos(m_WindowHandle, &x, &y);  // ❌ m_WindowHandle could be nullptr
    return { static_cast<float>(x), static_cast<float>(y) };
}
```

If `m_WindowHandle` is nullptr, `glfwGetCursorPos` behavior is undefined.

**Recommendation:**
```cpp
glm::vec2 Window::GetMousePos() const
{
    if (!m_WindowHandle)
        return { 0.0f, 0.0f };
    
    double x, y;
    glfwGetCursorPos(m_WindowHandle, &x, &y);
    return { static_cast<float>(x), static_cast<float>(y) };
}
```

**Impact:** Undefined behavior when querying mouse position with invalid window handle.

---

## Medium Severity Bugs

### 6. ⚠️ MEDIUM: const Correctness Violation
**Location:** [Radiance/Source/Radiance/LayerStack.h](Radiance/Source/Radiance/LayerStack.h#L27) and [Radiance/Source/Radiance/LayerStack.cpp](Radiance/Source/Radiance/LayerStack.cpp#L33)  
**Severity:** MEDIUM - Design Issue

**Problem:**
```cpp
void OnEvent(Event& event) const;  // ❌ Method is const
```

But the implementation modifies the event:
```cpp
void LayerStack::OnEvent(Event& event) const
{
    for (auto& layer : std::views::reverse(m_LayerStack))
    {
        layer->OnEvent(event);
        if (event.Handled)  // ❌ Modifies event.Handled
            break;
    }
}
```

**Problem:**
- A `const` method should not modify its parameters
- This violates const-correctness principles
- Could cause issues if the const guarantee is relied upon elsewhere

**Recommendation:**
```cpp
void OnEvent(Event& event);  // Remove const
```

**Impact:** Design flaw that could lead to subtle bugs and violates const-correctness contract.

---

## Summary Table

| # | Bug | File | Severity | Type |
|---|-----|------|----------|------|
| 1 | Unsafe cast in EventDispatcher | Event.h:43 | CRITICAL | Undefined Behavior |
| 2 | Null pointer in Application::Get() | Application.cpp:54 | CRITICAL | Crash |
| 3 | Memory leak in main() | EditorMain.cpp:36 | CRITICAL | Memory/Exception |
| 4 | Null pointer in callbacks | Window.cpp:23-95 | HIGH | Crash |
| 5 | Missing null check GetMousePos() | Window.cpp:113 | HIGH | Undefined Behavior |
| 6 | const correctness violation | LayerStack.h:27 | MEDIUM | Design |

---

## Recommendations Priority

1. **Immediate (before shipping):**
   - Fix Bug #1 (EventDispatcher cast)
   - Fix Bug #2 (Application::Get() null check)
   - Fix Bug #3 (Memory leak in main)
   - Fix Bug #4 (Window callbacks null check)

2. **Important (next sprint):**
   - Fix Bug #5 (GetMousePos null check)
   - Fix Bug #6 (const correctness)

3. **Testing:**
   - Add unit tests for Application lifecycle
   - Test edge cases: rapid create/destroy cycles
   - Test exception handling in entry point
   - Validate event dispatching with various event types

---

## Additional Bugs (Secondary Analysis)

###  CRITICAL: Metal Device Memory Leak7. 
**Location:** [Radiance/Source/Radiance/Application.cpp](Radiance/Source/Radiance/Application.cpp#L14)  
**Severity:** CRITICAL - Memory Leak
 
**Problem:**
```cpp
Application::Application(const ApplicationSpecification& specification)
    : m_Specification(specification)
{
    s_Application = this;
    
    m_Device = MTL:: Never released!CreateSystemDefaultDevice();  // 
```

The Metal device is created but never released in the destructor. Metal objects use reference counting and must be explicitly released.

**Recommendation:**
```cpp
Application::~Application()
{
    if (m_Device)
    {
        m_Device->release();  // Release Metal device
        m_Device = nullptr;
    }
    s_Application = nullptr;
    glfwTerminate();
}
```

**Impact:** Significant memory leak on application shutdown, wasted VRAM and system resources.

---

###  CRITICAL: Command Queue Memory Leak in EditorLayer8. 
**Location:** [RadianceEditor/Source/EditorMain.cpp](RadianceEditor/Source/EditorMain.cpp#L39)  
**Severity:** CRITICAL - Memory Leak

**Problem:**
```cpp
void OnAttach() override
{
    m_CommandQueue = Radiance::Application::Get().GetDevice()-> AllocatednewCommandQueue();  // 
}

void OnDetach() override
{
 Never released!}  // 
```

The Metal command queue allocated in `OnAttach()` is never released in `OnDetach()`. This is a significant GPU resource leak.

**Recommendation:**
```cpp
void OnDetach() override
{
    if (m_CommandQueue)
    {
        m_CommandQueue->release();
        m_CommandQueue = nullptr;
    }
}
```

**Impact:** GPU memory leak on layer detach/application exit.

---

###  HIGH: Double Event Polling9. 
**Location:** [Radiance/Source/Radiance/Application.cpp](Radiance/Source/Radiance/Application.cpp#L48) and [Radiance/Source/Radiance/Window.cpp](Radiance/Source/Radiance/Window.cpp#L132)  
**Severity:** HIGH - Performance/Logic Issue

**Problem:**
```cpp
// In Application::Run()
while (m_Running)
{
     First pollglfwPollEvents();  // 
    ...
}

// In Window::OnUpdate()
void Window::OnUpdate()
{
     Second poll - redundant!glfwPollEvents();  // 
}
```

Events are polled twice per frame, wasting CPU cycles and potentially causing double event processing.

**Recommendation:**
Remove the redundant `glfwPollEvents()` from `Window::OnUpdate()`. GLFW events should only be polled once per frame in the main loop.

**Impact:** Wasted CPU cycles and potential double-event processing issues.

---

###  CRITICAL: RenderPassDescriptor Memory Leak in EditorLayer10. 
**Location:** [RadianceEditor/Source/EditorMain.cpp](RadianceEditor/Source/EditorMain.cpp#L54)  
**Severity:** CRITICAL - Memory Leak

**Problem:**
```cpp
void OnUpdate(float ts) override
{
    NS::AutoreleasePool *pool = NS::AutoreleasePool::alloc()->init();

    CA::MetalDrawable* drawable = Radiance::Application::Get().GetMainWindow()-> Not releasedGetCurrentDrawable();  // 
    MTL::CommandBuffer* commandBuffer = m_CommandQueue-> Not releasedcommandBuffer();  // 

    MTL::RenderPassDescriptor *renderPassDescriptor = MTL::RenderPassDescriptor::alloc()-> Allocated but...init();  // 

    // ... setup code ...

    commandBuffer->presentDrawable(drawable);
    commandBuffer->commit();

    renderPassDescriptor-> This one is releasedrelease();  // 
    pool-> AutoreleasePool should handle drawable, but not commandBufferrelease();  // 
}
```

Issues:
- `drawable` is obtained via `nextDrawable()` which returns an autoreleased object - it should be released before the pool
- `commandBuffer` is created via `commandBuffer()` which returns an autoreleased object
- The objects are created inside the autoreleasepool, but may not be properly managed

**Recommendation:**
```cpp
void OnUpdate(float ts) override
{
    NS::AutoreleasePool *pool = NS::AutoreleasePool::alloc()->init();

    CA::MetalDrawable* drawable = Radiance::Application::Get().GetMainWindow()->GetCurrentDrawable();
    drawable->retain();  // Retain to keep alive after pool drain
    
    MTL::CommandBuffer* commandBuffer = m_CommandQueue->commandBuffer();
    commandBuffer->retain();  // Retain to keep alive after pool drain

    MTL::RenderPassDescriptor *renderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();

    // ... setup code ...

    commandBuffer->presentDrawable(drawable);
    commandBuffer->commit();

    renderPassDescriptor->release();
    drawable->release();
    commandBuffer->release();
    pool->release();
}
```

**Impact:** Memory leaks in rendering loop, accumulated over every frame.

---

###  HIGH: Thread Safety Issue with Static Application Pointer11. 
**Location:** [Radiance/Source/Radiance/Application.cpp](Radiance/Source/Radiance/Application.cpp#L7)  
**Severity:** HIGH - Race Condition

**Problem:**
```cpp
static Application *s_Application =  Not thread-safe!nullptr;  // 

Application& Application::Get()
{
    if (s_ Data race: multiple threads could check simultaneouslyApplication)  // 
        return *s_Application;
}
```

If multiple threads call `Application::Get()` concurrently:
- Two threads could both pass the nullptr check
- One thread could destroy the application while another is using it
- TOCTOU (Time-of-Check-Time-of-Use) race condition

**Recommendation:**
```cpp
#include <atomic>

static std::atomic<Application*> s_Application = nullptr;

Application& Application::Get()
{
    Application* app = s_Application.load(std::memory_order_acquire);
    if (!app)
        throw std::runtime_error("Application not initialized!");
    return *app;
}
```

**Impact:** Crashes and undefined behavior in multi-threaded scenarios.

---

###  HIGH: Metal Layer Resource Leak in Window12. 
**Location:** [Radiance/Source/Radiance/Window.cpp](Radiance/Source/Radiance/Window.cpp#L33-38) and [Radiance/Source/Radiance/Window.cpp](Radiance/Source/Radiance/Window.cpp#L122-128)  
**Severity:** HIGH - Memory Leak

**Problem:**
```cpp
void Window::Create()
{
    // ...
    m_MetalLayer = CA::MetalLayer:: Createdlayer();  // 
    m_MetalLayer->setDevice(Radiance::Application::Get().GetDevice());
    // ...
}

void Window::Destroy()
{
    if (m_WindowHandle)
        glfwDestroyWindow(m_WindowHandle);
    
    m_WindowHandle = nullptr;
 m_MetalLayer is never released!    // 
}
```

The Metal layer is created but never released when the window is destroyed.

**Recommendation:**
```cpp
void Window::Destroy()
{
    if (m_MetalLayer)
    {
        m_MetalLayer->release();
        m_MetalLayer = nullptr;
    }
    
    if (m_WindowHandle)
    {
        glfwDestroyWindow(m_WindowHandle);
        m_WindowHandle = nullptr;
    }
}
```

**Impact:** Metal layer resources leak on window destruction.

---

###  MEDIUM: Missing Exception Safety in Layer Operations13. 
**Location:** [Radiance/Source/Radiance/Application.cpp](Radiance/Source/Radiance/Application.cpp#L40-68)  
**Severity:** MEDIUM - Exception Safety

**Problem:**
```cpp
void Application::Run()
{
    Init();

    float lastTime = GetTime();

    while (m_Running)
    {
        glfwPollEvents();

        if (m_Window->ShouldClose())
        {
            Stop();
            break;
        }

        float currentTime = GetTime();
        float ts = glm::clamp(currentTime - lastTime, 0.001f, 0.1f);
        lastTime = currentTime;

        m_LayerStack.OnUpdate( Could throwts);     // 
        m_LayerStack. Could throwOnUIRender();     // 
        m_LayerStack. Could throwOnRender();       // 

        m_Window->OnUpdate();
    }

     Might not be called if exception is thrownShutdown();  // 
}
```

If any layer callback throws an exception, `Shutdown()` won't be called, leading to resource leaks.

**Recommendation:**
```cpp
void Application::Run()
{
    Init();

    float lastTime = GetTime();

    try
    {
        while (m_Running)
        {
            glfwPollEvents();

            if (m_Window->ShouldClose())
            {
                Stop();
                break;
            }

            float currentTime = GetTime();
            float ts = glm::clamp(currentTime - lastTime, 0.001f, 0.1f);
            lastTime = currentTime;

            m_LayerStack.OnUpdate(ts);
            m_LayerStack.OnUIRender();
            m_LayerStack.OnRender();

            m_Window->OnUpdate();
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception in application loop: " << e.what() << std::endl;
    }

    Shutdown();  // Always called
}
```

**Impact:** Resource leaks and improper shutdown if an exception occurs in any layer callback.

---

###  MEDIUM: Race Condition in Window Callbacks14. 
**Location:** [Radiance/Source/Radiance/Window.cpp](Radiance/Source/Radiance/Window.cpp#L40-120)  
**Severity:** MEDIUM - Race Condition

**Problem:**
```cpp
glfwSetWindowCloseCallback(m_WindowHandle, [](GLFWwindow* handle)
{
    Window& window = *((Window*)glfwGetWindowUserPointer( Race conditionhandle));  // 
    WindowClosedEvent event;
    window.RaiseEvent( Window could be destroyed by main threadevent);  // 
});
```

Scenarios:
- Main thread calls `Window::Destroy()` and sets `m_WindowHandle = nullptr`
- GLFW fires callback before `glfwDestroyWindow()` completes
- Callback tries to use a Window that's being destroyed
- Or callback fires after window is destroyed (dangling pointer)

**Recommendation:**
```cpp
void Window::Destroy()
{
    // Clear callbacks to prevent callbacks during destruction
    if (m_WindowHandle)
    {
        glfwSetWindowCloseCallback(m_WindowHandle, nullptr);
        glfwSetWindowSizeCallback(m_WindowHandle, nullptr);
        glfwSetKeyCallback(m_WindowHandle, nullptr);
        glfwSetMouseButtonCallback(m_WindowHandle, nullptr);
        glfwSetScrollCallback(m_WindowHandle, nullptr);
        glfwSetCursorPosCallback(m_WindowHandle, nullptr);
        glfwSetFramebufferSizeCallback(m_WindowHandle, nullptr);
        
        glfwDestroyWindow(m_WindowHandle);
        m_WindowHandle = nullptr;
    }
    
    if (m_MetalLayer)
    {
        m_MetalLayer->release();
        m_MetalLayer = nullptr;
    }
}
```

**Impact:** Crashes from dangling pointer access or use-after-free in window callbacks.

---

###  MEDIUM: No Validation in Application::Get() Return15. 
**Location:** [Radiance/Source/Radiance/Application.cpp](Radiance/Source/Radiance/Application.cpp#L83-87)  
**Severity:** MEDIUM - Undefined Behavior

**Problem:**
```cpp
Application& Application::Get()
{
    if (s_Application)
        return *s_Application;
 No return statement if s_Application is nullptr!    // 
    // This is undefined behavior - function reaches end without returning
}
```

If `s_Application` is nullptr, the function reaches the end without returning, causing undefined behavior.

**Recommendation:**
```cpp
Application& Application::Get()
{
    if (!s_Application)
        throw std::runtime_error("Application not initialized!");
    return *s_Application;
}
```

**Impact:** Undefined behavior, potential crash or memory corruption.

---

## Updated Summary Table

| # | Bug | File | Severity | Type |
|---|-----|------|----------|------|
| 1 | Unsafe cast in EventDispatcher | Event.h:43 | CRITICAL | Undefined Behavior |
| 2 | Null pointer in Application::Get() | Application.cpp:54 | CRITICAL | Crash |
| 3 | Memory leak in main() | EditorMain.cpp:36 | CRITICAL | Memory/Exception |
| 4 | Null pointer in callbacks | Window.cpp:23-95 | HIGH | Crash |
| 5 | Missing null check GetMousePos() | Window.cpp:113 | HIGH | Undefined Behavior |
| 6 | const correctness violation | LayerStack.h:27 | MEDIUM | Design |
| 7 | Metal device memory leak | Application.cpp:14 | CRITICAL | Memory Leak |
| 8 | Command queue memory leak | EditorMain.cpp:39 | CRITICAL | Memory Leak |
| 9 | Double event polling | Application.cpp:48, Window.cpp:132 | HIGH | Performance |
| 10 | RenderPassDescriptor leak | EditorMain.cpp:54 | CRITICAL | Memory Leak |
| 11 | Thread safety in static pointer | Application.cpp:7 | HIGH | Race Condition |
| 12 | Metal layer resource leak | Window.cpp:33-128 | HIGH | Memory Leak |
| 13 | Missing exception safety | Application.cpp:40-68 | MEDIUM | Exception Safety |
| 14 | Race condition in callbacks | Window.cpp:40-120 | MEDIUM | Race Condition |
| 15 | Missing return in Get() | Application.cpp:83-87 | MEDIUM | Undefined Behavior |

---

## Final Recommendations Priority

### Tier 1 - Critical (Fix Immediately):
- Bug #1 (EventDispatcher cast) - Can cause crashes
- Bug #2 (Application::Get() return) - Undefined behavior
- Bug #3 (Memory leak in main) - Exception safety
- Bug #4 (Null pointer in callbacks) - Crash risk
- Bug #7 (Metal device leak) - Significant resource leak
- Bug #8 (Command queue leak) - Per-frame GPU leak
- Bug #10 (RenderPass leak) - Per-frame GPU leak

### Tier 2 - High (Fix Before Release):
- Bug #5 (GetMousePos null check) - Potential crash
- Bug #9 (Double event polling) - Performance issue
- Bug #11 (Thread safety) - Concurrency issues
- Bug #12 (Metal layer leak) - Resource leak
- Bug #14 (Callback race condition) - Potential crash

### Tier 3 - Medium (Next Sprint):
- Bug #6 (const correctness) - Design issue
- Bug #13 (Exception safety) - Robustness
- Bug #15 (No return validation) - Code quality

### Testing Requirements:
1. **Memory leak testing**: Use Instruments (Metal Debugger, Leaks) to validate fixes
2. **Stress testing**: Rapid window create/destroy cycles
3. **Concurrency testing**: Multi-threaded Application::Get() access
4. **Event handling**: Validate event dispatch with all event types
5. **Rendering**: Verify frame-by-frame GPU memory doesn't grow
6. **Exception handling**: Test layer callbacks throwing exceptions
