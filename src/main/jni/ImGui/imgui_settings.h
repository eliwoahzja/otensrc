#ifndef IMGUI_SETTINGS_H
#define IMGUI_SETTINGS_H

#include "imgui.h"
#include "../System/Texture/box_shadow.h"
#include "portfolio_theme.h"

extern float menu[4];
extern ImFont* F50;
extern ImFont* F107;

namespace font
{
    extern ImFont* inter_semibold;
}

namespace c
{
    inline float scale = portfolio::factor;
    inline float widget_scale = portfolio::factor;
    inline ImVec4 accent = portfolio::g_accent;
    inline ImVec4 separator = portfolio::separator;

    namespace bg
    {
        inline ImVec4 background = portfolio::panel;
        inline ImVec2 size = ImVec2(450, 370);
        inline float rounding = portfolio::s(portfolio::shell_round);
    }

    namespace child
    {
        inline ImVec4 background = portfolio::box;
        inline ImVec4 cap = portfolio::box;
        inline float rounding = portfolio::s(portfolio::box_round);
        inline float padding = portfolio::s(portfolio::box_pad_x);
        inline float spacing = portfolio::s(13.f);
    }

    namespace page
    {
        inline ImVec4 background_active = portfolio::control_hover;
        inline ImVec4 background = portfolio::control;

        inline ImVec4 text_hov = portfolio::text_muted;
        inline ImVec4 text = portfolio::text_muted;

        inline float rounding = portfolio::s(portfolio::control_round);
    }

    namespace elements
    {
        inline ImVec4 background_hovered = portfolio::control_hover;
        inline ImVec4 background = portfolio::control;
        inline float rounding = portfolio::s(portfolio::control_round);
    }

    namespace checkbox
    {
        inline ImVec4 mark = ImColor(0, 0, 0, 255);
        inline ImVec4 background_on = portfolio::toggle_on;
        inline ImVec4 background_off = portfolio::toggle_off;
        inline ImVec4 circle_inactive = portfolio::knob_off;
        inline float rounding = portfolio::s(portfolio::toggle_round);
    }

    namespace text
    {
        inline ImVec4 text_active = portfolio::text;
        inline ImVec4 text_hov = portfolio::text_muted;
        inline ImVec4 text = portfolio::text_muted;
    }

    namespace widget
    {
        inline ImVec2 size = ImVec2(0, portfolio::s(portfolio::control_h));
        inline ImVec4 background = portfolio::control;
        inline ImVec4 outlinecolor = portfolio::separator;
        inline float rounding = portfolio::s(portfolio::control_round);
        inline float outline = portfolio::s(1.f);
    }

    namespace button
    {
        inline ImVec4 background = portfolio::control;
        inline ImVec4 background_hovered = portfolio::control_hover;
        inline ImVec4 background_active = portfolio::control_hover;
        inline ImVec4 outline = portfolio::separator;
        inline float rounding = portfolio::s(portfolio::control_round);
    }

    namespace scrollbar
    {
        inline float hitbox_area = portfolio::s(24.f);
        inline float hitbox_extra = portfolio::s(24.f);
        inline bool left_side = false;
        inline float gutter_spacing = portfolio::s(4.f);
    }

    inline void ApplyMainWindowStyle(ImGuiStyle& style)
    {
        portfolio::apply_style();
    }

    inline float MainTopAreaHeight()
    {
        return portfolio::s(portfolio::topbar_h);
    }

