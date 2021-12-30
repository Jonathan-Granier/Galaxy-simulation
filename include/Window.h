#pragma once
#include <glfw/glfw3.h>
#include "Renderer.h"
#include "Vulkan/ImGUI.h"
#include <memory>

class Window
{
public:
    Window(std::string iName, uint32_t iWidth, uint32_t iHeight);
    ~Window();

    void Run();
    void Resize(uint32_t iWidth, uint32_t iHeight);

private:
    void CreateSurface();
    void DestroySurface();

    void InitImGUI();
    void UpdateImGUI();
    GLFWwindow *m_Window = nullptr;
    std::string m_Name;
    uint32_t m_Width;
    uint32_t m_Height;

    Instance m_Instance;
    VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
    std::unique_ptr<Renderer> m_Renderer;

    struct
    {
        std::array<float, 50> Previous{};
        float Min = 9999.0f, Max = 0.0f;
        float Current = 0;
    } m_FrameTimes;

    uint32_t m_FrameCounter = 0;
};