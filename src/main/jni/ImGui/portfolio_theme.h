#pragma once
#include "imgui.h"
#include "imgui_internal.h"
#ifdef ETHNIR_LIQUID_SHADER
#include "liquid_glass_shader.h"
#endif

namespace portfolio
{

    inline ImVec4 g_accent = { 0x61/255.f, 0x5D/255.f, 0xCE/255.f, 1.f };

    // Glass fill, driven by the active theme. These used to be hardcoded to a
    // 63%-opaque near-black, which is what made light mode unreadable: the
    // palette flipped the text to near-black but the panel stayed black behind
    // it. EqSyncGlassTheme() in ethnir_menu.h drives them.
    inline ImVec4 g_glass_veil  = { 14/255.f, 14/255.f, 22/255.f, 80/255.f };
    inline ImVec4 g_glass_tint  = { 0.f,       0.f,       0.f,       160/255.f };
    inline ImVec4 g_glass_shade = { 14/255.f, 14/255.f, 22/255.f };
    inline float g_glass_shade_a0 = 46/255.f;
    inline float g_glass_shade_a1 = 82/255.f;

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

    inline constexpr float window_w         = 1060.f;
    inline constexpr float window_h         = 610.f;
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

    inline constexpr float sidebar_tabs_gap   = 0.f;
    inline constexpr float row_label_y_nudge  = 0.f;
    inline constexpr float row_label_font     = 14.f;
    inline constexpr float section_header_h   = 13.f;
    inline constexpr float user_avatar_size   = 60.f;
    inline constexpr float user_avatar_x      = 35.f;
    inline constexpr float user_bottom_margin = 36.f;
    inline constexpr float user_text_gap      = 16.f;
    inline constexpr float user_line_gap      = 4.f;
    inline constexpr float user_name_font     = 14.f;
    inline constexpr float logo_font          = 36.f;

    inline constexpr float kScreenMarginPx = 10.f;
    inline constexpr float kMinFactor       = 0.55f;
    inline constexpr float kMaxFactor       = 0.85f;
    inline constexpr float kTouchTargetPx   = 44.f;

    inline float settings_box_height(int rows)
    {
        return rows * settings_row_h + box_pad_y * 2.f;
    }

    inline constexpr float ref_display_w = 1920.f;
    inline constexpr float ref_display_h = 1080.f;
    inline constexpr float ref_factor    = 0.9375f * 0.75f;

    inline float auto_factor   = ref_factor;
    inline float manual_factor = 1.f;
    inline float factor        = ref_factor;

    inline void recompute() { factor = auto_factor * manual_factor; }

    inline void set_display_size(const ImVec2& display)
    {
        if (display.x <= 0.f || display.y <= 0.f) return;

        const float usable_w = ImMax(1.f, display.x - 2.f * kScreenMarginPx);
        const float usable_h = ImMax(1.f, display.y - 2.f * kScreenMarginPx);
        const float fit = ImMin(usable_w / window_w, usable_h / window_h);
        auto_factor = ImClamp(fit, kMinFactor, kMaxFactor);
        recompute();
    }

    inline void set_percent(float percent)
    {
        manual_factor = ImMax(0.1f, percent * 0.01f);
        recompute();
    }

    inline float s(float design_px) { return design_px * factor; }
    inline float px(float design_px) { return design_px * factor; }

    inline float touch_pad_x() { return s(10.f); }
    inline float touch_pad_y() { return 2.f; }

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

