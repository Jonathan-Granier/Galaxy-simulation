#pragma once
#include <glfw/glfw3.h>
#include "Renderer.h"
#include "Camera.h"
#include "Menu.h"
#include "Olympus/ImGUI.h"
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
    /// @param iWidth Window's width.
    /// @param iHeight Window's heigth.
    void Resize(uint32_t iWidth, uint32_t iHeight);

    /// Manage mouse scroll.
    /// @param iYOffset Scroll offset.
    void Scroll(double iYOffset);

    /// Manage Key input.
    /// @param iKey Key noticed
    /// @param iAction Key is press, repeat or release.
    void KeyInput(int iKey, int iAction);

private:
    /// Create glfw's surface.
    void CreateSurface();
    /// Destroy glfw's surface.
    void DestroySurface();

    /// Update mouse position
    void MouseInteraction();

    /// Update real time parameters.
    void UpdateParameters();

    void Restart();

    /// GLFW window.
    GLFWwindow *m_Window = nullptr;
    /// Window's name
    std::string m_Name;
    /// Window's width.
    uint32_t m_Width;
    /// Window's height.
    uint32_t m_Height;

    /// Vulkan instance.
    olp::Instance m_Instance;
    /// Vullan surface.
    VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
    /// Renderer.
    std::unique_ptr<Renderer> m_Renderer;

    Camera m_Camera;
    Menu m_Menu;
    glm::vec2 m_PrevMousePos;
};