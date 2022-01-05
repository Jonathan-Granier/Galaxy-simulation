#include "Window.h"
#include "Vulkan/VulkanObject/Debug.h"
#include <imgui/imgui.h>

//----------------------------------------------------------------------------------------------------------------------
static void FramebufferResizeCallback(GLFWwindow *window, int width, int height)
{
    auto app = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
    app->Resize(width, height);
}

static void ScrollCallBack(GLFWwindow *window, double xoffset, double yoffset)
{
    auto app = reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
    app->Scroll(yoffset);
}

//----------------------------------------------------------------------------------------------------------------------
Window::Window(std::string iName, uint32_t iWidth, uint32_t iHeight)
    : m_Name(iName),
      m_Width(iWidth),
      m_Height(iHeight),
      m_Menu(iWidth, iHeight)
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_Window = glfwCreateWindow(m_Width, m_Height, m_Name.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(m_Window, this);
    glfwSetFramebufferSizeCallback(m_Window, FramebufferResizeCallback);
    glfwSetScrollCallback(m_Window, ScrollCallBack);

    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    m_Instance.CreateInstance(m_Name, glfwExtensions, glfwExtensionCount);
    m_Instance.SetupDebugMessenger();

    CreateSurface();

    m_Renderer = std::make_unique<Renderer>(m_Instance, m_Surface, m_Width, m_Height);

    m_Camera.SetPerspective(45.0f, (float)m_Width / (float)m_Height, 0.1f, 1000.0f);
    m_Camera.SetPosition(glm::vec3(0.0f, 0.0f, -100.0f));
}

//----------------------------------------------------------------------------------------------------------------------
Window::~Window()
{
    if (m_Renderer)
        m_Renderer->ReleaseResources();

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
        m_FrameCounter++;
        if (m_FrameCounter >= 100)
            m_FrameCounter = 0;
        auto tStart = std::chrono::high_resolution_clock::now();
        UpdateMouse();
        m_Menu.UpdateMenu();
        UpdateParameters();

        m_Renderer->DrawNextFrame(m_Camera.GetViewMatrix(), m_Camera.GetPerspectiveMatrix());

        auto tEnd = std::chrono::high_resolution_clock::now();
        auto tDiff = std::chrono::duration<double, std::milli>(tEnd - tStart).count();
        m_FrameTimes.Current = static_cast<float>(tDiff) / 1000.0f;

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

    m_Menu.Resize(m_Width, m_Height);
    m_Renderer->RecreateSwapchainResources(m_Width, m_Height);
    m_Camera.UpdateAspectRatio(static_cast<float>(m_Width) / static_cast<float>(m_Height));
}

//----------------------------------------------------------------------------------------------------------------------
void Window::UpdateMouse()
{

    double xpos, ypos;
    glfwGetCursorPos(m_Window, &xpos, &ypos);

    bool left = glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    bool right = glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

    m_Menu.UpdateMouse(xpos, ypos, left, right);

    float dx = xpos - m_PrevMousePos.x;
    float dy = ypos - m_PrevMousePos.y;
    m_PrevMousePos = glm::vec2(xpos, ypos);

    // if (left)
    // {
    //     m_Camera.Translate(glm::vec3(dx * 0.01f, -dy * 0.01f, 0.0f));
    // }
    if (right)
    {
        m_Camera.Rotate(glm::vec3(dy * m_Camera.GetRotationSpeed(), dx * m_Camera.GetRotationSpeed(), 0.0f));
    }
}

void Window::UpdateParameters()
{
    m_Renderer->SetStep(m_Menu.GetRealTimeParameters().Step);
    m_Renderer->SetInteractionRate(m_Menu.GetRealTimeParameters().InteractionRate);
    m_Renderer->SetSmoothLenght(m_Menu.GetRealTimeParameters().SmoothingLenght);
}

//----------------------------------------------------------------------------------------------------------------------
void Window::Scroll(float iYOffset)
{
    m_Camera.Translate(glm::vec3(0.0f, 0.0f, (float)iYOffset * 1.f));
}