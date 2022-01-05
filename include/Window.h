#pragma once
#include <glfw/glfw3.h>
#include "Renderer.h"
#include "Camera.h"
#include "Menu.h"
#include "Vulkan/VulkanObject/ImGUI.h"
#include <memory>

/// Main window manage with GLFW.
class Window
{
public:
    /// Constructor.
    /// @param iName Window's name.
    /// @param iWidth Window's width.
    /// @param iHeight Window's heigth.
    Window(std::string iName, uint32_t iWidth, uint32_t iHeight);

    /// Destructor.
    ~Window();

    /// Run render loop.
    void Run();
    /// Resize the window.
    void Resize(uint32_t iWidth, uint32_t iHeight);
    void Scroll(float iYOffset);

private:
    /// Create glfw's surface.
    void CreateSurface();
    /// Destroy glfw's surface.
    void DestroySurface();

    void UpdateMouse();

    /// GLFW window.
    GLFWwindow *m_Window = nullptr;
    /// Window's name
    std::string m_Name;
    /// Window's width.
    uint32_t m_Width;
    /// Window's height.
    uint32_t m_Height;

    /// Vulkan instance.
    Instance m_Instance;
    /// Vullan surface.
    VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
    /// Renderer.
    std::unique_ptr<Renderer> m_Renderer;

    struct
    {
        /// Previous frame times
        std::array<float, 50> Previous{};
        /// Maximum frame time.
        float Min = 9999.0f;
        /// Minimum frame time.
        float Max = 0.0f;
        /// Current frame time.
        float Current = 0;
    } m_FrameTimes;

    /// Count the number of frame.
    uint32_t m_FrameCounter = 0;

    Camera m_Camera;
    Menu m_Menu;
    glm::vec2 m_PrevMousePos;
};