#pragma once

#include <string>
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
        float Step = 0.001;
        float SmoothingLenght = 1.0f;
        float InteractionRate = 0.05f;
    };

    Menu(uint32_t iWidth, uint32_t iHeight);
    ~Menu();
    void UpdateMenu();
    void UpdateMouse(float iXPos, float iYPos, bool iLeftClick, bool iRightClick);

    void Resize(uint32_t iWidth, uint32_t iHeight);
    const GalaxyParameters &GetGalaxyParameters() { return m_GalaxyParameters; }
    const RealTimeParameters &GetRealTimeParameters() { return m_RealTimeParameters; }

    bool IsActive() { return m_Active; }
    void SetVisible(bool iIsVisible) { m_Visible = iIsVisible; }

private:
    void AddTitle(const std::string &iTitle);

    bool m_Active = false;
    bool m_Visible = true;
    GalaxyParameters m_GalaxyParameters;
    RealTimeParameters m_RealTimeParameters;
};