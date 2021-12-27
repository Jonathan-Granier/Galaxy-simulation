#include "Window.h"
#include "Vulkan/Debug.h"

//----------------------------------------------------------------------------------------------------------------------
Window::Window(std::string iName, uint32_t iWidth, uint32_t iHeight) : m_Name(iName),
                                                                       m_Width(iWidth),
                                                                       m_Height(iHeight)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_Window = glfwCreateWindow(m_Width, m_Height, m_Name.c_str(), nullptr, nullptr);
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    m_Instance.CreateInstance(m_Name, glfwExtensions, glfwExtensionCount);
    m_Instance.SetupDebugMessenger();

    CreateSurface();

    m_Renderer = std::make_unique<Renderer>(m_Instance, m_Surface, m_Width, m_Height);
}

//----------------------------------------------------------------------------------------------------------------------
Window::~Window()
{
    if (m_Renderer)
        m_Renderer->ReleaseRessources();

    DestroySurface();
    m_Instance.Destroy();
    glfwDestroyWindow(m_Window);
    glfwTerminate();
}

//----------------------------------------------------------------------------------------------------------------------
void Window::Run()
{
    while (!glfwWindowShouldClose(m_Window))
    {
        glfwPollEvents();
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Window::CreateSurface()
{
    VK_CHECK_RESULT(glfwCreateWindowSurface(m_Instance.GetVkInstance(), m_Window, nullptr, &m_Surface));
}

//----------------------------------------------------------------------------------------------------------------------
void Window::DestroySurface()
{
    vkDestroySurfaceKHR(m_Instance.GetVkInstance(), m_Surface, nullptr);
}
