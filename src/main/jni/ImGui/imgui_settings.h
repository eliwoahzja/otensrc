#ifndef IMGUI_SETTINGS_H
#define IMGUI_SETTINGS_H

#include "imgui.h"
#include "../System/Texture/box_shadow.h"

extern float menu[4];
extern ImFont* F50;
extern ImFont* F107;

namespace font
{
    extern ImFont* inter_semibold;
}

namespace c
{
    inline float scale = 1.5f;
    inline float widget_scale = 1.0f;
    inline ImVec4 accent = ImColor(118, 187, 117);
    inline ImVec4 separator = ImColor(22, 23, 26);

    namespace bg
    {
        inline ImVec4 background = ImColor(15, 15, 15);
        inline ImVec2 size = ImVec2(450, 370);
        inline float rounding = 8.f;
    }

    namespace child
    {
        inline ImVec4 background = ImColor(17, 17, 18);
        inline ImVec4 cap = ImColor(20, 21, 23);
        inline float rounding = 8.f;
        inline float padding = 13.f;
        inline float spacing = 13.f;
    }

    namespace page
    {
        inline ImVec4 background_active = ImColor(31, 33, 38);
        inline ImVec4 background = ImColor(22, 23, 25);

        inline ImVec4 text_hov = ImColor(69, 74, 95);
        inline ImVec4 text = ImColor(68, 71, 85);

        inline float rounding = 4.f;
    }

    namespace elements
    {
        inline ImVec4 background_hovered = ImColor(31, 33, 38);
        inline ImVec4 background = ImColor(22, 23, 25);
        inline float rounding = 2.f;
    }

    namespace checkbox
    {
        inline ImVec4 mark = ImColor(0, 0, 0, 255);
        inline ImVec4 background_on = ImColor(79, 134, 247);
        inline ImVec4 background_off = ImColor(40, 42, 48);
        inline ImVec4 circle_inactive = ImColor(80, 84, 96);
        inline float rounding = 4.f;
    }

    namespace text
    {
        inline ImVec4 text_active = ImColor(255, 255, 255);
        inline ImVec4 text_hov = ImColor(69, 74, 95);
        inline ImVec4 text = ImColor(68, 71, 85);
    }

    namespace widget
    {
        inline ImVec2 size = ImVec2(0, 34.f);
        inline ImVec4 background = ImColor(22, 23, 25);
        inline ImVec4 outlinecolor = ImColor(30, 32, 36);
        inline float rounding = 4.f;
        inline float outline = 1.f;
    }

    namespace button
    {
        inline ImVec4 background = ImColor(22, 23, 25);
        inline ImVec4 background_hovered = ImColor(31, 33, 38);
        inline ImVec4 background_active = ImColor(40, 42, 48);
        inline ImVec4 outline = ImColor(50, 52, 56);
        inline float rounding = 4.f;
    }

    namespace scrollbar
    {
        // Extra invisible hitbox size (per side) for easier dragging on touch devices.
        inline float hitbox_area = 24.f;
        // Backward compatibility for older references.
        inline float hitbox_extra = 24.f;
        inline bool left_side = false;
        inline float gutter_spacing = 4.f;
    }

    inline void ApplyMainWindowStyle(ImGuiStyle& style)
    {
        style.WindowPadding = ImVec2(0.0f, 0.0f);
        style.ItemSpacing = ImVec2(10.0f * scale, 10.0f * scale);
        style.WindowBorderSize = 0.0f;
        style.ScrollbarSize = 8.0f * scale;
    }

    inline float MainTopAreaHeight()
    {
        return 40.0f * scale;
    }

