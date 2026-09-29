#pragma once

#include "ImGui/Call_ImGui.h"
#include "../Core/SaveConfig.h"
#include "Logo.h"
#include "../ImGui/imgui_settings.h"
#include "../ImGui/custom_widgets.hpp"

extern ImFont* F50;
extern ImFont* F48;
extern ImFont* JAAT;
extern float menu[4];
extern std::string usedKey;
extern std::string EXP;
extern std::string userType;

void RenderLicenseInfo(ImDrawList* draw, const ImVec2& startPos)
{
    const ImVec4 titleColor = c::text::text_active;
    const ImVec4 valueColor = c::text::text;

    ImGui::SetCursorPosX(15);
    ImGui::PushFont(F50);
    ImGui::SetWindowFontScale(0.6f);
    ImGui::TextColored(titleColor, "License");
    ImGui::SetWindowFontScale(1.0f);
    ImGui::PopFont();
    
    if (!usedKey.empty())
    {
        ImGui::SameLine(0, 15.0f);
        ImGui::TextColored(valueColor, ": %s", usedKey.c_str());
    }
    
    custom::Separator_line();
    
    
    std::string countdownText = getExpiryCountdown();
    
    ImGui::SetCursorPosX(15);
    ImGui::PushFont(F50);
    ImGui::SetWindowFontScale(0.6f);
    ImGui::TextColored(titleColor, "Expiry");
    ImGui::SetWindowFontScale(1.0f);
    ImGui::PopFont();
    
    if (!EXP.empty())
    {
        ImGui::SameLine(0, 15.0f);
        ImGui::TextColored(valueColor, ": %s", EXP.c_str());
        
        if (!countdownText.empty())
        {
            ImGui::SetCursorPosX(15);
            ImGui::TextColored(valueColor, "                 (%s)", countdownText.c_str());
        }
    }
    
    custom::Separator_line();
    
    
    ImGui::SetCursorPosX(15);
    ImGui::PushFont(F50);
    ImGui::SetWindowFontScale(0.6f);
    ImGui::TextColored(titleColor, "Subscription");
    ImGui::SetWindowFontScale(1.0f);
    ImGui::PopFont();
    
    if (!userType.empty())
    {
        ImGui::SameLine(0, 15.0f);
        if (userType == "Premium")
        {
            ImGui::TextColored(ImColor(191, 153, 0, 165), ": %s", userType.c_str());
        }
        else
        {
            ImGui::TextColored(ImColor(0, 204, 0, 165), ": %s", userType.c_str());
        }
    }
}

void RenderLogoSettings(ImDrawList* draw)
{
    float tempOpacity = GetLogoOpacity();
    float tempSize = GetLogoSizeMultiplier();
    
    custom::SliderFloat("Opacity", &tempOpacity, 0.0f, 1.0f, "%.2f");
    SetLogoOpacity(tempOpacity);

    custom::SliderFloat("Size", &tempSize, 0.1f, 2.0f, "%.2f");
    SetLogoSizeMultiplier(tempSize);

    const float buttonHeight = 50.0f;
    const float buttonInsetX = 10.0f;
    ImGui::SetCursorPosX(buttonInsetX);
    ImVec2 buttonSize = ImVec2(ImMax(1.0f, ImGui::GetContentRegionAvail().x - buttonInsetX), buttonHeight);
    
    ImGui::PushStyleColor(ImGuiCol_Button, c::button::background_hovered);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, c::button::background_active);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, c::button::background_active);
    ImGui::PushStyleColor(ImGuiCol_Border, c::button::outline);
    ImGui::PushStyleColor(ImGuiCol_Text, c::text::text_active);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, c::button::rounding);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    
    if (ImGui::Button("RESET LOGO", buttonSize))
    {
        SetLogoOpacity(1.0f);
        SetLogoSizeMultiplier(1.0f);
    }
    
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(5);
}

void RenderConfigManagement(ImDrawList* draw)
{
    const float buttonHeight = 55.0f;
    const float buttonInsetX = 10.0f;
    ImGui::SetCursorPosX(buttonInsetX);
    ImVec2 buttonSize = ImVec2(ImMax(1.0f, ImGui::GetContentRegionAvail().x - buttonInsetX), buttonHeight);
    
    ImGui::PushStyleColor(ImGuiCol_Button, c::widget::background);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, c::button::background_active);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, c::button::background_active);
    ImGui::PushStyleColor(ImGuiCol_Border, c::button::outline);
    ImGui::PushStyleColor(ImGuiCol_Text, c::text::text_active);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, c::button::rounding);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    
    if (ImGui::Button("LOAD CONFIG", buttonSize))
    {
        LoadConfiguration("astral_config");
    }
    
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(5);

    ImGui::SetCursorPosX(buttonInsetX);
    buttonSize = ImVec2(ImMax(1.0f, ImGui::GetContentRegionAvail().x - buttonInsetX), buttonHeight);
    
    ImGui::PushStyleColor(ImGuiCol_Button, c::widget::background);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, c::button::background_active);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, c::button::background_active);
    ImGui::PushStyleColor(ImGuiCol_Border, c::button::outline);
    ImGui::PushStyleColor(ImGuiCol_Text, c::text::text_active);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, c::button::rounding);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
    
    if (ImGui::Button("SAVE CONFIG", buttonSize))
    {
        SaveConfiguration("astral_config");
    }
    
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(5);
}