    inline void apply_style()
    {
        ImGuiStyle& s = ImGui::GetStyle();

        s.WindowPadding            = ImVec2(0.f, 0.f);
        s.WindowRounding           = 0.f;
        s.WindowMinSize            = ImVec2(32.f, 32.f);
        s.ChildRounding            = 0.f;
        s.PopupRounding            = 0.f;
        s.FramePadding             = ImVec2(8.f, 6.f);
        s.FrameRounding            = 0.f;
        s.ItemSpacing              = ImVec2(10.f, 8.f);
        s.ItemInnerSpacing         = ImVec2(6.f, 6.f);
        s.CellPadding              = ImVec2(4.f, 4.f);
        s.TouchExtraPadding        = ImVec2(kTouchTargetPx * 0.18f, kTouchTargetPx * 0.18f);
        s.IndentSpacing            = 20.f;
        s.ColumnsMinSpacing        = 5.f;
        s.ScrollbarSize            = 6.f;
        s.ScrollbarRounding        = 0.f;
        s.GrabMinSize              = 12.f;
        s.GrabRounding             = 0.f;
        s.LogSliderDeadzone        = 0.f;
        s.TabRounding              = 0.f;
        s.TabMinWidthForCloseButton = FLT_MAX;
        s.SeparatorTextPadding     = ImVec2(50.f, 50.f);
        s.DisplayWindowPadding     = ImVec2(0.f, 0.f);
        s.DisplaySafeAreaPadding   = ImVec2(0.f, 0.f);
        s.MouseCursorScale         = 1.f;

        s.WindowBorderSize = 0.f;
        s.ChildBorderSize  = 0.f;
        s.FrameBorderSize  = 0.f;
        s.AntiAliasedLines = true;
        s.AntiAliasedFill  = true;

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
        apply_style();
        ImGui::GetStyle().ScaleAllSizes(factor);
        ImGui::GetIO().FontGlobalScale = 1.f;
    }

    inline ImVec4 accent_vec4(float alpha = 1.f) { return { g_accent.x, g_accent.y, g_accent.z, alpha }; }
    inline ImU32 accent_u32(float alpha = 1.f) { return ImGui::GetColorU32(accent_vec4(alpha)); }

    inline void draw_accent_rect(ImDrawList* dl, const ImVec2& p0, const ImVec2& p1,
        float rounding, float strength, int layers = 4)
    {
        if (strength <= 0.001f || p1.x <= p0.x || p1.y <= p0.y)
            return;
        const float w = p1.x - p0.x;
        const float h = p1.y - p0.y;
        for (int i = layers; i >= 1; --i)
        {
            const float t = (float)i / (float)layers;
            const float grow_x = w * 0.10f * t;
            const float grow_y = h * 0.55f * t;
            const float a = strength * 0.16f * (1.f - t * 0.55f);
            dl->AddRectFilled(
                { p0.x - grow_x, p0.y - grow_y },
                { p1.x + grow_x, p1.y + grow_y },
                accent_u32(a), rounding + grow_y);
        }
    }

    inline void uv_for_screen_rect(const ImVec4& r, ImVec2& uv_min, ImVec2& uv_max)
    {
        const ImVec2 disp = ImGui::GetIO().DisplaySize;
        if (disp.x <= 0.f || disp.y <= 0.f)
        {
            uv_min = ImVec2(0.f, 0.f);
            uv_max = ImVec2(1.f, 1.f);
            return;
        }
        uv_min = ImVec2(ImClamp(r.x / disp.x, 0.f, 1.f), ImClamp(r.y / disp.y, 0.f, 1.f));
        uv_max = ImVec2(ImClamp((r.x + r.z) / disp.x, 0.f, 1.f), ImClamp((r.y + r.w) / disp.y, 0.f, 1.f));

        if (uv_max.x <= uv_min.x) uv_max.x = ImMin(1.f, uv_min.x + 1e-4f);
        if (uv_max.y <= uv_min.y) uv_max.y = ImMin(1.f, uv_min.y + 1e-4f);
    }

