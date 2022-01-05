#include "Menu.h"
#include <imgui/imgui.h>

//----------------------------------------------------------------------------------------------------------------------
Menu::Menu(uint32_t iWidth, uint32_t iHeight)
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
    io.DisplaySize = ImVec2(iWidth, iHeight);
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
}

//----------------------------------------------------------------------------------------------------------------------
Menu::~Menu()
{
    if (ImGui::GetCurrentContext())
    {
        ImGui::DestroyContext();
    }
}

//----------------------------------------------------------------------------------------------------------------------
void Menu::UpdateMenu()
{
    ImGui::NewFrame();

    ImGui::Begin("Simulation settings (F1 to hide)");
    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.9f);

    AddTitle("Real time settings");

    ImGui::NewLine();

    ImGui::Text("The time step duration");
    ImGui::SliderFloat("##Step", &m_RealTimeParameters.Step, 0.0001f, 0.1f, "%.4f", ImGuiSliderFlags_Logarithmic);

    ImGui::NewLine();

    ImGui::Text("The smoothing length");
    ImGui::SliderFloat("##SmoothingLength", &m_RealTimeParameters.SmoothingLenght, 0.001f, 1.f, NULL, ImGuiSliderFlags_Logarithmic);

    ImGui::NewLine();

    ImGui::Text("The interaction rate");
    ImGui::SliderFloat("##InteractionRate", &m_RealTimeParameters.InteractionRate, 0.001f, 1.f, "%.3f", ImGuiSliderFlags_Logarithmic);

    ImGui::NewLine();

    AddTitle("Start settings");

    ImGui::NewLine();

    // Init imGui windows and elements
    ImGui::Text("The number of stars");
    ImGui::SliderInt("##NbStars", &m_GalaxyParameters.NbStars, 1000, 1000000, NULL, ImGuiSliderFlags_Logarithmic);

    ImGui::NewLine();

    ImGui::Text("The diameter of the galaxy");
    ImGui::SliderFloat("##GalaxyDiameter", &m_GalaxyParameters.Diameter, 10.f, 1000.f, "%.0f");

    ImGui::Text("The thickness of the galaxy");
    ImGui::SliderFloat("##GalaxyThickness", &m_GalaxyParameters.Thickness, 1.f, 100.f, "%.0f");

    ImGui::NewLine();

    ImGui::Text("The initial speed of the stars");
    ImGui::SliderFloat("##StarsSpeed", &m_GalaxyParameters.StarsSpeed, 0.f, 500.f, "%.1f");

    ImGui::NewLine();

    ImGui::Text("The mass of the black hole");
    ImGui::SliderFloat("##BlackHoleMass", &m_GalaxyParameters.BlackHoleMass, 100.f, 1000000.f, "%.0f", ImGuiSliderFlags_Logarithmic);

    ImGui::End();

    // Render to generate draw buffers
    ImGui::Render();
}

//----------------------------------------------------------------------------------------------------------------------
void Menu::UpdateMouse(float iXPos, float iYPos, bool iLeftClick, bool iRightClick)
{
    ImGuiIO &io = ImGui::GetIO();
    io.MousePos = ImVec2(iXPos, iYPos);
    io.MouseDown[0] = iLeftClick;
    io.MouseDown[1] = iRightClick;
}

//----------------------------------------------------------------------------------------------------------------------
void Menu::Resize(uint32_t iWidth, uint32_t iHeight)
{
    ImGuiIO &io = ImGui::GetIO();
    io.DisplaySize = ImVec2(static_cast<float>(iWidth), static_cast<float>(iHeight));
}

//----------------------------------------------------------------------------------------------------------------------
void Menu::AddTitle(const std::string &iTitle)
{
    float windowWidth = ImGui::GetWindowSize().x;
    float textWidth = ImGui::CalcTextSize(iTitle.c_str()).x;

    ImGui::NewLine();
    ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
    ImGui::Text(iTitle.c_str());
    ImGui::Separator();
}