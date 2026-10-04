#pragma once
#include "imgui.h"
#include "imgui_internal.h"

namespace portfolio
{
    // ============ ACCENT ============
    inline ImVec4 g_accent = { 0x61/255.f, 0x5D/255.f, 0xCE/255.f, 1.f };  // #615DCE

    // ============ NIGHT PALETTE (DEFAULT) ============
    inline ImVec4 bg            = { 0.f, 0.f, 0.f, 0.50f };
    inline ImVec4 panel         = { 0.f, 0.f, 0.f, 0.50f };
    inline ImVec4 sidebar       = { 0.f, 0.f, 0.f, 0.40f };
    inline ImVec4 box           = { 0.f, 0.f, 0.f, 0.40f };
    inline ImVec4 control       = { 0.f, 0.f, 0.f, 0.50f };
    inline ImVec4 control_hover = { 0.f, 0.f, 0.f, 0.60f };
    inline ImVec4 circle_checkbox_hover = { 0x86/255.f, 0x86/255.f, 0x86/255.f, 1.f };
    inline ImVec4 text          = { 1.f, 1.f, 1.f, 1.f };
    inline ImVec4 text_muted    = { 1.f, 1.f, 1.f, 0.60f };
    inline ImVec4 header_text   = { 1.f, 1.f, 1.f, 0.26f };
    inline ImVec4 separator     = { 0.f, 0.f, 0.f, 0.25f };
    inline ImVec4 sidebar_sep   = { 1.f, 1.f, 1.f, 0.17f };
    inline ImVec4 dropdown_bg   = { 0.f, 0.f, 0.f, 0.70f };
    inline ImVec4 config_sel    = { 0x25/255.f, 0x25/255.f, 0x25/255.f, 0.50f };
    inline ImVec4 config_hover  = { 0.f, 0.f, 0.f, 0.50f };
    inline ImVec4 slider_bg     = { 0x2B/255.f, 0x2B/255.f, 0x2B/255.f, 1.f };
    inline ImVec4 slider_fill   = { 0x86/255.f, 0x86/255.f, 0x86/255.f, 1.f };
    inline ImVec4 toggle_off    = { 0x1A/255.f, 0x1A/255.f, 0x1A/255.f, 1.f };
    inline ImVec4 toggle_on     = { 0x86/255.f, 0x86/255.f, 0x86/255.f, 1.f };
    inline ImVec4 knob_off      = { 0x6E/255.f, 0x6E/255.f, 0x6E/255.f, 1.f };
    inline ImVec4 success       = { 0.35f, 0.78f, 0.48f, 1.f };
    inline ImVec4 danger        = { 1.f, 0x70/255.f, 0x70/255.f, 1.f };

    inline ImVec4 ink  = { 1.f, 1.f, 1.f, 1.f };
    inline ImVec4 haze = { 1.f, 1.f, 1.f, 1.f };

    inline ImVec4 fg(float alpha)   { return { ink.x,  ink.y,  ink.z,  alpha }; }
    inline ImVec4 wash(float alpha) { return { haze.x, haze.y, haze.z, alpha }; }

    // ============ LAYOUT CONSTANTS (design px) ============
    inline constexpr float window_w         = 1160.f;
    inline constexpr float window_h         = 669.f;
    inline constexpr float shell_round      = 14.f;
    inline constexpr float sidebar_w        = 243.f;
    inline constexpr float sidebar_pad      = 24.f;
    inline constexpr float sidebar_tab_h    = 51.f;
    inline constexpr float sidebar_tab_w    = 241.f;
    inline constexpr float sidebar_tab_icon = 24.f;
    inline constexpr float sidebar_tabs_y   = 87.f;
    inline constexpr float sidebar_tab_icon_x = 48.f;
    inline constexpr float sidebar_tab_text_gap = 10.f;
    inline constexpr float sidebar_logo_size  = 60.f;
    inline constexpr float sidebar_logo_gap   = 16.f;
    inline constexpr float sidebar_logo_x     = 8.f;
    inline constexpr float sidebar_logo_y     = 22.f;
    inline constexpr float sidebar_logo_font  = 36.f;
    inline constexpr float topbar_h           = 88.f;
    inline constexpr float topbar_row_y       = 21.f;
    inline constexpr float topbar_row_h       = 49.f;
    inline constexpr float topbar_save_w      = 138.f;
    inline constexpr float topbar_search_w    = 434.f;
    inline constexpr float topbar_icon_size   = 24.f;
    inline constexpr float topbar_gear_margin = 30.f;
    inline constexpr float content_pad_x      = 19.f;
    inline constexpr float column_w           = 434.f;
    inline constexpr float column_gap         = 16.f;
    inline constexpr float settings_row_h     = 37.f;
    inline constexpr float separator_h        = 1.f;
    inline constexpr float row_separator_gap  = 5.f;
    inline constexpr float box_pad_y          = 8.f;
    inline constexpr float box_pad_x          = 14.f;
    inline constexpr float box_round          = 6.f;
    inline constexpr float control_h          = 24.f;
    inline constexpr float toggle_w           = 42.f;
    inline constexpr float toggle_h           = 22.f;
    inline constexpr float toggle_round       = 11.f;
    inline constexpr float toggle_knob_r      = 8.f;
    inline constexpr float toggle_knob_travel = 20.f;
    inline constexpr float toggle_knob_inset  = 11.f;
    inline constexpr float control_round      = 4.f;
    inline constexpr float slider_h           = 6.f;
    inline constexpr float slider_track_h     = 2.f;
    inline constexpr float slider_value_w     = 55.f;
    inline constexpr float slider_w           = 143.f;
    inline constexpr float compact_slider_w   = 87.f;
    inline constexpr float slider_knob_r      = 7.f;
    inline constexpr float combo_h            = 24.f;
    inline constexpr float combo_w            = 157.f;
    inline constexpr float compact_combo_w    = 106.f;
    inline constexpr float combo_round        = 4.f;
    inline constexpr float keybind_w          = 157.f;
    inline constexpr float compact_keybind_w  = 106.f;
    inline constexpr float keybind_icon_size  = 13.f;
    inline constexpr float keybind_icon_pad   = 8.f;
    inline constexpr float dropdown_item_h    = 28.f;
    inline constexpr float color_swatch_size  = 24.f;
    inline constexpr float button_h           = 24.f;

