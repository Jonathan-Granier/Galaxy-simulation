#pragma once
#include <glfw/glfw3.h>
#include "Renderer.h"
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

    GLFWwindow *m_Window = nullptr;
    std::string m_Name;
    uint32_t m_Width;
    uint32_t m_Height;

    Instance m_Instance;
    VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
    std::unique_ptr<Renderer> m_Renderer;
};