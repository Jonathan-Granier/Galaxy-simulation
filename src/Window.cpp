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
Window::Window(std::string iName, uint32_t iWidth, uint32_t iHeight) : m_Name(iName),
                                                                       m_Width(iWidth),
                                                                       m_Height(iHeight)
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
    InitImGUI();

    m_Renderer = std::make_unique<Renderer>(m_Instance, m_Surface, m_Width, m_Height);
    m_Camera.SetPerspective(45.0f, (float)m_Width / (float)m_Height, 0.1f, 1000.0f);
    m_Camera.SetPosition(glm::vec3(0.0f, 0.0f, -100.0f));
}

//----------------------------------------------------------------------------------------------------------------------
Window::~Window()
{
    if (m_Renderer)
        m_Renderer->ReleaseResources();

    if (ImGui::GetCurrentContext())
    {
        ImGui::DestroyContext();
    }

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
        UpdateImGUI();
        m_Renderer->DrawNextFrame(m_Camera.GetViewMatrix(), m_Camera.GetPerspectiveMatrix());

        auto tEnd = std::chrono::high_resolution_clock::now();
        auto tDiff = std::chrono::duration<double, std::milli>(tEnd - tStart).count();
        m_FrameTimes.Current = static_cast<float>(tDiff) / 1000.0f;

        glfwPollEvents();
    }
}
//----------------------------------------------------------------------------------------------------------------------
void Window::InitImGUI()
{
    ImGui::CreateContext();

    ImGuiStyle &style = ImGui::GetStyle();
    style.Colors[ImGuiCol_TitleBg] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.0f, 0.0f, 0.0f, 0.1f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.8f, 0.0f, 0.0f, 0.4f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.8f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(1.0f, 1.0f, 1.0f, 0.1f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(1.0f, 1.0f, 1.0f, 0.2f);
    style.Colors[ImGuiCol_Button] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(1.0f, 0.0f, 0.0f, 0.6f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);
    // Dimensions
    ImGuiIO &io = ImGui::GetIO();
    io.FontGlobalScale = 1.0;
    io.DisplaySize = ImVec2(m_Width, m_Height);
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
}
//----------------------------------------------------------------------------------------------------------------------
void Window::UpdateImGUI()
{
    ImGuiIO &io = ImGui::GetIO();

    io.DisplaySize = ImVec2(static_cast<float>(m_Width), static_cast<float>(m_Height));
    //io.DeltaTime = frameTimer;

    ImGui::NewFrame();

    // Init imGui windows and elements

    ImVec4 clear_color = ImColor(114, 144, 154);
    static float f = 0.0f;
    ImGui::TextUnformatted("Olympus");

    // Update frame time display
    if (m_FrameCounter == 0)
    {
        std::rotate(m_FrameTimes.Previous.begin(), m_FrameTimes.Previous.begin() + 1, m_FrameTimes.Previous.end());
        float frameTime = 1000.0f / (m_FrameTimes.Current * 1000.0f);
        m_FrameTimes.Previous.back() = frameTime;
        if (frameTime < m_FrameTimes.Min)
        {
            m_FrameTimes.Min = frameTime;
        }
        if (frameTime > m_FrameTimes.Max)
        {
            m_FrameTimes.Max = frameTime;
        }
    }

    ImGui::PlotLines("Frame Times", &m_FrameTimes.Previous[0], 50, 0, "", m_FrameTimes.Min, m_FrameTimes.Max, ImVec2(0, 80));

    // Render to generate draw buffers
    ImGui::Render();
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
    m_Renderer->RecreateSwapchainResources(m_Width, m_Height);
    m_Camera.UpdateAspectRatio(static_cast<float>(m_Width) / static_cast<float>(m_Height));
}

//----------------------------------------------------------------------------------------------------------------------
void Window::UpdateMouse()
{

    double xpos, ypos;
    glfwGetCursorPos(m_Window, &xpos, &ypos);

    ImGuiIO &io = ImGui::GetIO();
    io.MousePos = ImVec2(xpos, ypos);
    io.MouseDown[0] = glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    io.MouseDown[1] = glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

    float dx = xpos - m_PrevMousePos.x;
    float dy = ypos - m_PrevMousePos.y;
    m_PrevMousePos = glm::vec2(xpos, ypos);

    // Left
    if (io.MouseDown[0])
    {
        m_Camera.Translate(glm::vec3(dx * 0.01f, -dy * 0.01f, 0.0f));
    }
    else if (io.MouseDown[1])
    {
        m_Camera.Rotate(glm::vec3(dy * m_Camera.GetRotationSpeed(), dx * m_Camera.GetRotationSpeed(), 0.0f));
    }
}

void Window::Scroll(float iYOffset)
{
    m_Camera.Translate(glm::vec3(0.0f, 0.0f, (float)iYOffset * 1.0f));
}