    inline void DrawLiquidGlassPanel(ImDrawList* dl, const ImVec2& p0, const ImVec2& p1, float R, ImTextureID backdrop = nullptr)
    {
        const float w = p1.x - p0.x;
        const float h = p1.y - p0.y;
        if (w < 4.f || h < 4.f) return;

        dl->AddShadowRect(p0, p1, IM_COL32(0, 0, 0, 120), 40.f, ImVec2(0, 16), 0, R);

        if (backdrop)
        {
            const ImVec4 r(p0.x, p0.y, w, h);
            ImVec2 uv_min, uv_max;
            uv_for_screen_rect(r, uv_min, uv_max);

#ifdef ETHNIR_LIQUID_SHADER
            // Real optics: SDF normals, UV lensing, per-channel dispersion and
            // a Fresnel rim. Falls through to the draw-call version below if
            // the program failed to build.
            liquid::Params lp;
            lp.rect_min = p0;
            lp.rect_size = ImVec2(w, h);
            lp.uv = ImVec4(uv_min.x, uv_min.y, uv_max.x, uv_max.y);
            lp.radius = R;
            if (liquid::draw(dl, (GLuint)(intptr_t)backdrop, lp))
            {
                dl->AddRect(p0 + ImVec2(0.5f, 0.5f), p1 - ImVec2(0.5f, 0.5f),
                            IM_COL32(255, 255, 255, 26), R, 0, 1.f);
                return;
            }
#endif
            dl->AddImageRounded(backdrop, p0, p1, uv_min, uv_max, IM_COL32_WHITE, R);
            dl->AddRectFilled(p0, p1, ImGui::ColorConvertFloat4ToU32(g_glass_veil), R);
        }
        else
        {
            dl->AddRectFilled(p0, p1, ImGui::ColorConvertFloat4ToU32(g_glass_tint), R);
        }

        const float inset = ImMin(R * 0.5f, ImMin(w, h) * 0.08f);
        const ImVec2 ni0 = { p0.x + inset, p0.y + inset };
        const ImVec2 ni1 = { p1.x - inset, p1.y - inset };

        dl->AddRectFilledMultiColor(ni0, ni1,
            ImGui::ColorConvertFloat4ToU32(ImVec4(g_glass_shade.x, g_glass_shade.y, g_glass_shade.z, g_glass_shade_a0)),
            ImGui::ColorConvertFloat4ToU32(ImVec4(g_glass_shade.x, g_glass_shade.y, g_glass_shade.z, g_glass_shade_a0)),
            ImGui::ColorConvertFloat4ToU32(ImVec4(g_glass_shade.x, g_glass_shade.y, g_glass_shade.z, g_glass_shade_a1)),
            ImGui::ColorConvertFloat4ToU32(ImVec4(g_glass_shade.x, g_glass_shade.y, g_glass_shade.z, g_glass_shade_a1)),
            ImMin(R - inset, 8.f));

        const float rimW = ImMin(inset * 1.5f, 12.f);
        const int fringeSteps = 3;
        for (int i = 0; i < fringeSteps; ++i)
        {
            const float t = (float)(i + 1) / fringeSteps;
            const float offset = rimW * t * 0.5f;
            const float alpha = 0.03f * (1.f - t * 0.5f);

            dl->AddRectFilledMultiColor(
                { p0.x + R, p0.y + offset },
                { p1.x - R, p0.y + offset + 1.f },
                IM_COL32(255, 0, 0, (int)(alpha * 255)),
                IM_COL32(0, 0, 255, (int)(alpha * 255)),
                IM_COL32(0, 0, 255, (int)(alpha * 255)),
                IM_COL32(255, 0, 0, (int)(alpha * 255))
            );

            dl->AddRectFilledMultiColor(
                { p0.x + offset, p0.y + R },
                { p0.x + offset + 1.f, p1.y - R },
                IM_COL32(255, 0, 0, (int)(alpha * 255)),
                IM_COL32(255, 0, 0, (int)(alpha * 255)),
                IM_COL32(0, 0, 255, (int)(alpha * 255)),
                IM_COL32(0, 0, 255, (int)(alpha * 255))
            );

            dl->AddRectFilledMultiColor(
                { p1.x - offset - 1.f, p0.y + R },
                { p1.x - offset, p1.y - R },
                IM_COL32(0, 0, 255, (int)(alpha * 255)),
                IM_COL32(0, 0, 255, (int)(alpha * 255)),
                IM_COL32(255, 0, 0, (int)(alpha * 255)),
                IM_COL32(255, 0, 0, (int)(alpha * 255))
            );

            dl->AddRectFilledMultiColor(
                { p0.x + R, p1.y - offset - 1.f },
                { p1.x - R, p1.y - offset },
                IM_COL32(0, 0, 255, (int)(alpha * 255)),
                IM_COL32(255, 0, 0, (int)(alpha * 255)),
                IM_COL32(255, 0, 0, (int)(alpha * 255)),
                IM_COL32(0, 0, 255, (int)(alpha * 255))
            );
        }

        const float highlightAlpha = 0.5f;
        dl->AddRectFilledMultiColor(
            { p0.x + R, p0.y + 0.5f },
            { p1.x - R, p0.y + 1.5f },
            IM_COL32(255, 255, 255, (int)(highlightAlpha * 255)),
            IM_COL32(255, 255, 255, 0),
            IM_COL32(255, 255, 255, 0),
            IM_COL32(255, 255, 255, (int)(highlightAlpha * 255))
        );

        dl->AddRectFilledMultiColor(
            { p0.x + R + 4, p0.y + 8 },
            { p1.x - R - 4, p0.y + 16 },
            IM_COL32(255, 255, 255, 15),
            IM_COL32(255, 255, 255, 0),
            IM_COL32(255, 255, 255, 0),
            IM_COL32(255, 255, 255, 15)
        );

        dl->AddRect(p0 + ImVec2(0.5f, 0.5f), p1 - ImVec2(0.5f, 0.5f),
            IM_COL32(255, 255, 255, 33), R, 0, 1.f);

        const float accentRimAlpha = 0.04f;
        const float cornerR = R;
        dl->AddRectFilledMultiColor(
            { p0.x + cornerR, p0.y },
            { p1.x - cornerR, p0.y + 2.f },
            accent_u32(accentRimAlpha), IM_COL32(0,0,0,0), IM_COL32(0,0,0,0), accent_u32(accentRimAlpha)
        );
        dl->AddRectFilledMultiColor(
            { p0.x, p0.y + cornerR },
            { p0.x + 2.f, p1.y - cornerR },
            accent_u32(accentRimAlpha), accent_u32(accentRimAlpha), IM_COL32(0,0,0,0), IM_COL32(0,0,0,0)
        );
    }