    inline void UpdateTheme(bool dark_mode, const float* accent_rgba, float dt)
    {
        bg::background = ImLerp(bg::background, dark_mode ? ImColor(15, 15, 15) : ImColor(255, 255, 255), dt * 12.0f);
        separator = ImLerp(separator, dark_mode ? ImColor(22, 23, 26) : ImColor(222, 228, 244), dt * 12.0f);

        const ImVec4 accent_target = dark_mode
            ? (accent_rgba ? ImVec4(accent_rgba[0], accent_rgba[1], accent_rgba[2], 1.0f) : ImColor(118, 187, 117).Value)
            : ImColor(121, 131, 207).Value;
        accent = ImLerp(accent, accent_target, dt * 12.0f);

        elements::background_hovered = ImLerp(elements::background_hovered, dark_mode ? ImColor(31, 33, 38) : ImColor(197, 207, 232), dt * 25.0f);
        elements::background = ImLerp(elements::background, dark_mode ? ImColor(22, 23, 25) : ImColor(222, 228, 244), dt * 25.0f);

        widget::background = ImLerp(widget::background, dark_mode ? ImColor(22, 23, 25) : ImColor(236, 240, 250), dt * 25.0f);
        widget::outlinecolor = ImLerp(widget::outlinecolor, dark_mode ? ImColor(30, 32, 36) : ImColor(194, 204, 228), dt * 25.0f);
        button::background = ImLerp(button::background, dark_mode ? ImColor(22, 23, 25) : ImColor(236, 240, 250), dt * 25.0f);
        button::background_hovered = ImLerp(button::background_hovered, dark_mode ? ImColor(31, 33, 38) : ImColor(213, 222, 242), dt * 25.0f);
        button::background_active = ImLerp(button::background_active, dark_mode ? ImColor(40, 42, 48) : ImColor(196, 206, 232), dt * 25.0f);
        button::outline = ImLerp(button::outline, dark_mode ? ImColor(50, 52, 56) : ImColor(177, 188, 217), dt * 25.0f);

        checkbox::mark = ImLerp(checkbox::mark, dark_mode ? ImColor(0, 0, 0) : ImColor(255, 255, 255), dt * 12.0f);
        checkbox::background_off = ImLerp(checkbox::background_off, dark_mode ? ImColor(40, 42, 48) : ImColor(205, 214, 236), dt * 25.0f);
        checkbox::circle_inactive = ImLerp(checkbox::circle_inactive, dark_mode ? ImColor(80, 84, 96) : ImColor(120, 130, 158), dt * 25.0f);

        child::background = ImLerp(child::background, dark_mode ? ImColor(17, 17, 18) : ImColor(241, 243, 249), dt * 12.0f);
        child::cap = ImLerp(child::cap, dark_mode ? ImColor(20, 21, 23) : ImColor(228, 235, 248), dt * 12.0f);
        child::padding = 13.0f;
        child::spacing = 13.0f;

        page::text_hov = ImLerp(page::text_hov, dark_mode ? ImColor(68, 71, 85) : ImColor(136, 145, 176), dt * 12.0f);
        page::text = ImLerp(page::text, dark_mode ? ImColor(68, 71, 85) : ImColor(136, 145, 176), dt * 12.0f);
        page::background_active = ImLerp(page::background_active, dark_mode ? ImColor(31, 33, 38) : ImColor(196, 205, 228), dt * 25.0f);
        page::background = ImLerp(page::background, dark_mode ? ImColor(22, 23, 25) : ImColor(222, 228, 244), dt * 25.0f);

        text::text_active = ImLerp(text::text_active, dark_mode ? ImColor(255, 255, 255) : ImColor(0, 0, 0), dt * 12.0f);
        text::text_hov = ImLerp(text::text_hov, dark_mode ? ImColor(68, 71, 85) : ImColor(68, 71, 81), dt * 12.0f);
        text::text = ImLerp(text::text, dark_mode ? ImColor(68, 71, 85) : ImColor(68, 71, 81), dt * 12.0f);
    }

    inline void ApplyTheme()
    {
        accent = ImColor(188, 110, 255);
        separator = ImColor(52, 28, 78);

        bg::background = ImColor(4, 4, 6, 176);
        child::background = ImColor(14, 0, 24, 200);
        child::cap = ImColor(0, 0, 0, 124);
        child::padding = 13.0f;
        child::spacing = 13.0f;

        page::background_active = ImColor(73, 33, 121, 255);
        page::background = ImColor(0, 0, 0, 150);
        page::text_hov = ImColor(231, 214, 255);
        page::text = ImColor(154, 123, 201);

        elements::background_hovered = ImColor(12, 12, 14, 184);
        elements::background = ImColor(0, 0, 0, 156);

        checkbox::mark = ImColor(255, 246, 255);
        checkbox::background_on = ImColor(188, 110, 255);
        checkbox::background_off = ImColor(65, 49, 92);
        checkbox::circle_inactive = ImColor(155, 134, 198);

        text::text_active = ImColor(251, 245, 255);
        text::text_hov = ImColor(194, 168, 234);
        text::text = ImColor(136, 112, 176);

        widget::background = ImColor(0, 0, 0, 146);
        widget::outlinecolor = ImColor(90, 52, 136, 140);

        button::background = ImColor(0, 0, 0, 152);
        button::background_hovered = ImColor(14, 14, 16, 178);
        button::background_active = ImColor(22, 22, 28, 198);
        button::outline = ImColor(109, 67, 158, 150);

        ImGuiStyle& style = ImGui::GetStyle();
        style.ScrollbarSize = 5.0f;
        style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.06f, 0.03f, 0.10f, 0.72f);
        style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.16f, 0.46f, 0.95f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.42f, 0.22f, 0.62f, 0.98f);
        style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.28f, 0.76f, 1.0f);
    }

    inline void DrawWindowShadow(const ImVec2& menuSize)
    {
        RectangleShadowSettings shadowSettings;
        shadowSettings.rectPos = ImVec2(0.0f, 0.0f);
        shadowSettings.rectSize = menuSize;
        shadowSettings.sigma = 15.0f;
        shadowSettings.padding = ImVec2(0.0f, 0.0f);
        shadowSettings.rings = 6;
        shadowSettings.spacingBetweenRings = 2;
        shadowSettings.samplesPerCornerSide = 2;
        shadowSettings.shadowColor = ImGui::ColorConvertU32ToFloat4(IM_COL32(70, 22, 108, 235));
        shadowSettings.shadowSize = ImVec2(0.0f, 0.0f);
        drawRectangleShadowVerticesAdaptive(shadowSettings);
    }

    inline void DrawMenuBackdrop(ImDrawList* drawList, const ImVec2& min, const ImVec2& max, float rounding, ImTextureID backgroundTexture = nullptr)
    {
        if (drawList == nullptr) {
            return;
        }

        if (backgroundTexture != nullptr) {
            drawList->AddImageRounded(
                backgroundTexture,
                min,
                max,
                ImVec2(0.0f, 0.0f),
                ImVec2(1.0f, 1.0f),
                IM_COL32(255, 255, 255, 255),
                rounding
            );
        } else {
            drawList->AddRectFilled(min, max, IM_COL32(12, 6, 24, 255), rounding);
        }

        drawList->AddRectFilled(min, max, IM_COL32(10, 0, 18, 70), rounding);
        drawList->AddRect(min, max, IM_COL32(188, 110, 255, 156), rounding, 0, 1.0f);
    }
}