    inline void UpdateTheme(bool dark_mode, const float* accent_rgba, float dt)
    {
        (void)dark_mode;
        (void)accent_rgba;
        (void)dt;
        portfolio::set_display_size(ImGui::GetIO().DisplaySize);
        portfolio::apply_scaled_style();
        scale = portfolio::factor;
        widget_scale = portfolio::factor;
        accent = portfolio::g_accent;
        separator = portfolio::separator;
        bg::background = portfolio::panel;
        child::background = portfolio::box;
        child::cap = portfolio::box;
        child::padding = portfolio::s(portfolio::box_pad_x) / scale;
        child::spacing = portfolio::s(13.f) / scale;
        page::background_active = portfolio::control_hover;
        page::background = portfolio::control;
        page::text_hov = portfolio::text_muted;
        page::text = portfolio::text_muted;
        elements::background_hovered = portfolio::control_hover;
        elements::background = portfolio::control;
        checkbox::mark = ImColor(0, 0, 0, 255);
        checkbox::background_on = portfolio::toggle_on;
        checkbox::background_off = portfolio::toggle_off;
        checkbox::circle_inactive = portfolio::knob_off;
        text::text_active = portfolio::text;
        text::text_hov = portfolio::text_muted;
        text::text = portfolio::text_muted;
        widget::background = portfolio::control;
        widget::outlinecolor = portfolio::separator;
        button::background = portfolio::control;
        button::background_hovered = portfolio::control_hover;
        button::background_active = portfolio::control_hover;
        button::outline = portfolio::separator;
    }
}

namespace main_runtime_theme
{
    inline float g_menuHue = 0.6726f;  // Indigo hue

    inline ImU32 g_accentRgbOverride = IM_COL32(0x61, 0x5D, 0xCE, 0xFF);

    inline ImVec4 GetAccentVec4(float alpha = 1.0f)
    {
        return portfolio::accent_vec4(alpha);
    }

    inline ImU32 GetAccentU32(float alpha = 1.0f)
    {
        return portfolio::accent_u32(alpha);
    }

    inline ImVec4 GetAccentTint(float strength, float alpha = 1.0f)
    {
        return portfolio::accent_vec4(strength * alpha);
    }

    inline ImU32 GetAccentTintU32(float strength, float alpha = 1.0f)
    {
        return portfolio::accent_u32(strength * alpha);
    }

    inline void ApplyAccentFromHue()
    {
        menu[0] = portfolio::g_accent.x;
        menu[1] = portfolio::g_accent.y;
        menu[2] = portfolio::g_accent.z;
        menu[3] = 1.0f;
    }

    inline float GetContentPadding()
    {
        return portfolio::s(portfolio::content_pad_x);
    }

    inline float GetColumnGap()
    {
        return portfolio::s(portfolio::column_gap);
    }

    inline float GetChildPadding()
    {
        return portfolio::s(portfolio::box_pad_x);
    }

    inline ImVec4 GetSidebarShellBackgroundColor()
    {
        return portfolio::sidebar;
    }

    inline ImVec4 GetActiveTabBackgroundColor()
    {
        return portfolio::box;
    }

    inline void ApplyThemeState()
    {
        c::scale = portfolio::factor;
        c::widget_scale = portfolio::factor;
        c::accent = portfolio::g_accent;
        c::separator = portfolio::separator;

        c::bg::background = portfolio::panel;
        c::child::background = portfolio::box;
        c::child::cap = portfolio::box;
        c::child::padding = portfolio::s(portfolio::box_pad_x) / c::scale;
        c::child::spacing = portfolio::s(13.f) / c::scale;

        c::page::background_active = portfolio::control_hover;
        c::page::background = portfolio::control;
        c::page::text_hov = portfolio::text_muted;
        c::page::text = portfolio::text_muted;

        c::elements::background_hovered = portfolio::control_hover;
        c::elements::background = portfolio::control;

        c::checkbox::mark = ImColor(0, 0, 0, 255);
        c::checkbox::background_on = portfolio::toggle_on;
        c::checkbox::background_off = portfolio::toggle_off;
        c::checkbox::circle_inactive = portfolio::knob_off;

        c::text::text_active = portfolio::text;
        c::text::text_hov = portfolio::text_muted;
        c::text::text = portfolio::text_muted;

        c::widget::background = portfolio::control;
        c::widget::outlinecolor = portfolio::separator;

        c::button::background = portfolio::control;
        c::button::background_hovered = portfolio::control_hover;
        c::button::background_active = portfolio::control_hover;
        c::button::outline = portfolio::separator;
    }
}

#endif // IMGUI_SETTINGS_H