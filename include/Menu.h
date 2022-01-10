#pragma once

#include <string>
#include <vector>
#include <array>
#include <chrono>

class Menu
{
public:
    struct GalaxyParameters
    {
        int NbStars = 20000;
        float Diameter = 100.f;
        float Thickness = 5.f;
        float StarsSpeed = 20.f;
        float BlackHoleMass = 1000.f;
    };

    struct RealTimeParameters
    {
        float Step = 0.0001f;
        float SmoothingLenght = 1.0f;
        float InteractionRate = 0.05f;
    };

    Menu(uint32_t iWidth, uint32_t iHeight);
    ~Menu();
    void UpdateMenu();
    void UpdateMouse(double iXPos, double iYPos, bool iLeftClick, bool iRightClick);

    void Resize(uint32_t iWidth, uint32_t iHeight);
    const GalaxyParameters &GetGalaxyParameters() { return m_GalaxyParameters; }
    const RealTimeParameters &GetRealTimeParameters() { return m_RealTimeParameters; }

    bool IsActive() const { return m_Active; }
    void SetVisible(bool iIsVisible) { m_Visible = iIsVisible; }
    bool IsVisible() const { return m_Visible; }
    bool IsRestart() const { return m_Restart; }

private:
    void AddTitle(const std::string &iTitle);
    std::vector<bool> CenteredButtons(const std::vector<std::string> iTexts, float iButtonsHeight, float iSpacesSize);
    void UpdateFPS();

    bool m_Active = false;
    bool m_Visible = true;
    bool m_Restart = false;
    GalaxyParameters m_GalaxyParameters;
    RealTimeParameters m_RealTimeParameters;

    int m_FrameCounter = 0;
    std::array<float, 50> m_FPS{0};
    float m_MaxFPS = 0;
    float m_MinFPS = 9999.0f;
    std::chrono::_V2::system_clock::time_point m_Start;
};