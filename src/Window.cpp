#include "Window.h"
#include "Vulkan/Debug.h"
#include <imgui/imgui.h>

struct UISettings
{
    bool displayModels = true;
    bool displayLogos = true;
    bool displayBackground = true;
    bool animateLight = false;
    float lightSpeed = 0.25f;
    std::array<float, 50> frameTimes{};
    float frameTimeMin = 9999.0f, frameTimeMax = 0.0f;
    float lightTimer = 0.0f;
};

static UISettings uiSettings;

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
    m_ImGUI = std::make_unique<ImGUI>(m_Renderer->GetDevice(), m_Renderer->GetBufferFactory());

    ImGuiIO &io = ImGui::GetIO();
    io.DisplaySize = ImVec2(m_Width, m_Height);
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
    m_ImGUI->CreateRessources(m_Renderer->GetRenderPass());
}

//----------------------------------------------------------------------------------------------------------------------
Window::~Window()
{
    m_Renderer->Wait();
    if (m_ImGUI)
        m_ImGUI->Destroy();

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
        UpdateImGUI();
        m_Renderer->DrawNextFrame(*m_ImGUI);
        glfwPollEvents();
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Window::UpdateImGUI()
{
    ImGuiIO &io = ImGui::GetIO();

    io.DisplaySize = ImVec2(static_cast<float>(m_Width), static_cast<float>(m_Height));
    //io.DeltaTime = frameTimer;
    double xpos, ypos;
    glfwGetCursorPos(m_Window, &xpos, &ypos);
    io.MousePos = ImVec2(xpos, ypos);
    io.MouseDown[0] = glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    io.MouseDown[1] = glfwGetMouseButton(m_Window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS;

    ImGui::NewFrame();

    // Init imGui windows and elements

    ImVec4 clear_color = ImColor(114, 144, 154);
    static float f = 0.0f;
    ImGui::TextUnformatted("Olympus");
    ImGui::TextUnformatted("TRUC");

    bool updateFrameGraph = true;
    // Update frame time display
    if (updateFrameGraph)
    {
        std::rotate(uiSettings.frameTimes.begin(), uiSettings.frameTimes.begin() + 1, uiSettings.frameTimes.end());
        float frameTime = 1000.0f / (12 * 1000.0f);
        uiSettings.frameTimes.back() = frameTime;
        if (frameTime < uiSettings.frameTimeMin)
        {
            uiSettings.frameTimeMin = frameTime;
        }
        if (frameTime > uiSettings.frameTimeMax)
        {
            uiSettings.frameTimeMax = frameTime;
        }
    }

    ImGui::PlotLines("Frame Times", &uiSettings.frameTimes[0], 50, 0, "", uiSettings.frameTimeMin, uiSettings.frameTimeMax, ImVec2(0, 80));

    ImGui::Text("Camera");
    //ImGui::InputFloat3("position", 10.0f, 2);
    //ImGui::InputFloat3("rotation", 20.0f, 2);

    ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_FirstUseEver);
    ImGui::Begin("Example settings");
    ImGui::Checkbox("Render models", &uiSettings.displayModels);
    ImGui::Checkbox("Display logos", &uiSettings.displayLogos);
    ImGui::Checkbox("Display background", &uiSettings.displayBackground);
    ImGui::Checkbox("Animate light", &uiSettings.animateLight);
    ImGui::SliderFloat("Light speed", &uiSettings.lightSpeed, 0.1f, 1.0f);
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
    //ImGui::ShowDemoWindow();

    // Render to generate draw buffers
    ImGui::Render();

    m_ImGUI->Update();
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
    m_Renderer->Wait();
    m_ImGUI->Destroy();
    m_Renderer->RecreateSwapchainRessources(m_Width, m_Height);
    m_ImGUI->CreateRessources(m_Renderer->GetRenderPass());
}