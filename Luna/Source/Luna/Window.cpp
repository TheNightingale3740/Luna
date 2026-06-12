#include "Luna/Window.h"

#include "Luna/Application.h"

#include "Luna/Events/WindowEvents.h"
#include "Luna/Events/InputEvents.h"

#include "Luna/Metal/MetalContext.h"

namespace Luna
{

    Window::Window(const WindowSpecification& spec)
        : m_Specification(spec)
    {
    }

    Window::~Window()
    {
        Destroy();
    }

    void Window::Create()
    {
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_WindowHandle = glfwCreateWindow((int)m_Specification.Width, (int)m_Specification.Height, m_Specification.Title.c_str(), nullptr, nullptr);

        glfwSetWindowUserPointer(m_WindowHandle, this);

		int width, height;
		glfwGetFramebufferSize(m_WindowHandle, &width, &height);

		m_MetalLayer = CA::MetalLayer::layer();
		m_MetalLayer->setDevice(Luna::Application::Get().GetDevice());
		//m_MetalLayer->setPixelFormat(MTL::PixelFormatBGRA8Unorm_sRGB);
		m_MetalLayer->setPixelFormat(MTL::PixelFormatBGRA8Unorm);
		m_MetalLayer->setDrawableSize(CGSizeMake(width, height));

		AttachMetalLayerToWindow(m_WindowHandle, m_MetalLayer, !(m_Specification.CustomTitlebar));

		glfwSetFramebufferSizeCallback(m_WindowHandle, [](GLFWwindow* handle, int width, int height)
		{
			Window& window = *((Window*)glfwGetWindowUserPointer(handle));	
			window.ResizeDrawable(width, height);
		});

        glfwSetWindowCloseCallback(m_WindowHandle, [](GLFWwindow* handle)
		{
			Window& window = *((Window*)glfwGetWindowUserPointer(handle));

			WindowClosedEvent event;
			window.RaiseEvent(event);
		});

		glfwSetWindowSizeCallback(m_WindowHandle, [](GLFWwindow* handle, int width, int height)
		{
			Window& window = *((Window*)glfwGetWindowUserPointer(handle));

			WindowResizeEvent event((uint32_t)width, (uint32_t)height);
			window.RaiseEvent(event);
		});

		glfwSetKeyCallback(m_WindowHandle, [](GLFWwindow* handle, int key, int scancode, int action, int mods)
		{
			Window& window = *((Window*)glfwGetWindowUserPointer(handle));

			switch (action)
			{
				case GLFW_PRESS:
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, action == GLFW_REPEAT);
					window.RaiseEvent(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					window.RaiseEvent(event);
					break;
				}
			}
		});

		glfwSetMouseButtonCallback(m_WindowHandle, [](GLFWwindow* handle, int button, int action, int mods)
		{
			Window& window = *((Window*)glfwGetWindowUserPointer(handle));

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					window.RaiseEvent(event);
					break;
				}
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					window.RaiseEvent(event);
					break;
				}
			}
		});

		glfwSetScrollCallback(m_WindowHandle, [](GLFWwindow* handle, double xOffset, double yOffset)
		{
			Window& window = *((Window*)glfwGetWindowUserPointer(handle));

			MouseScrolledEvent event(xOffset, yOffset);
			window.RaiseEvent(event);
		});

		glfwSetCursorPosCallback(m_WindowHandle, [](GLFWwindow* handle, double x, double y)
		{
			Window& window = *((Window*)glfwGetWindowUserPointer(handle));

			MouseMovedEvent event(x, y);
			window.RaiseEvent(event);
		});
    }

    void Window::Destroy()
    {
        if (m_WindowHandle)
            glfwDestroyWindow(m_WindowHandle);
        m_WindowHandle = nullptr;
		
		if (m_MetalLayer)
		{
			m_MetalLayer->release();
			m_MetalLayer = nullptr;
		}
    }

    void Window::OnUpdate()
    {
		// Do Something here if needed
    }

    void Window::RaiseEvent(Event& event)
    {
        if (m_Specification.EventCallback)
            m_Specification.EventCallback(event);
    }

	void Window::ResizeDrawable(uint32_t width, uint32_t height)
	{
		if (m_MetalLayer)
			m_MetalLayer->setDrawableSize(CGSizeMake(width, height));
	}

    glm::vec2 Window::GetMousePos() const
    {
        double x, y;
        glfwGetCursorPos(m_WindowHandle, &x, &y);
        return { static_cast<float>(x), static_cast<float>(y) };
    }

	glm::vec2 Window::GetWindowPosition() const
	{
		int xPos, yPos;
		glfwGetWindowPos(m_WindowHandle, &xPos, &yPos);

		return { xPos, yPos };
	}

	void Window::BeginWindowDrag() const
	{
		Luna::BeginWindowDrag(m_WindowHandle);
	}
	
}