    inline void DrawLiquidGlassButton(ImDrawList* dl, const ImVec2& p0, const ImVec2& p1, float R,
        const char* icon, const char* label, ImFont* iconFont, float iconSize,
        ImFont* textFont, float textSize,
        bool hovered, bool pressed, bool active)
    {
        ImU32 bgCol = pressed ? IM_COL32(0, 0, 0, 120) : (hovered ? IM_COL32(255, 255, 255, 30) : IM_COL32(255, 255, 255, 15));
        ImU32 borderCol = active ? accent_u32(0.6f) : (hovered ? IM_COL32(255, 255, 255, 60) : IM_COL32(255, 255, 255, 25));
        ImU32 textCol = hovered || active ? IM_COL32_WHITE : IM_COL32(255, 255, 255, 200);

        dl->AddRectFilled(p0, p1, bgCol, R);
        dl->AddRect(p0 + ImVec2(0.5f, 0.5f), p1 - ImVec2(0.5f, 0.5f), borderCol, R, 0, 1.f);

        dl->AddRectFilledMultiColor(
            { p0.x + R, p0.y + 0.5f },
            { p1.x - R, p0.y + 1.5f },
            IM_COL32(255, 255, 255, 60), IM_COL32(255, 255, 255, 0),
            IM_COL32(255, 255, 255, 0), IM_COL32(255, 255, 255, 60)
        );

        const float cx = (p0.x + p1.x) * 0.5f;
        const float cy = (p0.y + p1.y) * 0.5f;
        const float gap = 8.f;
        const float labelW = (label && textFont) ? textFont->CalcTextSizeA(textSize, FLT_MAX, 0.f, label).x : 0.f;
        const float totalW = (icon ? iconSize : 0.f) + ((icon && label) ? gap : 0.f) + labelW;

        float x = cx - totalW * 0.5f;
        if (icon && iconFont)
        {

            dl->AddText(iconFont, iconSize, ImVec2(x, cy - iconSize * 0.5f), textCol, icon);
            x += iconSize + gap;
        }
        if (label && textFont)
            dl->AddText(textFont, textSize, ImVec2(x, cy - textSize * 0.5f), textCol, label);
    }

    inline void DrawLiquidGlassSearchField(ImDrawList* dl, const ImVec2& p0, const ImVec2& p1, float R,
        bool focused, bool hovered)
    {
        ImU32 bgCol     = focused ? IM_COL32(255, 255, 255, 25) : (hovered ? IM_COL32(255, 255, 255, 15) : IM_COL32(255, 255, 255, 10));
        ImU32 borderCol = focused ? accent_u32(0.55f) : (hovered ? IM_COL32(255, 255, 255, 40) : IM_COL32(255, 255, 255, 15));

        dl->AddRectFilled(p0, p1, bgCol, R);
        dl->AddRect(p0 + ImVec2(0.5f, 0.5f), p1 - ImVec2(0.5f, 0.5f), borderCol, R, 0, 1.f);

        dl->AddRectFilledMultiColor(
            { p0.x + R, p0.y + 0.5f },
            { p1.x - R, p0.y + 1.5f },
            IM_COL32(255, 255, 255, 50), IM_COL32(255, 255, 255, 0),
            IM_COL32(255, 255, 255, 0), IM_COL32(255, 255, 255, 50)
        );
    }
}