    // ============ UI SCALE ============
    inline constexpr float ref_display_w = 1920.f;
    inline constexpr float ref_display_h = 1080.f;
    inline constexpr float ref_factor    = 0.9375f * 0.75f;  // 0.703125f

    inline float auto_factor   = ref_factor;
    inline float manual_factor = 1.f;
    inline float factor        = ref_factor;

    inline void recompute() { factor = auto_factor * manual_factor; }

    inline void set_display_size(const ImVec2& display)
    {
        if (display.x <= 0.f || display.y <= 0.f) return;
        const float fit = ImMin(display.x / ref_display_w, display.y / ref_display_h);
        auto_factor = ImClamp(fit, 0.5f, 3.f) * ref_factor;
        recompute();
    }

    inline void set_percent(float percent)
    {
        manual_factor = ImMax(0.1f, percent * 0.01f);
        recompute();
    }

    inline float s(float design_px) { return design_px * factor; }
    inline float px(float design_px) { return design_px * factor; }

    // ============ ANIMATION ============
    inline constexpr float anim_slider_min = 0.f;
    inline constexpr float anim_slider_max = 200.f;
    inline constexpr float anim_threshold  = 50.f;
    inline float anim_speed_percent = 100.f;

    inline bool anim_enabled() { return anim_speed_percent >= anim_threshold; }

    inline float anim_multiplier()
    {
        if (!anim_enabled()) return 0.f;
        return anim_speed_percent / 100.f;
    }

    inline float anim_lerp_t(float rate)
    {
        const float m = anim_multiplier();
        if (m <= 0.f) return 1.f;
        return ImMin(1.f, ImGui::GetIO().DeltaTime * rate * m);
    }

    inline float anim_step(float current, float target, float rate)
    {
        return ImLerp(current, target, anim_lerp_t(rate));
    }

    // ============ APPLY TO IMGUI STYLE ============
    inline void apply_style()
    {
        ImGuiStyle& s = ImGui::GetStyle();
        s.WindowRounding   = 0.f;
        s.WindowBorderSize = 0.f;
        s.WindowPadding    = { 0.f, 0.f };
        s.AntiAliasedLines = true;
        s.AntiAliasedFill  = true;
        s.ChildBorderSize  = 0.f;
        s.FrameBorderSize  = 0.f;
        s.ScrollbarSize    = 0.f;

        ImVec4* c = s.Colors;
        c[ImGuiCol_WindowBg]         = { 0.f, 0.f, 0.f, 0.f };
        c[ImGuiCol_ChildBg]          = { 0.f, 0.f, 0.f, 0.f };
        c[ImGuiCol_Text]             = text;
        c[ImGuiCol_ScrollbarBg]      = { 0.f, 0.f, 0.f, 0.f };
        c[ImGuiCol_ScrollbarGrab]    = { 0.f, 0.f, 0.f, 0.f };
        c[ImGuiCol_ScrollbarGrabHovered] = { 0.f, 0.f, 0.f, 0.f };
        c[ImGuiCol_ScrollbarGrabActive]  = { 0.f, 0.f, 0.f, 0.f };
    }

    inline void apply_scaled_style()
    {
        ImGui::GetStyle().ScaleAllSizes(factor);
        ImGui::GetIO().FontGlobalScale = 1.f;
    }

    // ============ ACCENT HELPERS ============
    inline ImVec4 accent_vec4(float alpha = 1.f) { return { g_accent.x, g_accent.y, g_accent.z, alpha }; }
    inline ImU32 accent_u32(float alpha = 1.f) { return ImGui::GetColorU32(accent_vec4(alpha)); }
}