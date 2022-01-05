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
        float Step = 1.0;
        float SmoothingLenght = 0.05;
        float InteractionRate = 0.05;
    };

    Menu(uint32_t iWidth, uint32_t iHeight);
    ~Menu();
    void UpdateMenu();
    void UpdateMouse(float iXPos, float iYPos, bool iLeftClick, bool iRightClick);

    void Resize(uint32_t iWidth, uint32_t iHeight);
    const GalaxyParameters &GetGalaxyParameters() { return m_GalaxyParameters; }
    const RealTimeParameters &GetRealTimeParameters() { return m_RealTimeParameters; }

private:
    void AddTitle(const std::string &iTitle);

    GalaxyParameters m_GalaxyParameters;
    RealTimeParameters m_RealTimeParameters;
};