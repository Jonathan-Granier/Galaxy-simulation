#include "Window.h"
#include "Vulkan/Debug.h"

//----------------------------------------------------------------------------------------------------------------------
static void FramebufferResizeCallback(GLFWwindow *window, int width, int height)
{
    auto app = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
    app->Resize(width, height);
}

//----------------------------------------------------------------------------------------------------------------------
Window::Window(std::string iName, uint32_t iWidth, uint32_t iHeight) : m_Name(iName),
                                                                       m_Width(iWidth),
                                                                       m_Height(iHeight)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_Window = glfwCreateWindow(m_Width, m_Height, m_Name.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(m_Window, this);
    glfwSetFramebufferSizeCallback(m_Window, FramebufferResizeCallback);

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

//----------------------------------------------------------------------------------------------------------------------
void Window::Resize(uint32_t iWidth, uint32_t iHeight)
{
    int width = iWidth;
    int height = iHeight;
    while (width == 0 || height == 0)
    {
        glfwGetFramebufferSize(m_Window, &width, &height);
        glfwWaitEvents();
    }

    m_Width = width;
    m_Height = height;
    m_Renderer->RecreateSwapchainRessources(m_Width, m_Height);
}