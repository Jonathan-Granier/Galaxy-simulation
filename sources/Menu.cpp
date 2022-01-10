#include "Menu.h"
#include <imgui/imgui.h>
#include <algorithm>

//----------------------------------------------------------------------------------------------------------------------
Menu::Menu(uint32_t iWidth, uint32_t iHeight)
{
    ImGui::CreateContext();

    //  Style from this post : https://github.com/ocornut/imgui/issues/707#issuecomment-678611331
    ImGuiStyle &style = ImGui::GetStyle();
    style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_Border] = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.08f, 0.50f, 0.72f, 1.00f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.67f, 0.67f, 0.67f, 0.39f);
    style.Colors[ImGuiCol_Separator] = style.Colors[ImGuiCol_Border];
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.41f, 0.42f, 0.44f, 1.00f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.29f, 0.30f, 0.31f, 0.67f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.08f, 0.08f, 0.09f, 0.83f);
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.33f, 0.34f, 0.36f, 0.83f);
    style.Colors[ImGuiCol_TabActive] = ImVec4(0.23f, 0.23f, 0.24f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.08f, 0.08f, 0.09f, 1.00f);
    style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.11f, 0.64f, 0.92f, 1.00f);
    style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    style.GrabRounding = style.FrameRounding = 2.3f;
    // Dimensions
    ImGuiIO &io = ImGui::GetIO();
    io.FontGlobalScale = 1.0;
    io.DisplaySize = ImVec2(static_cast<float>(iWidth), static_cast<float>(iHeight));
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
    UpdateFPS();
    ImGui::NewFrame();

    if (m_Visible)
    {

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

        ImGui::NewLine();

        std::vector<bool> buttons = CenteredButtons({"Restart"}, 25.0, 20.f);
        m_Restart = buttons[0];

        m_Active = ImGui::IsWindowFocused();

        ImGui::End();

        ImGui::Begin("Frame rate (F1 to hide)");
        ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.8f);
        ImGui::PlotLines("FPS", &m_FPS[0], 50, 0, "", m_MinFPS, m_MaxFPS, ImVec2(0, 80));
        ImGui::End();
    }

    // Render to generate draw buffers
    ImGui::Render();
}

//----------------------------------------------------------------------------------------------------------------------
void Menu::UpdateMouse(double iXPos, double iYPos, bool iLeftClick, bool iRightClick)
{
    ImGuiIO &io = ImGui::GetIO();
    io.MousePos = ImVec2(static_cast<float>(iXPos), static_cast<float>(iYPos));
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
    ImGui::Text("%s", iTitle.c_str());
    ImGui::Separator();
}

//----------------------------------------------------------------------------------------------------------------------
std::vector<bool> Menu::CenteredButtons(const std::vector<std::string> iTexts, float iButtonsHeight, float iSpacesSize)
{
    float windowWidth = ImGui::GetWindowSize().x;
    float buttonsWidths = (windowWidth - ((static_cast<float>(iTexts.size()) + 2) * iSpacesSize)) / static_cast<float>(iTexts.size());
    std::vector<bool> result(iTexts.size());

    ImGui::NewLine();
    ImGui::NewLine();

    for (uint32_t i = 0; i < static_cast<uint32_t>(iTexts.size()); i++)
    {
        ImGui::SameLine();
        ImGui::SetCursorPosX(iSpacesSize + buttonsWidths * static_cast<float>(i) + iSpacesSize * static_cast<float>(i));
        ImGui::Text("%s", "");
        ImGui::SameLine();
        result[i] = ImGui::Button(iTexts[i].c_str(), ImVec2(buttonsWidths, iButtonsHeight));
    }

    return result;
}

//----------------------------------------------------------------------------------------------------------------------
void Menu::UpdateFPS()
{
    m_FrameCounter++;
    // reset every second.
    auto now = std::chrono::high_resolution_clock::now();
    float diff = static_cast<float>(std::chrono::duration<double, std::milli>(now - m_Start).count());
    if (diff > 1000)
    {
        std::rotate(m_FPS.begin(), m_FPS.begin() + 1, m_FPS.end());
        float currentFPS = static_cast<float>(m_FrameCounter) * 1000.0f / diff;
        m_FPS.back() = currentFPS;
        if (currentFPS > m_MaxFPS)
        {
            m_MaxFPS = currentFPS;
        }
        if (currentFPS < m_MinFPS)
        {
            m_MinFPS = currentFPS;
        }
        m_FrameCounter = 0;
        m_Start = now;
    }
}