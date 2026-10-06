#pragma once

#include "ImGui/Call_ImGui.h"
#include "../Core/SaveConfig.h"
#include "Logo.h"
#include "../ImGui/imgui_settings.h"
#include "../ImGui/custom_widgets.hpp"
#include "../ImGui/ethnir_menu.h"

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
        const std::string expiryLabel = BuildRuntimeExpiryLabel();
        ethnir::EqValueRow("License", usedKey.c_str());
        ethnir::EqValueRow("Expiry", expiryLabel.c_str());
        ethnir::EqValueRow("Subscription", userType.c_str());
    }

    // These drive the menu window itself, not the watermark: Size scales every
    // portfolio::s() metric, Opacity fades the shell's style alpha.
    inline void RenderAppearanceCard()
    {
        // Opacity only recolours, so it applies under the finger. Size moves every
        // metric the row itself is laid out with, so it is committed on release:
        // applying it mid-drag would slide the track out from under the drag.
        static float pendingSize = -1.0f;

        float opacity = main_runtime_theme::MenuOpacity();
        ethnir::RowSlider(ICON_FA_EYE, "Opacity", &opacity,
                          main_runtime_theme::kMenuOpacityMin, 1.0f, "%.2f");
        main_runtime_theme::SetMenuOpacity(opacity);

        float size = (pendingSize > 0.0f) ? pendingSize : main_runtime_theme::MenuScalePercent();
        if (ethnir::RowSlider(ICON_FA_EXPAND, "Size", &size,
                              main_runtime_theme::kMenuScaleMin, main_runtime_theme::kMenuScaleMax, "%.0f%%"))
            pendingSize = size;
        if (pendingSize > 0.0f && !ImGui::IsMouseDown(0))
        {
            main_runtime_theme::SetMenuScalePercent(pendingSize);
            pendingSize = -1.0f;
        }

        if (ethnir::EqActionRow("Reset Appearance"))
        {
            pendingSize = -1.0f;
            main_runtime_theme::SetMenuOpacity(1.0f);
            main_runtime_theme::SetMenuScalePercent(100.0f);
        }
    }

    inline void RenderConfigCard()
    {
        if (ethnir::EqActionRow("Load Config"))
            LoadConfiguration("astral_config");
        if (ethnir::EqActionRow("Save Config"))
            SaveConfiguration("astral_config");
    }

    inline void RenderEnhancementCard()
    {
        ethnir::RowToggle(ICON_FA_EYE, "Clear Display", &Config.ExtraMenu.ClearDisplay);
        if (ethnir::EqActionRow("Reset Guest"))
            Config.ExtraMenu.ResetGuest = true;
    }
}