void RenderEnhancement()
{
    custom::Checkbox("Clear Display", &Config.ExtraMenu.ClearDisplay);
    custom::Separator_line();
    custom::Checkbox("Reset Guest", &Config.ExtraMenu.ResetGuest);
    custom::Separator_line();

    const float row_x = ImGui::GetCursorPosX();
    const float row_w = ImGui::GetContentRegionAvail().x;
    const float preview_w = ImGui::GetFrameHeight() * c::scale;

    ImGui::AlignTextToFramePadding();
    ImGui::TextUnformatted("Menu Accent");
    ImGui::SameLine();
    ImGui::SetCursorPosX(row_x + row_w - preview_w);
    ImGui::SetNextItemWidth(preview_w);
    custom::ColorEdit4(
        "##menu_accent_preview",
        menu,
        ImGuiColorEditFlags_NoAlpha |
        ImGuiColorEditFlags_NoInputs |
        ImGuiColorEditFlags_NoLabel |
        ImGuiColorEditFlags_PickerHueWheel
    );

    menu[3] = 1.0f;
}

namespace settings_tab
{
    inline void ContentGap(float height = 6.0f)
    {
        ImGui::Dummy(ImVec2(0.0f, height));
    }

    inline std::string BuildRuntimeExpiryLabel()
    {
        std::string label = EXP;
        const std::string countdown = getExpiryCountdown();
        if (!countdown.empty())
        {
            if (!label.empty())
                label += " ";
            label += "(" + countdown + ")";
        }
        return label;
    }

    inline void RenderLicenseCard()
    {
        const float sidePad = 15.0f;
        auto drawLine = [sidePad](const char *name, const char *value, const ImVec4 &valueColor) {
            ImGui::SetCursorPosX(sidePad);
            ImGui::TextColored(c::text::text_active, "%s", name);
            ImGui::SameLine(0.0f, 15.0f);
            ImGui::TextColored(valueColor, ": %s", value ? value : "");
            ContentGap();
        };

        const std::string expiryLabel = BuildRuntimeExpiryLabel();
        const ImVec4 subColor = (userType == "Premium")
            ? ImVec4(0.75f, 0.60f, 0.00f, 0.86f)
            : ImVec4(0.00f, 0.80f, 0.00f, 0.86f);

        drawLine("License", usedKey.c_str(), c::text::text);
        drawLine("Expiry", expiryLabel.c_str(), c::text::text);
        drawLine("Subscription", userType.c_str(), subColor);
    }

    inline void RenderLogoCard()
    {
        float opacity = GetLogoOpacity();
        float size = GetLogoSizeMultiplier();
        custom::SliderFloat("Opacity", &opacity, 0.0f, 1.0f, "%.2f");
        custom::SliderFloat("Size", &size, 0.1f, 2.0f, "%.2f");
        SetLogoOpacity(opacity);
        SetLogoSizeMultiplier(size);
        if (custom::Button("RESET LOGO", ImVec2(ImMax(1.0f, ImGui::GetContentRegionAvail().x), 50.0f)))
        {
            SetLogoOpacity(1.0f);
            SetLogoSizeMultiplier(1.0f);
        }
    }

    inline void RenderConfigCard()
    {
        if (custom::Button("LOAD CONFIG", ImVec2(ImMax(1.0f, ImGui::GetContentRegionAvail().x), 54.0f)))
            LoadConfiguration("astral_config");
        ContentGap(4.0f);
        if (custom::Button("SAVE CONFIG", ImVec2(ImMax(1.0f, ImGui::GetContentRegionAvail().x), 54.0f)))
            SaveConfiguration("astral_config");
    }

    inline void RenderEnhancementCard()
    {
        custom::Checkbox("Clear Display", &Config.ExtraMenu.ClearDisplay);
        if (custom::Button("RESET GUEST", ImVec2(ImMax(1.0f, ImGui::GetContentRegionAvail().x), 50.0f)))
            Config.ExtraMenu.ResetGuest = true;
    }
}