namespace main_runtime_theme
{
    inline float g_menuHue = 0.78f;

    inline ImVec4 GetAccentVec4(float alpha = 1.0f)
    {
        return ImVec4(menu[0], menu[1], menu[2], alpha);
    }

    inline ImU32 GetAccentU32(float alpha = 1.0f)
    {
        return ImGui::ColorConvertFloat4ToU32(GetAccentVec4(alpha));
    }

    inline ImVec4 GetAccentTint(float strength, float alpha = 1.0f)
    {
        return ImVec4(menu[0] * strength, menu[1] * strength, menu[2] * strength, alpha);
    }

    inline ImU32 GetAccentTintU32(float strength, float alpha = 1.0f)
    {
        return ImGui::ColorConvertFloat4ToU32(GetAccentTint(strength, alpha));
    }

    inline void ApplyAccentFromHue()
    {
        ImGui::ColorConvertHSVtoRGB(g_menuHue, 0.78f, 1.0f, menu[0], menu[1], menu[2]);
        menu[3] = 1.0f;
    }

    inline float GetContentPadding()
    {
        return 10.0f;
    }

    inline float GetColumnGap()
    {
        return 10.0f;
    }

    inline float GetChildPadding()
    {
        return 10.0f;
    }

    inline ImVec4 GetSidebarShellBackgroundColor()
    {
        return ImColor(0, 0, 0, 130);
    }

    inline ImVec4 GetActiveTabBackgroundColor()
    {
        return ImColor(0, 0, 0, 150);
    }

    inline void ApplyThemeState()
    {
        c::scale = 1.15f;
        c::widget_scale = 1.45f;
        const float childPadding = GetChildPadding();
        c::accent = ImColor(GetAccentVec4());
        c::separator = ImColor(0.0f, 0.0f, 0.0f, 0.0f);

        c::bg::background = ImColor(0.0f, 0.0f, 0.0f, 0.50f);
        c::child::background = GetActiveTabBackgroundColor();
        c::child::cap = GetActiveTabBackgroundColor();
        c::child::padding = childPadding / c::scale;
        c::child::spacing = childPadding / c::scale;

        c::page::background_active = ImColor(GetAccentTint(0.18f, 0.48f));
        c::page::background = ImColor(0.02f, 0.02f, 0.03f, 0.52f);
        c::page::text_hov = ImColor(0.94f, 0.95f, 0.98f, 1.0f);
        c::page::text = ImColor(0.62f, 0.65f, 0.75f, 0.96f);

        c::elements::background_hovered = ImColor(0.08f, 0.08f, 0.10f, 0.86f);
        c::elements::background = ImColor(0.04f, 0.04f, 0.05f, 0.68f);

        c::checkbox::mark = ImColor(0.98f, 0.98f, 1.0f, 1.0f);
        c::checkbox::background_on = ImColor(GetAccentTint(0.92f, 0.96f));
        c::checkbox::background_off = ImColor(0.14f, 0.14f, 0.17f, 0.94f);
        c::checkbox::circle_inactive = ImColor(0.46f, 0.48f, 0.56f, 0.94f);

        c::text::text_active = ImColor(0.97f, 0.97f, 0.99f, 1.0f);
        c::text::text_hov = ImColor(GetAccentTint(0.95f, 0.96f));
        c::text::text = ImColor(0.71f, 0.74f, 0.82f, 0.95f);

        c::widget::background = ImColor(0.02f, 0.02f, 0.03f, 0.74f);
        c::widget::outlinecolor = ImColor(0.24f, 0.25f, 0.30f, 0.72f);

        c::button::background = ImColor(0.03f, 0.03f, 0.04f, 0.78f);
        c::button::background_hovered = ImColor(0.07f, 0.07f, 0.09f, 0.86f);
        c::button::background_active = ImColor(0.10f, 0.10f, 0.12f, 0.92f);
        c::button::outline = ImColor(GetAccentTint(0.62f, 0.52f));
    }
}

#endif // IMGUI_SETTINGS_H
