#pragma once
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_settings.h"
#include "portfolio_theme.h"
#include "realtime_backdrop.h"
#include "Icon.h"
#include <functional>
#include <cmath>
#include <cstring>
#include <cctype>
#include <cstdio>

namespace ethnir
{
    struct MenuState
    {
        bool        Open          = true;
        int         ActiveTab     = 1;
        char        Search[64]    = "";
        const char* TitleText     = "ETHNIR";
        const char* SubtitleText  = "MOD MENU";
        ImTextureID Backdrop      = nullptr;

        int HeaderPressed  = -1;
        int TrafficPressed = -1;

        bool  Dark              = true;
        bool  ShowSettingsPanel = true;
        bool  HelpOpen          = false;
        float AnimSpeed         = 1.0f;
        int   AccentIndex       = 0;
        int   MenuBind          = 0;

        bool  AutoSave          = true;
        float SaveDebounce      = 0.5f;
        bool  Dirty             = false;
        bool  InputActive       = false;
        float DirtyTimer        = 0.0f;
        std::function<void()> OnSave;

        std::function<void(int tab)> DrawTab;

        int    LastTab  = -1;
        float  Fade     = 1.0f;
        float  Appear   = 0.0f;

        float  BackdropPhase = 0.0f;
        bool   WasOpen  = false;
        bool   Closing  = false;
        ImVec2 WinPos   = ImVec2(0.0f, 0.0f);
        bool   WinPosInit = false;
        bool   Dragging   = false;
        ImVec2 PanelPos   = ImVec2(0.0f, 0.0f);
        bool   PanelPosInit = false;
        bool   PanelDragging = false;
        float  SaveFlash  = 0.0f;

        float  DragScaleX = 1.0f;
        float  DragScaleY = 1.0f;

        bool   HeaderControl = false;

        ImVec2 DragRef   = ImVec2(0.0f, 0.0f);
        bool   FrameSearchHit = false;
        bool   LastSearchHit  = true;
        bool   HelpChipPressedFrame = false;
        int    FrameRowCount  = 0;
        int    LastRowCount   = 0;
    };

    inline float kWinW     = portfolio::s(portfolio::window_w);
    inline float kWinH     = portfolio::s(portfolio::window_h);
    inline float kPad      = portfolio::s(14.f);
    inline float kBrandH   = portfolio::s(30.f);
    inline float kSideW    = portfolio::s(portfolio::sidebar_w);
    inline float kColGap   = portfolio::s(portfolio::column_gap);
    inline float kRadius   = portfolio::s(portfolio::shell_round);
    inline float kRowH     = portfolio::s(portfolio::settings_row_h);
    inline float kSliderH  = portfolio::s(32.f);

    inline void RefreshMetrics()
    {
        kWinW    = portfolio::s(portfolio::window_w);
        kWinH    = portfolio::s(portfolio::window_h);
        kPad     = portfolio::s(14.f);
        kBrandH  = portfolio::s(30.f);
        kSideW   = portfolio::s(portfolio::sidebar_w);
        kColGap  = portfolio::s(portfolio::column_gap);
        kRadius  = portfolio::s(portfolio::shell_round);
        kRowH    = portfolio::s(portfolio::settings_row_h);
        kSliderH = portfolio::s(32.f);
    }

    constexpr ImVec4 kSwitchOn(0.204f, 0.780f, 0.349f, 1.0f);

    inline ImU32 PortfolioAccent() { return portfolio::accent_u32(); }
    inline ImVec4 PortfolioAccentVec(float alpha = 1.f) { return portfolio::accent_vec4(alpha); }

    static const char* kMenuBinds[] = { "Num 0", "Num 1", "F1", "F4", "Home", "None" };
    static const int kMenuBindCount = IM_ARRAYSIZE(kMenuBinds);

    struct Palette
    {
        ImVec4 base, scrim, text, textDim, textFaint;
        ImVec4 cardBg, cardEdge, hover, side, sideEdge, switchOff, track, popupBg, sep, glassRim;
    };

    inline Palette EqPalDark()
    {
        Palette p;
        using namespace portfolio;
        p.base      = panel;
        p.scrim     = bg;
        p.text      = text;
        p.textDim   = text_muted;
        p.textFaint = header_text;
        p.cardBg    = box;
        p.cardEdge  = separator;
        p.glassRim  = { 1.f, 1.f, 1.f, 1.f };
        p.hover     = control_hover;
        p.side      = sidebar;
        p.sideEdge  = sidebar_sep;
        p.switchOff = toggle_off;
        p.track     = slider_bg;
        p.popupBg   = dropdown_bg;
        p.sep       = separator;
        return p;
    }

    inline Palette EqPalLight()
    {
        Palette p;
        const float w = 1.f;
        p.base      = { w, w, w, 0.55f };
        p.scrim     = { w, w, w, 0.55f };
        p.text      = { 0x14/255.f, 0x14/255.f, 0x1A/255.f, 1.f };
        p.textDim   = { 0.f, 0.f, 0.f, 0.58f };
        p.textFaint = { 0.f, 0.f, 0.f, 0.34f };
        p.cardBg    = { w, w, w, 0.45f };
        p.cardEdge  = { 0.f, 0.f, 0.f, 0.12f };
        p.glassRim  = { 1.f, 1.f, 1.f, 1.f };
        p.hover     = { w, w, w, 0.72f };
        p.side      = { w, w, w, 0.45f };
        p.sideEdge  = { 0.f, 0.f, 0.f, 0.14f };
        p.switchOff = { 0xC7/255.f, 0xC7/255.f, 0xCC/255.f, 1.f };
        p.track     = { 0xD2/255.f, 0xD2/255.f, 0xD6/255.f, 1.f };
        p.popupBg   = { w, w, w, 0.86f };
        p.sep       = { 0.f, 0.f, 0.f, 0.12f };
        return p;
    }

    inline MenuState*& EqState() { static MenuState* s = nullptr; return s; }
    inline Palette EqPal() { MenuState* s = EqState(); return (s && !s->Dark) ? EqPalLight() : EqPalDark(); }

    inline ImFont* EqTextFont() { if (font::inter_semibold) return font::inter_semibold; if (F50) return F50; return ImGui::GetFont(); }
    inline ImFont* EqTitleFont() { if (F50) return F50; return EqTextFont(); }
    inline ImFont* EqIconFont() { if (F107) return F107; return EqTextFont(); }

    inline float EqRate(float k) { MenuState* s = EqState(); return k * (s ? ImMax(0.15f, s->AnimSpeed) : 1.0f); }
    inline float EqDamp(float a, float b, float k, float dt) { return b + (a - b) * std::exp(-EqRate(k) * dt); }
    inline float EqEase(float t) { return t * t * (3.0f - 2.0f * t); }

    inline float EqAnim(const char* key, float target, float rate, float dt, float initial = 0.0f)
    {
        ImGuiStorage* stg = ImGui::GetStateStorage();
        const ImGuiID id = ImGui::GetID(key);
        const float v = stg->GetFloat(id, initial);
        const float nv = EqDamp(v, target, rate, dt);
        stg->SetFloat(id, nv);
        return nv;
    }

    inline ImVec4 EqMix(const ImVec4& a, const ImVec4& b, float t) { return ImVec4(ImLerp(a.x, b.x, t), ImLerp(a.y, b.y, t), ImLerp(a.z, b.z, t), ImLerp(a.w, b.w, t)); }
    inline ImU32 EqCol(const ImVec4& c) { return ImGui::GetColorU32(c); }
    inline ImU32 EqColA(const ImVec4& c, float a) { return ImGui::GetColorU32(ImVec4(c.x, c.y, c.z, c.w * a)); }
    inline ImU32 EqAccent() { return portfolio::accent_u32(); }
    inline ImVec4 EqAccentVec() { return portfolio::accent_vec4(); }
    inline ImU32 EqAccentA(float a) { return portfolio::accent_u32(a); }

    inline void EqDrawGlassRim(ImDrawList* dl, const ImVec2& p0, const ImVec2& p1, float R, const Palette& pal)
    {
        const float w = p1.x - p0.x, h = p1.y - p0.y;

        if (w < 2.0f || h < 2.0f) return;
        const float t = ImMin(1.6f, ImMin(w, h) * 0.5f);
        const float x0 = p0.x, y0 = p0.y, x1 = p1.x, y1 = p1.y;
        const ImU32 hi  = EqColA(pal.glassRim, 0.55f);
        const ImU32 dim = EqColA(pal.glassRim, 0.16f);
        const ImU32 off = EqColA(pal.glassRim, 0.0f);
        const float rx = ImMin(R, (x1 - x0) * 0.5f);
        const float ry = ImMin(R, (y1 - y0) * 0.5f);

        dl->AddRectFilledMultiColor(ImVec2(x0 + rx, y0), ImVec2(x1 - rx, y0 + t), hi, off, off, hi);
        dl->AddRectFilledMultiColor(ImVec2(x0, y0 + ry), ImVec2(x0 + t, y1 - ry), hi, hi, dim, dim);
        dl->AddRectFilledMultiColor(ImVec2(x1 - t, y0 + ry), ImVec2(x1, y1 - ry), dim, dim, off, off);
        dl->AddRectFilledMultiColor(ImVec2(x0 + rx, y1 - t), ImVec2(x1 - rx, y1), dim, dim, dim, dim);
    }

    inline void EqApplyAccentIndex(int index)
    {
        (void)index;
        MenuState* s = EqState();
        if (s) s->AccentIndex = 0;
        main_runtime_theme::ApplyAccentFromHue();
        if (s) s->Dirty = true;
    }

    inline void EqStripId(const char* label, char* out, int cap)
    {
        if (!label || cap <= 0) { if (cap > 0) out[0] = 0; return; }
        int i = 0;
        for (; i < cap - 1 && label[i] != 0; ++i)
        {
            if (label[i] == '#' && label[i + 1] == '#') break;
            out[i] = label[i];
        }
        out[i] = 0;
    }

    inline void EqMarkDirty() { MenuState* s = EqState(); if (s) s->Dirty = true; }

    inline float EqBackdropPhase() { MenuState* s = EqState(); return s ? s->BackdropPhase : 0.0f; }

    inline bool EqPress(const char* id, const ImVec2& size, bool* outHovered = nullptr, bool* outHeld = nullptr)
    {
        ImGuiWindow* w = ImGui::GetCurrentWindow();
        const ImGuiID wid = w->GetID(id);
        const ImVec2 p = ImGui::GetCursorScreenPos();

        const float px = portfolio::touch_pad_x();
        const float py = portfolio::touch_pad_y();
        const ImVec2 hit_min(p.x - px, p.y - py);
        const ImVec2 hit_max(p.x + size.x + px, p.y + size.y + py);

        bool hovered = false, held = false;
        const bool pressed = ImGui::ButtonBehavior(ImRect(hit_min, hit_max), wid, &hovered, &held, ImGuiButtonFlags_None);

        ImGui::KeepAliveID(wid);
        if (outHovered) *outHovered = hovered;
        if (outHeld) *outHeld = held;
        if (held && EqState()) EqState()->InputActive = true;
        return pressed;
    }

    struct ColumnState { bool Active = false; float X0 = 0, X1 = 0, W = 0, TopY = 0; int Col = 0; float Y[2] = { 0, 0 }; };
    inline ColumnState& EqCols() { static ColumnState c; return c; }
    inline float EqCardWidth()
    {
        ColumnState& c = EqCols();
        if (c.Active) return c.W;
        return ImGui::GetContentRegionAvail().x;
    }

    inline void EqBeginColumns(float gap = kColGap)
    {
        ColumnState& c = EqCols();
        const ImVec2 p = ImGui::GetCursorScreenPos();
        const float avail = ImGui::GetContentRegionAvail().x;
        c.Active = true;
        c.W = ImMax(1.0f, ImFloor((avail - gap) * 0.5f));
        c.X0 = p.x;
        c.X1 = p.x + c.W + gap;
        c.TopY = p.y;
        c.Col = 0;
        c.Y[0] = c.Y[1] = p.y;
    }

    inline void EqNextColumn()
    {
        ColumnState& c = EqCols();
        if (!c.Active || c.Col >= 1) return;
        c.Y[0] = ImGui::GetCursorScreenPos().y;
        c.Col = 1;
        ImGui::SetCursorScreenPos(ImVec2(c.X1, c.TopY));
    }

    inline void EqEndColumns()
    {
        ColumnState& c = EqCols();
        if (!c.Active) return;
        c.Y[c.Col] = ImGui::GetCursorScreenPos().y;
        ImGui::SetCursorScreenPos(ImVec2(c.X0, ImMax(c.Y[0], c.Y[1])));
        c.Active = false;
    }

    inline void EqDrawGlyph(ImDrawList* dl, const char* glyph, ImVec2 center, float size, ImU32 col)
    {
        if (!glyph) return;
        ImFont* f = EqIconFont();
        const ImVec2 ts = f->CalcTextSizeA(size, FLT_MAX, 0.0f, glyph);
        dl->AddText(f, size, ImVec2(center.x - ts.x * 0.5f, center.y - ts.y * 0.5f), col, glyph);
    }

    inline void EqDrawLabel(ImDrawList* dl, ImVec2 pos, const char* text, ImU32 col, float size)
    {
        dl->AddText(EqTextFont(), size, pos, col, text);
    }

    inline ImVec2 EqLabelSize(const char* text, float size)
    {
        return EqTextFont()->CalcTextSizeA(size, FLT_MAX, 0.0f, text);
    }

    inline float EqTrackedWidth(ImFont* f, float size, const char* text, float track)
    {
        const float w = f->CalcTextSizeA(size, FLT_MAX, 0.0f, text).x;
        const int n = (int)strlen(text);
        return n > 1 ? w + track * (float)(n - 1) : w;
    }

    inline void EqDrawTracked(ImDrawList* dl, ImFont* f, float size, ImVec2 pos, ImU32 col, const char* text, float track)
    {
        if (!text) return;
        for (const char* c = text; *c; ++c)
        {
            char buf[2] = { *c, 0 };
            dl->AddText(f, size, pos, col, buf);
            pos.x += f->CalcTextSizeA(size, FLT_MAX, 0.0f, buf).x + track;
        }
    }

    inline bool& EqFilterOn() { static bool on = true; return on; }

    inline bool EqPassFilter(const char* label)
    {
        MenuState* st = EqState();
        if (!st) return true;
        if (!EqFilterOn()) return true;
        st->FrameRowCount++;
        if (st->Search[0] == 0) { st->FrameSearchHit = true; return true; }
        char a[128], b[128];
        int i = 0; for (; i < 127 && label[i]; ++i) a[i] = (char)toupper((unsigned char)label[i]); a[i] = 0;
        i = 0; for (; i < 127 && st->Search[i]; ++i) b[i] = (char)toupper((unsigned char)st->Search[i]); b[i] = 0;
        const bool hit = strstr(a, b) != nullptr;
        if (hit) st->FrameSearchHit = true;
        return hit;
    }

    inline void EqDrawSwitch(ImDrawList* dl, const char* id, bool on, ImVec2 center, float dt, bool hovered)
    {
        const float active = EqAnim(id, on ? 1.0f : 0.0f, 16.0f, dt, on ? 1.0f : 0.0f);
        const Palette pal = EqPal();

        const float w = ImFloor(portfolio::s(portfolio::toggle_w) + 0.5f);
        const float h = ImFloor(portfolio::s(portfolio::toggle_h) + 0.5f);
        const ImVec2 mn(ImFloor(center.x - w * 0.5f), ImFloor(center.y - h * 0.5f));
        const ImVec2 mx(mn.x + w, mn.y + h);
        const float round = portfolio::s(portfolio::toggle_round);

        const ImVec4 track = on ? portfolio::control
                                : EqMix(pal.track, portfolio::control_hover, hovered ? 1.0f : 0.0f);
        dl->AddRectFilled(mn, mx, EqCol(track), round);

        if (active > 0.01f)
        {
            dl->AddRectFilled(mn, mx, EqAccentA(0.10f * active), round);
            portfolio::draw_accent_rect(dl, mn, mx, round, active * 0.55f);
        }

        const ImVec4 knob = EqMix(EqMix(pal.textDim, portfolio::circle_checkbox_hover, hovered ? 1.0f : 0.0f),
                                  portfolio::accent_vec4(), active);
        const float knob_x = mn.x + portfolio::s(portfolio::toggle_knob_inset)
                           + portfolio::s(portfolio::toggle_knob_travel) * active;
        dl->AddCircleFilled(ImVec2(knob_x, center.y),
                            portfolio::s(portfolio::toggle_knob_r), EqCol(knob), 24);
    }

    inline int& EqCardRowIndex() { static int i = 0; return i; }

    inline void EqRowSeparator(ImDrawList*, const ImVec2&, const ImVec2&) {}

    inline void SectionLabel(const char* text)
    {
        const ImVec2 p = ImGui::GetCursorScreenPos();
        ImDrawList* dl = ImGui::GetWindowDrawList();
        const Palette pal = EqPal();
        const float size = portfolio::s(portfolio::section_header_h);
        dl->AddText(EqTextFont(), size, ImVec2(p.x, p.y), EqCol(pal.textFaint), text);
        ImGui::SetCursorScreenPos(ImVec2(p.x, p.y + size + portfolio::s(6.f)));
    }

    inline void BeginGroupCard(const char* id)
    {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, portfolio::box);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, portfolio::s(portfolio::box_round));
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,
            ImVec2(portfolio::s(portfolio::box_pad_x), portfolio::s(portfolio::box_pad_y)));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
        ImGuiWindow* parent = ImGui::GetCurrentWindow();
        char name[160];
        ImFormatString(name, IM_ARRAYSIZE(name), "%s/ethcard_%08X", parent->Name, parent->GetID(id));
        ImGui::SetNextWindowSize(ImVec2(EqCardWidth(), 0.0f));
        ImGui::Begin(name, nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
                     ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_ChildWindow |
                     ImGuiWindowFlags_AlwaysAutoResize);
        EqCardRowIndex() = 0;
    }

    inline void EndGroupCard()
    {
        ImGui::EndChild();
        ImGui::PopStyleVar(4);
        ImGui::PopStyleColor();
        ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2(0.0f, portfolio::s(12.f)));
    }

    inline bool RowToggle(const char* icon, const char* label, bool* v)
    {
        (void)icon;
        if (!EqPassFilter(label)) return false;
        const Palette pal = EqPal();
        const ImVec2 p = ImGui::GetCursorScreenPos();
        const float w = ImGui::GetContentRegionAvail().x;
        const float h = kRowH;

        char clean[128];
        EqStripId(label, clean, IM_ARRAYSIZE(clean));
        bool hovered = false;
        const bool pressed = EqPress(label, ImVec2(w, h), &hovered);
        if (pressed) { *v = !*v; EqMarkDirty(); }

        ImDrawList* dl = ImGui::GetWindowDrawList();
        if (EqCardRowIndex()++ > 0) EqRowSeparator(dl, p, p + ImVec2(w, h));
        if (hovered) dl->AddRectFilled(p, p + ImVec2(w, h), EqColA(pal.text, 0.025f), portfolio::s(portfolio::control_round));
        const float labelSize = portfolio::s(portfolio::row_label_font);
        const float ty = p.y + (h - labelSize) * 0.5f;
        EqDrawLabel(dl, ImVec2(p.x + portfolio::s(portfolio::box_pad_x), ty), clean,
                    EqCol(*v || hovered ? pal.text : pal.textDim), labelSize);

        const float tw = ImFloor(portfolio::s(portfolio::toggle_w) + 0.5f);
        EqDrawSwitch(dl, label, *v, ImVec2(p.x + w - portfolio::s(portfolio::box_pad_x) - tw * 0.5f, p.y + h * 0.5f),
                     ImGui::GetIO().DeltaTime, hovered);

        ImGui::SetCursorScreenPos(ImVec2(p.x, p.y + h + 2.0f));
        return pressed;
    }

    inline bool RowSlider(const char* icon, const char* label, float* v, float v_min, float v_max, const char* fmt)
    {
        (void)icon;
        if (!EqPassFilter(label)) return false;
        ImGuiIO& io = ImGui::GetIO();
        const Palette pal = EqPal();
        const ImVec2 p = ImGui::GetCursorScreenPos();
        const float w = ImGui::GetContentRegionAvail().x;
        const float h = kSliderH;
        const float trackW = portfolio::s(portfolio::slider_w);
        const float trackX = p.x + w - trackW;
        const float trackY = p.y + h * 0.5f;

        char clean[128];
        EqStripId(label, clean, IM_ARRAYSIZE(clean));
        bool hovered = false, held = false;
        EqPress(label, ImVec2(w, h), &hovered, &held);

        bool changed = false;
        if (held)
        {
            const float t = ImSaturate((io.MousePos.x - trackX) / trackW);
            const float nv = v_min + t * (v_max - v_min);
            changed = (nv != *v);
            *v = nv;
            if (changed) EqMarkDirty();
        }

        char id2[160];
        ImFormatString(id2, IM_ARRAYSIZE(id2), "%s##anim", label);
        const float shown = EqAnim(id2, *v, held ? 40.0f : 18.0f, io.DeltaTime, *v);

        char buf[32];
        ImFormatString(buf, IM_ARRAYSIZE(buf), fmt, *v);
        const float labelSize = portfolio::s(portfolio::row_label_font);
        const ImVec2 vs = EqTextFont()->CalcTextSizeA(labelSize, FLT_MAX, 0.0f, buf);
        ImDrawList* dl = ImGui::GetWindowDrawList();
        if (EqCardRowIndex()++ > 0) EqRowSeparator(dl, p, p + ImVec2(w, h));
        if (hovered || held) dl->AddRectFilled(p, p + ImVec2(w, h), EqColA(pal.text, 0.025f), portfolio::s(portfolio::control_round));
        EqDrawLabel(dl, ImVec2(p.x + portfolio::s(portfolio::box_pad_x), p.y + (h - labelSize) * 0.5f), clean,
                    EqCol(hovered || held ? pal.text : pal.textDim), labelSize);

        EqDrawLabel(dl, ImVec2(trackX - vs.x - portfolio::s(10.f), p.y + (h - labelSize) * 0.5f),
                    buf, EqCol(hovered || held ? pal.text : pal.textDim), labelSize);

        const float trackH = portfolio::s(portfolio::slider_track_h);
        const float fillH  = portfolio::s(portfolio::slider_h);
        const float fillW  = ImSaturate((shown - v_min) / ImMax(0.0001f, v_max - v_min)) * trackW;

        dl->AddRectFilled(ImVec2(trackX, trackY - trackH * 0.5f),
                          ImVec2(trackX + trackW, trackY + trackH * 0.5f),
                          EqCol(EqMix(pal.track, portfolio::control_hover, hovered ? 0.35f : 0.0f)),
                          trackH * 0.5f);

        if (fillW > 0.5f)
        {
            const ImVec2 f0(trackX, trackY - fillH * 0.5f);
            const ImVec2 f1(trackX + fillW, trackY + fillH * 0.5f);
            portfolio::draw_accent_rect(dl, f0, f1, fillH * 0.5f, 0.45f);
            dl->AddRectFilled(f0, f1, EqAccent(), fillH * 0.5f);
        }

        const ImVec2 kc(trackX + fillW, trackY);
        if (hovered || held) dl->AddCircleFilled(kc, portfolio::s(portfolio::slider_knob_r), EqAccentA(0.22f), 20);
        dl->AddCircleFilled(kc, held ? portfolio::s(portfolio::slider_knob_r) * 0.85f : portfolio::s(portfolio::slider_knob_r) * 0.7f, IM_COL32(252, 253, 255, 250), 20);

        ImGui::SetCursorScreenPos(ImVec2(p.x, p.y + h + 2.0f));
        return changed;
    }

    inline bool ComboRow(const char* icon, const char* label, int* current, const char* const* items, int count)
    {
        (void)icon;
        if (!EqPassFilter(label)) return false;
        if (count <= 0) return false;
        *current = ImClamp(*current, 0, count - 1);
        ImGuiIO& io = ImGui::GetIO();
        const Palette pal = EqPal();
        const ImVec2 p = ImGui::GetCursorScreenPos();
        const float w = ImGui::GetContentRegionAvail().x;
        const float h = portfolio::s(portfolio::combo_h);

        char clean[128];
        EqStripId(label, clean, IM_ARRAYSIZE(clean));
        bool hovered = false;
        const bool pressed = EqPress(label, ImVec2(w, h), &hovered);

        ImDrawList* dl = ImGui::GetWindowDrawList();
        if (EqCardRowIndex()++ > 0) EqRowSeparator(dl, p, p + ImVec2(w, h));
        if (hovered) dl->AddRectFilled(p, p + ImVec2(w, h), EqColA(pal.text, 0.025f), portfolio::s(portfolio::control_round));
        const float labelSize = portfolio::s(portfolio::row_label_font);
        EqDrawLabel(dl, ImVec2(p.x + portfolio::s(portfolio::box_pad_x), p.y + (h - labelSize) * 0.5f),
                    clean, EqCol(hovered ? pal.text : pal.textDim), labelSize);

        const float comboW = portfolio::s(portfolio::combo_w);
        const float comboH = portfolio::s(portfolio::combo_h);
        const ImVec2 bmin(p.x + w - portfolio::s(portfolio::box_pad_x) - comboW, p.y + (h - comboH) * 0.5f);
        const ImVec2 bmax(bmin.x + comboW, bmin.y + comboH);
        const bool open = ImGui::IsPopupOpen(label);
        dl->AddRectFilled(bmin, bmax, EqCol(hovered || open ? portfolio::control_hover : portfolio::control),
                          portfolio::s(portfolio::combo_round));

        const char* preview = items[*current];
        EqDrawLabel(dl, ImVec2(bmin.x + portfolio::s(10.f), bmin.y + (comboH - labelSize) * 0.5f),
                    preview, EqCol(hovered || open ? pal.text : pal.textDim), labelSize);

        char cid[160];
        ImFormatString(cid, IM_ARRAYSIZE(cid), "%s##chev", label);
        const float ct = EqAnim(cid, open ? 1.0f : 0.0f, 18.0f, io.DeltaTime, 0.0f);
        const float cy = (bmin.y + bmax.y) * 0.5f;
        const float a = ImLerp(1.6f, -1.6f, ct);
        const ImU32 chev = EqCol(EqMix(pal.textDim, portfolio::g_accent, ct * 0.6f));
        const float chx = bmax.x - portfolio::s(8.f);
        dl->AddLine(ImVec2(chx - 4.0f, cy - a), ImVec2(chx, cy + a), chev, 1.4f);
        dl->AddLine(ImVec2(chx, cy + a), ImVec2(chx + 4.0f, cy - a), chev, 1.4f);

        if (pressed) ImGui::OpenPopup(label);

        bool changed = false;
        const float itemH = 30.0f;
        const float popupH = count * itemH + 12.0f;
        ImVec2 ppos(p.x, p.y + h + 4.0f);
        if (ppos.y + popupH > io.DisplaySize.y - 8.0f && p.y - popupH - 4.0f > 0.0f)
            ppos.y = p.y - popupH - 4.0f;
        ImGui::SetNextWindowPos(ppos);
        ImGui::SetNextWindowSize(ImVec2(w, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_PopupBg, pal.popupBg);
        ImGui::PushStyleColor(ImGuiCol_Border, pal.cardEdge);
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(7.0f, 6.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 14.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 1.0f);
        if (ImGui::BeginPopup(label))
        {
            for (int i = 0; i < count; ++i)
            {
                ImGui::PushID(i);
                const bool sel = (*current == i);
                ImGui::PushStyleColor(ImGuiCol_Text, sel ? EqAccent() : EqCol(pal.text));
                if (ImGui::Selectable(items[i], false, 0, ImVec2(0.0f, itemH))) { *current = i; changed = true; EqMarkDirty(); }
                ImGui::PopStyleColor();
                const bool hov = ImGui::IsItemHovered();
                const ImVec2 imin = ImGui::GetItemRectMin();
                const ImVec2 imax = ImGui::GetItemRectMax();
                ImDrawList* pdl = ImGui::GetWindowDrawList();
                if (hov) pdl->AddRectFilled(imin, imax, EqColA(pal.text, 0.06f), 9.0f);
                if (i > 0) pdl->AddLine(ImVec2(imin.x + 2.0f, imin.y), ImVec2(imax.x - 2.0f, imin.y), EqCol(pal.sep), 1.0f);
                if (sel) EqDrawGlyph(pdl, ICON_FA_CHECK, ImVec2(imax.x - 16.0f, (imin.y + imax.y) * 0.5f), 11.0f, EqAccent());
                ImGui::PopID();
            }
            ImGui::EndPopup();
        }
        ImGui::PopStyleVar(4);
        ImGui::PopStyleColor(5);
        ImGui::SetCursorScreenPos(ImVec2(p.x, p.y + h + 2.0f));
        return changed;
    }

    inline bool ColorRow(const char* icon, const char* label, float* rgba )
    {
        (void)icon;
        if (!EqPassFilter(label)) return false;
        static const unsigned char kPalette[][3] =
        {
            { 0, 212, 255 }, { 0, 210, 120 }, { 255, 60, 80 },
            { 255, 200, 50 }, { 10, 132, 255 }, { 255, 255, 255 }
        };
        const int paletteCount = IM_ARRAYSIZE(kPalette);

        const Palette pal = EqPal();
        const ImVec2 p = ImGui::GetCursorScreenPos();
        const float w = ImGui::GetContentRegionAvail().x;
        const float h = kRowH;

        char clean[128];
        EqStripId(label, clean, IM_ARRAYSIZE(clean));
        bool hovered = false;
        const bool pressed = EqPress(label, ImVec2(w, h), &hovered);

        const ImVec4 col(rgba[0] / 255.0f, rgba[1] / 255.0f, rgba[2] / 255.0f, 1.0f);
        const ImU32 swatch = ImGui::ColorConvertFloat4ToU32(col);
        ImDrawList* dl = ImGui::GetWindowDrawList();
        if (EqCardRowIndex()++ > 0) EqRowSeparator(dl, p, p + ImVec2(w, h));
        if (hovered) dl->AddRectFilled(p, p + ImVec2(w, h), EqColA(pal.text, 0.025f), portfolio::s(portfolio::control_round));
        EqDrawLabel(dl, ImVec2(p.x + portfolio::s(portfolio::box_pad_x), p.y + (h - portfolio::s(portfolio::row_label_font)) * 0.5f), clean, EqCol(hovered ? pal.text : pal.textDim), portfolio::s(portfolio::row_label_font));

        char hexBuf[16];
        ImFormatString(hexBuf, IM_ARRAYSIZE(hexBuf), "#%02X%02X%02X", (int)rgba[0], (int)rgba[1], (int)rgba[2]);
        const ImVec2 hs = EqLabelSize(hexBuf, 12.0f);
        EqDrawLabel(dl, ImVec2(p.x + w - 32.0f - hs.x, p.y + (h - 12.0f) * 0.5f), hexBuf, EqCol(pal.textFaint), 12.0f);
        dl->AddRectFilled(ImVec2(p.x + w - 24.0f, p.y + h * 0.5f - 8.0f), ImVec2(p.x + w - 8.0f, p.y + h * 0.5f + 8.0f), swatch, 6.0f);
        dl->AddRect(ImVec2(p.x + w - 24.0f, p.y + h * 0.5f - 8.0f), ImVec2(p.x + w - 8.0f, p.y + h * 0.5f + 8.0f), EqColA(pal.cardEdge, 1.4f), 6.0f);

        ImGui::SetCursorScreenPos(ImVec2(p.x, p.y + h + 2.0f));
        if (!pressed) return false;

        int best = 0;
        float bestDist = FLT_MAX;
        for (int i = 0; i < paletteCount; ++i)
        {
            const float dr = col.x - kPalette[i][0] / 255.0f;
            const float dg = col.y - kPalette[i][1] / 255.0f;
            const float db = col.z - kPalette[i][2] / 255.0f;
            const float d = dr * dr + dg * dg + db * db;
            if (d < bestDist) { bestDist = d; best = i; }
        }
        const int next = (best + 1) % paletteCount;
        rgba[0] = (float)kPalette[next][0];
        rgba[1] = (float)kPalette[next][1];
        rgba[2] = (float)kPalette[next][2];
        rgba[3] = 255.0f;
        EqMarkDirty();
        return true;
    }

    inline bool SegmentedRow(const char* label, int* current, const char* const* items, int count)
    {
        if (count <= 0) return false;
        *current = ImClamp(*current, 0, count - 1);
        if (!EqPassFilter(label)) return false;
        const Palette pal = EqPal();
        const ImVec2 p = ImGui::GetCursorScreenPos();
        const float w = ImGui::GetContentRegionAvail().x;
        const float h = kRowH;
        const float segH = 22.0f;
        const float segGap = 3.0f;
        const float segW = 74.0f;
        const float trackW = count * segW + (count - 1) * segGap;
        const float trackX = p.x + w - trackW;

        char clean[128];
        EqStripId(label, clean, IM_ARRAYSIZE(clean));

        bool rowHovered = false;
        const float rowPressW = ImMax(24.0f, trackX - 6.0f - p.x);
        EqPress(label, ImVec2(rowPressW, h), &rowHovered);
        ImDrawList* dl = ImGui::GetWindowDrawList();
        if (EqCardRowIndex()++ > 0) EqRowSeparator(dl, p, p + ImVec2(w, h));
        if (rowHovered) dl->AddRectFilled(p, p + ImVec2(rowPressW, h), EqColA(pal.text, 0.025f), portfolio::s(portfolio::control_round));
        EqDrawLabel(dl, ImVec2(p.x + portfolio::s(portfolio::box_pad_x), p.y + (h - portfolio::s(portfolio::row_label_font)) * 0.5f), clean, EqCol(rowHovered ? pal.text : pal.textDim), portfolio::s(portfolio::row_label_font));

        bool changed = false;
        const float ty = p.y + (h - segH) * 0.5f;
        dl->AddRectFilled(ImVec2(trackX - 3.0f, ty), ImVec2(trackX + trackW + 3.0f, ty + segH), EqColA(pal.switchOff, 0.8f), segH * 0.5f);
        for (int i = 0; i < count; ++i)
        {
            const ImVec2 smin(trackX + i * (segW + segGap), ty);
            const ImVec2 smax = smin + ImVec2(segW, segH);
            char sid[192];
            ImFormatString(sid, IM_ARRAYSIZE(sid), "%s##seg%d", label, i);
            ImGui::SetCursorScreenPos(smin);
            const bool sel = (*current == i);
            bool segHovered = false;
            const bool segPressed = EqPress(sid, ImVec2(segW, segH), &segHovered);
            if (segHovered && !sel)
                dl->AddRectFilled(smin, smax, EqCol(pal.hover), segH * 0.5f);
            if (segPressed && !sel) { *current = i; changed = true; EqMarkDirty(); }
            if (*current == i)
                dl->AddRectFilled(smin, smax, EqColA(pal.text, 0.14f), segH * 0.5f);
            const ImVec2 ts = EqLabelSize(items[i], 11.5f);
            EqDrawLabel(dl, ImVec2(smin.x + (segW - ts.x) * 0.5f, ty + (segH - 11.5f) * 0.5f),
                        items[i], EqCol(sel ? pal.text : pal.textDim), 11.5f);
        }
        ImGui::SetCursorScreenPos(ImVec2(p.x, p.y + h + 2.0f));
        return changed;
    }

    inline bool KeybindRow(const char* label, int* bind, const char* const* binds, int count)
    {
        if (count <= 0) return false;
        *bind = ImClamp(*bind, 0, count - 1);
        if (!EqPassFilter(label)) return false;
        const Palette pal = EqPal();
        const ImVec2 p = ImGui::GetCursorScreenPos();
        const float w = ImGui::GetContentRegionAvail().x;
        const float h = kRowH;
        const float chipH = 20.0f;
        const char* key = binds[*bind];
        const ImVec2 ks = EqLabelSize(key, 12.0f);
        const float chipW = ks.x + 22.0f;
        const ImVec2 c0(p.x + w - chipW, p.y + (h - chipH) * 0.5f);
        const ImVec2 c1(p.x + w, c0.y + chipH);

        char clean[128];
        EqStripId(label, clean, IM_ARRAYSIZE(clean));
        bool hovered = false;
        const bool pressed = EqPress(label, ImVec2(w, h), &hovered);
        ImDrawList* dl = ImGui::GetWindowDrawList();
        if (EqCardRowIndex()++ > 0) EqRowSeparator(dl, p, p + ImVec2(w, h));
        if (hovered) dl->AddRectFilled(p, p + ImVec2(w, h), EqColA(pal.text, 0.025f), portfolio::s(portfolio::control_round));
        EqDrawLabel(dl, ImVec2(p.x + portfolio::s(portfolio::box_pad_x), p.y + (h - portfolio::s(portfolio::row_label_font)) * 0.5f), clean, EqCol(hovered ? pal.text : pal.textDim), portfolio::s(portfolio::row_label_font));
        dl->AddRectFilled(c0, c1, EqColA(pal.text, hovered ? 0.14f : 0.08f), 7.0f);
        dl->AddRect(c0 + ImVec2(0.5f, 0.5f), c1 - ImVec2(0.5f, 0.5f), EqCol(pal.cardEdge), 7.0f);
        EqDrawLabel(dl, ImVec2(c0.x + 11.0f, p.y + (h - 12.0f) * 0.5f), key, EqCol(pal.text), 12.0f);

        ImGui::SetCursorScreenPos(ImVec2(p.x, p.y + h + 2.0f));
        if (pressed) { *bind = (*bind + 1) % count; EqMarkDirty(); return true; }
        return false;
    }

    struct TabDef { const char* glyph; const char* label; int tab; };
    static const TabDef kTabs[] =
    {
        { ICON_FA_CROSSHAIRS, "AimBot",  1 },
        { ICON_FA_USERS,      "Players", 0 },
        { ICON_FA_GLOBE,      "World",   2 },
        { ICON_FA_TH_LIST,    "Skins",   3 },
        { ICON_FA_SLIDERS_H,  "Misc",    4 },
        { ICON_FA_COG,        "Config",  5 },
    };
    static const int kTabCount = IM_ARRAYSIZE(kTabs);

    inline void EqClampMenuPos(MenuState& st, const ImVec2& size, const ImGuiIO& io)
    {
        const float dw = ImMax(io.DisplaySize.x, 1.0f);
        const float dh = ImMax(io.DisplaySize.y, 1.0f);
        const float xMin = 120.0f - size.x, xMax = dw - 120.0f;
        const float yMin = 40.0f, yMax = dh - 40.0f;
        st.WinPos.x = ImClamp(st.WinPos.x, ImMin(xMin, xMax), ImMax(xMin, xMax));
        st.WinPos.y = ImClamp(st.WinPos.y, ImMin(yMin, yMax), ImMax(yMin, yMax));
    }

    inline void EqDrawShellBase(ImDrawList* dl, const ImVec2& p0, const ImVec2& p1, float R, const Palette& pal, ImTextureID backdrop)
    {
        if (backdrop != nullptr)
        {
            dl->AddImageRounded(backdrop, p0, p1, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), IM_COL32(255, 255, 255, 255), R);
        }
        else
        {
            dl->AddRectFilled(p0, p1, EqCol(pal.base), R);
        }
        dl->AddRectFilled(p0, p1, EqCol(pal.scrim), R);
    }

    inline void EqDrawSidebar(MenuState& st, const ImVec2& s0, const ImVec2& s1)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        const Palette pal = EqPal();

        ImGui::SetCursorScreenPos(s0);
        ImGui::BeginChild("##ethnir_nav", s1 - s0, false, ImGuiWindowFlags_NoBackground);
        dl = ImGui::GetWindowDrawList();
        dl->AddRectFilled(s0, s1, EqCol(pal.side), portfolio::s(portfolio::shell_round));
        dl->AddRect(s0 + ImVec2(0.5f, 0.5f), s1 - ImVec2(0.5f, 0.5f), EqCol(pal.sideEdge), portfolio::s(portfolio::shell_round));

        const float itemTop = s0.y + portfolio::s(portfolio::sidebar_tabs_y);
        const float itemPitch = portfolio::s(portfolio::sidebar_tab_h + portfolio::sidebar_tabs_gap);
        const float itemH = portfolio::s(portfolio::sidebar_tab_h);

        // activeRow removed: the per-tab gradient replaced the sliding highlight
for (int i = 0; i < kTabCount; ++i)
        {

            const ImVec2 tmin(s0.x, itemTop + i * itemPitch);
            const ImVec2 tmax(tmin.x + portfolio::s(portfolio::sidebar_tab_w), tmin.y + itemH);
            ImGui::SetCursorScreenPos(tmin);
            char id[32];
            ImFormatString(id, IM_ARRAYSIZE(id), "##ethnir_tab%d", i);
            bool hov = false;
            const bool pressed = EqPress(id, tmax - tmin, &hov);
            const bool act = (kTabs[i].tab == st.ActiveTab);
            if (pressed && !act) { st.ActiveTab = kTabs[i].tab; st.Fade = 0.0f; }

            char hid[36];
            ImFormatString(hid, IM_ARRAYSIZE(hid), "##ethnir_hot%d", i);
            const float dt = ImGui::GetIO().DeltaTime;
            const float sel  = EqAnim(hid, act ? 1.0f : 0.0f, 22.0f, dt, act ? 1.0f : 0.0f);
            const float hotA = EqAnim(hid + 1, (hov && !act) ? 1.0f : 0.0f, 24.0f, dt, 0.0f);

            if (sel > 0.01f)
            {
                const ImU32 gs = EqAccentA(0.28f * sel);
                const ImU32 ge = EqAccentA(0.0f);
                dl->AddRectFilledMultiColor(tmin, tmax, gs, ge, ge, gs, portfolio::s(portfolio::shell_round));
                dl->AddRectFilled(tmin, ImVec2(tmin.x + portfolio::s(3.f), tmax.y), EqAccentA(sel));
            }
            if (!act && hotA > 0.01f)
            {
                const ImU32 hs = EqCol(portfolio::fg(0.068f * hotA));
                const ImU32 he = EqCol(portfolio::fg(0.0f));
                dl->AddRectFilledMultiColor(tmin, tmax, hs, he, he, hs, portfolio::s(portfolio::shell_round));
            }

            const float iconSize = portfolio::s(portfolio::sidebar_tab_icon);
            const float iconX = tmin.x + portfolio::s(portfolio::sidebar_tab_icon_x);
            const float iconA = 0.55f + 0.45f * ImMax(sel, hotA);
            EqDrawGlyph(dl, kTabs[i].glyph, ImVec2(iconX + iconSize * 0.5f, (tmin.y + tmax.y) * 0.5f),
                        iconSize, EqCol(portfolio::fg(iconA)));
            const float textSize = portfolio::s(portfolio::row_label_font);
            EqDrawLabel(dl, ImVec2(iconX + iconSize + portfolio::s(portfolio::sidebar_tab_text_gap),
                                   tmin.y + (itemH - textSize) * 0.5f),
                        kTabs[i].label, EqCol(portfolio::fg(0.6f + 0.4f * ImMax(sel, hotA))), textSize);
        }

        const ImVec2 chipC(s0.x + portfolio::s(27.f), s1.y - portfolio::s(27.f));
        ImGui::SetCursorScreenPos(chipC - ImVec2(portfolio::s(13), portfolio::s(13)));
        bool chipHov = false;
        const bool chipPressed = EqPress("##ethnir_help", ImVec2(portfolio::s(26), portfolio::s(26)), &chipHov);
        if (chipPressed)
        {
            st.HelpOpen = !st.HelpOpen;
            st.HelpChipPressedFrame = true;
        }
        if (chipHov || st.HelpOpen) dl->AddCircleFilled(chipC, portfolio::s(14.f), EqColA(pal.text, 0.08f), 24);
        dl->AddCircle(chipC, portfolio::s(13.f), EqColA(pal.text, st.HelpOpen ? 0.22f : 0.14f), 24, 1.0f);
        EqDrawLabel(dl, ImVec2(chipC.x - portfolio::s(4.f), chipC.y - portfolio::s(7.f)), "?", EqCol(st.HelpOpen ? pal.text : pal.textDim), portfolio::s(13.f));
        EqDrawLabel(dl, ImVec2(chipC.x + portfolio::s(20.f), chipC.y - portfolio::s(5.5f)), st.SubtitleText, EqCol(pal.textFaint), portfolio::s(10.f));

        ImGui::EndChild();
    }

    inline void EqDrawSettingsPanel(MenuState& st)
    {
        const Palette pal = EqPal();
        const ImGuiIO& io = ImGui::GetIO();
        const ImVec2 size(portfolio::s(258.f), 0.0f);
        if (!st.PanelPosInit)
        {
            const float gap = portfolio::s(14.f);
            float px = st.WinPos.x + kWinW + gap;
            if (px + size.x + 12.0f > io.DisplaySize.x)
                px = st.WinPos.x - size.x - gap;
            st.PanelPos = ImVec2(ImClamp(px, 12.0f, ImMax(12.0f, io.DisplaySize.x - size.x - 12.0f)),
                                 st.WinPos.y + portfolio::s(26.f));
            st.PanelPosInit = true;
        }
        const float panelMaxX = io.DisplaySize.x - size.x - 12.0f;
        st.PanelPos.x = (panelMaxX > 12.0f) ? ImClamp(st.PanelPos.x, 12.0f, panelMaxX) : 12.0f;
        st.PanelPos.y = ImClamp(st.PanelPos.y, 12.0f, ImMax(12.0f, io.DisplaySize.y - 60.0f));

        ImGui::SetNextWindowPos(st.PanelPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(size, ImGuiCond_Always);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
        ImGui::Begin("##ethnir_settings", nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse |
                     ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBackground |
                     ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize |
                     ImGuiWindowFlags_NoBringToFrontOnFocus);
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);

        const ImVec2 p0 = ImGui::GetWindowPos();
        const ImVec2 p1 = ImVec2(p0.x + ImGui::GetWindowSize().x, p0.y + ImGui::GetWindowSize().y);
        ImDrawList* dl = ImGui::GetWindowDrawList();
        const float R = portfolio::s(portfolio::shell_round);

        portfolio::DrawLiquidGlassPanel(dl, p0, p1, R, st.Backdrop);

        ImGui::SetCursorScreenPos({ p0.x + portfolio::s(16.f), p0.y + portfolio::s(15.f) });
        EqDrawTracked(dl, EqTextFont(), portfolio::s(12.f), ImVec2(p0.x + portfolio::s(16.f), p0.y + portfolio::s(15.f)), EqCol(pal.text), "QUICK SETTINGS", 0.8f);

        const ImVec2 dragMin(p0.x + portfolio::s(12.f), p0.y + portfolio::s(8.f));
        const ImVec2 dragMax(p1.x - portfolio::s(12.f), p0.y + portfolio::s(38.f));
        ImGui::SetCursorScreenPos(dragMin);
        ImGui::InvisibleButton("##panel_drag", dragMax - dragMin);
        if (!st.PanelDragging && ImGui::IsItemActive() && ImGui::IsMouseDragging(0))
            st.PanelDragging = true;
        if (st.PanelDragging)
        {
            st.InputActive = true;
            if (!io.MouseDown[0]) st.PanelDragging = false;
            else { st.PanelPos += io.MouseDelta; ImGui::SetWindowPos(st.PanelPos); }
        }
        ImGui::SetCursorScreenPos(ImVec2(p0.x + portfolio::s(14.f), p0.y + portfolio::s(46.f)));

        EqFilterOn() = false;
        BeginGroupCard("eth_panel_rows");
        {
            static const char* themes[] = { "Dark", "Light" };
            int theme = st.Dark ? 0 : 1;
            if (SegmentedRow("Theme", &theme, themes, IM_ARRAYSIZE(themes)))
                st.Dark = (theme == 0);

            float animPct = st.AnimSpeed * 100.0f;
            if (RowSlider(nullptr, "Animation", &animPct, 50.0f, 200.0f, "%.0f%%"))
                st.AnimSpeed = animPct / 100.0f;

            ImDrawList* cdl = ImGui::GetWindowDrawList();
            const float w = ImGui::GetContentRegionAvail().x;
            const float h = kRowH;
            const ImVec2 p = ImGui::GetCursorScreenPos();
            if (EqCardRowIndex()++ > 0) EqRowSeparator(cdl, p, p + ImVec2(w, h));
            EqDrawLabel(cdl, ImVec2(p.x + portfolio::s(3.f), p.y + (h - portfolio::s(13.f)) * 0.5f), "Accent color", EqCol(pal.textDim), portfolio::s(13.f));
            const float sw = portfolio::s(16.f);
            const ImVec2 c(p.x + w - sw - portfolio::s(10.f), p.y + h * 0.5f);
            char aid[32];
            ImFormatString(aid, IM_ARRAYSIZE(aid), "##accent");
            ImGui::SetCursorScreenPos(c - ImVec2(portfolio::s(9), portfolio::s(9)));
            bool hov = false;
            EqPress(aid, ImVec2(portfolio::s(18), portfolio::s(18)), &hov);
            cdl->AddCircleFilled(c, hov ? portfolio::s(7.f) : portfolio::s(6.f), EqAccent(), 24);
            if (st.AccentIndex == 0)
                cdl->AddCircle(c, portfolio::s(9.f), EqAccentA(0.9f), 24, portfolio::s(1.6f));
            ImGui::SetCursorScreenPos(ImVec2(p.x, p.y + h + portfolio::s(2.f)));

            KeybindRow("Menu bind", &st.MenuBind, kMenuBinds, kMenuBindCount);
        }
        EndGroupCard();
        EqFilterOn() = true;

        ImGui::End();
    }

    inline bool EqDrawHelpCard(MenuState& st, const ImVec2& anchor, const ImVec2& sidebarMin)
    {
        const Palette pal = EqPal();
        const ImGuiIO& io = ImGui::GetIO();
        const ImVec2 size(292.0f, 172.0f);
        ImVec2 pos(sidebarMin.x, anchor.y - size.y - 12.0f);
        if (pos.y < sidebarMin.y) pos.y = sidebarMin.y;
        pos.x = ImClamp(pos.x, 8.0f, ImMax(8.0f, io.DisplaySize.x - size.x - 8.0f));
        pos.y = ImClamp(pos.y, 8.0f, ImMax(8.0f, io.DisplaySize.y - size.y - 8.0f));

        ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(size, ImGuiCond_Always);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16.0f, 14.0f));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
        ImGui::Begin("##ethnir_help", nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                     ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings |
                     ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoBringToFrontOnFocus);
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();

        const ImVec2 p0 = ImGui::GetWindowPos();
        const ImVec2 p1 = p0 + ImGui::GetWindowSize();
        ImDrawList* dl = ImGui::GetWindowDrawList();
        const float R = 18.0f;
        dl->AddShadowRect(p0, p1, IM_COL32(0, 0, 0, 110), 24.0f, ImVec2(0, 8), 0, R);
        dl->AddRectFilled(p0, p1, EqCol(pal.popupBg), R);
        dl->AddRect(p0 + ImVec2(0.5f, 0.5f), p1 - ImVec2(0.5f, 0.5f), EqCol(pal.cardEdge), R);

        EqDrawTracked(dl, EqTitleFont(), 14.0f, ImVec2(p0.x + 16.0f, p0.y + 14.0f), EqCol(pal.text), st.TitleText, 1.4f);
        EqDrawTracked(dl, EqTextFont(), 9.5f, ImVec2(p0.x + 18.0f, p0.y + 34.0f), EqColA(EqAccentVec(), 0.95f), st.SubtitleText, 1.0f);

        static const char* lines[] =
        {
            "Drag the top bar to move the window.",
            "Search filters the active page instantly.",
            "The gear opens quick settings.",
            "Changes save automatically after a beat.",
        };
        float y = p0.y + 58.0f;
        for (int i = 0; i < IM_ARRAYSIZE(lines); ++i)
        {
            dl->AddCircleFilled(ImVec2(p0.x + 20.0f, y + 6.0f), 2.2f, EqColA(EqAccentVec(), 0.9f), 12);
            EqDrawLabel(dl, ImVec2(p0.x + 32.0f, y), lines[i], EqCol(pal.textDim), 11.5f);
            y += 24.0f;
        }
        const bool hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows);
        ImGui::End();
        return hovered;
    }

    inline void EqRender(MenuState& st)
    {
        EqState() = &st;
        st.HeaderPressed = -1;
        st.TrafficPressed = -1;
        st.FrameRowCount = 0;
        st.InputActive = false;
        EqCols().Active = false;

        if (!st.Open && !st.Closing)
        {
            st.WasOpen = false;
            st.WinPosInit = false;
            st.Appear = 0.0f;
            st.Fade = 1.0f;
            EqFilterOn() = true;
            return;
        }

        ImGuiIO& io = ImGui::GetIO();
        const float dt = io.DeltaTime;
        const Palette pal = EqPal();

        if (!st.WasOpen) { st.Appear = 0.0f; st.Fade = 0.0f; st.LastTab = st.ActiveTab; }
        st.WasOpen = true;
        bool finishClose = false;
        if (st.Closing)
        {

            st.Appear = ImMax(0.0f, st.Appear - dt / 0.16f);
            if (st.Appear <= 0.0f) finishClose = true;
        }
        else
        {
            st.Appear = ImMin(1.0f, st.Appear + dt / 0.20f);
        }
        const float appear = EqEase(st.Appear);

        if (st.LastTab != st.ActiveTab) { st.LastTab = st.ActiveTab; st.Fade = 0.0f; }
        st.Fade = ImMin(1.0f, st.Fade + dt / 0.16f);
        const float fade = EqEase(st.Fade);
        if (st.SaveFlash > 0.0f) st.SaveFlash = ImMax(0.0f, st.SaveFlash - dt);

        st.BackdropPhase += dt * 0.9f;
        if (st.BackdropPhase > 6.2831853f) st.BackdropPhase -= 6.2831853f;

        ImVec2 winSize(kWinW, kWinH);
        winSize.x = ImMin(winSize.x, io.DisplaySize.x - 12.0f);
        winSize.y = ImMin(winSize.y, io.DisplaySize.y - 12.0f);
        winSize.x = ImMax(winSize.x, 420.0f);
        winSize.y = ImMax(winSize.y, 300.0f);
        if (!st.WinPosInit)
        {
            const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            st.WinPos = ImVec2(center.x - winSize.x * 0.5f, center.y - winSize.y * 0.5f);
            st.WinPosInit = true;
        }
        EqClampMenuPos(st, winSize, io);

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, appear);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
        ImGui::SetNextWindowPos(st.WinPos + ImVec2(0.0f, (1.0f - appear) * 22.0f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(winSize, ImGuiCond_Always);
        ImGui::Begin("##ethnir_shell", nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoSavedSettings |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
        ImGui::PopStyleColor();
        ImGui::PopStyleVar(2);

        ImDrawList* dl = ImGui::GetWindowDrawList();
        const ImVec2 p0 = ImGui::GetWindowPos();
        const ImVec2 p1 = p0 + ImGui::GetWindowSize();

        {
            const ImVec2 mid = (p0 + p1) * 0.5f;
            const float sc = 0.94f + 0.06f * appear;
            const ImVec2 b0 = mid + (p0 - mid) * sc;
            const ImVec2 b1 = mid + (p1 - mid) * sc;

            portfolio::DrawLiquidGlassPanel(dl, b0, b1, kRadius, st.Backdrop);
        }

        const ImVec2 brandPos(p0.x + kPad + 6.0f, p0.y + kPad + 3.0f);
        EqDrawTracked(dl, EqTitleFont(), 14.0f, brandPos, EqCol(pal.text), st.TitleText, 1.6f);
        {
            const float bw = EqTrackedWidth(EqTitleFont(), 14.0f, st.TitleText, 1.6f);
            dl->AddRectFilled(ImVec2(brandPos.x + bw + 8.0f, brandPos.y + 6.0f),
                              ImVec2(brandPos.x + bw + 8.0f + 6.0f, brandPos.y + 9.0f), EqAccent(), 1.5f);
        }

        st.HeaderControl = false;
        const ImVec2 h0(p0.x + kPad + kSideW + 16.0f, p0.y + kPad + 2.0f);
        const ImVec2 h1(p1.x - kPad, h0.y + 30.0f);
        const float contentW = h1.x - h0.x;

        if (!st.Closing)
        {

            {
                const float saveW = portfolio::s(portfolio::topbar_save_w);
                const float saveH = portfolio::s(portfolio::topbar_row_h);
                const ImVec2 bmin = { h0.x, h0.y + portfolio::s(portfolio::topbar_row_y) };
                const ImVec2 bmax = { bmin.x + saveW, bmin.y + saveH };
                const float R = portfolio::s(14.f);

                ImGui::SetCursorScreenPos(bmin);
                bool saveHov = false, saveHeld = false;
                const bool pressed = EqPress("##ethnir_save", bmax - bmin, &saveHov, &saveHeld);
                if (pressed)
                {
                    st.HeaderPressed = 0;
                    st.SaveFlash = 1.6f;
                    st.Dirty = false;
                    st.DirtyTimer = 0.0f;
                }

                portfolio::DrawLiquidGlassButton(dl, bmin, bmax, R,
                    st.SaveFlash > 0.0f ? ICON_FA_CHECK : ICON_FA_SAVE,
                    st.SaveFlash > 0.0f ? "Saved" : "Save",
                    EqIconFont(), portfolio::s(16.f),
                    EqTextFont(), portfolio::s(13.f),
                    saveHov, saveHeld, false);
            }

            char fpsbuf[24];
            ImFormatString(fpsbuf, IM_ARRAYSIZE(fpsbuf), "%d FPS", (int)(io.Framerate + 0.5f));
            const ImVec2 fts = EqLabelSize(fpsbuf, 11.0f);
            EqDrawLabel(dl, ImVec2(h1.x - portfolio::s(30.f) - portfolio::s(6.f) - portfolio::s(36.f) - portfolio::s(12.f) - fts.x, h0.y + portfolio::s(15.f) - fts.y * 0.5f),
                        fpsbuf, EqCol(pal.textFaint), 11.0f);

            const float iconSize = portfolio::s(portfolio::topbar_icon_size);
            const float btnSize = portfolio::s(36.f);
            for (int i = 0; i < 2; ++i)
            {
                const float bx = h1.x - portfolio::s(30.f) - i * btnSize;
                const ImVec2 bmin(bx, h0.y + portfolio::s(portfolio::topbar_row_y) + (portfolio::s(portfolio::topbar_row_h) - btnSize) * 0.5f);
                const ImVec2 bmax(bx + btnSize, bmin.y + btnSize);
                const float R = portfolio::s(12.f);

                ImGui::SetCursorScreenPos(bmin);
                bool hov = false, held = false;
                const bool pressed = EqPress(i == 0 ? "##ethnir_gear" : "##ethnir_min", ImVec2(btnSize, btnSize), &hov, &held);
                const bool on = (i == 0 && st.ShowSettingsPanel);
                if (pressed)
                {
                    if (i == 0) st.ShowSettingsPanel = !st.ShowSettingsPanel;
                    else st.Closing = true;
                }

                portfolio::DrawLiquidGlassButton(dl, bmin, bmax, R,
                    i == 0 ? ICON_FA_COG : ICON_FA_MINUS,
                    nullptr,
                    EqIconFont(), iconSize,
                    nullptr, 0.f,
                    hov, held, on);
            }

            const float searchW = portfolio::s(portfolio::topbar_search_w);
            const float searchH = portfolio::s(portfolio::topbar_row_h);
            const float right = h1.x - portfolio::s(26.f) - btnSize - portfolio::s(12.f);
            const ImVec2 fmin(right - searchW, h0.y + portfolio::s(portfolio::topbar_row_y));
            const ImVec2 fmax(right, fmin.y + searchH);
            const float searchR = portfolio::s(14.f);

            if (io.MouseClicked[0])
            {
                if (ImGui::IsMouseHoveringRect(fmin, fmax)) st.HeaderControl = true;
            }

            const bool searchHov = ImGui::IsMouseHoveringRect(fmin, fmax);
            portfolio::DrawLiquidGlassSearchField(dl, fmin, fmax, searchR, false, searchHov);
            EqDrawGlyph(dl, ICON_FA_SEARCH,
                        ImVec2(fmin.x + portfolio::s(22.f), (fmin.y + fmax.y) * 0.5f),
                        portfolio::s(11.f), EqColA(pal.textFaint, 0.95f));

            char hint[64];
            ImFormatString(hint, IM_ARRAYSIZE(hint), "explore %d functions...", st.LastRowCount);
            ImGui::SetCursorScreenPos({ fmin.x + portfolio::s(40.f), fmin.y + (searchH - ImGui::GetFrameHeight()) * 0.5f });
            ImGui::PushItemWidth(searchW - portfolio::s(40.f) - portfolio::s(30.f));
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0.f, 0.f });
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_Text, pal.text);
            if (EqTextFont()) ImGui::PushFont(EqTextFont());
            ImGui::InputTextWithHint("##ethnir_search", hint, st.Search, IM_ARRAYSIZE(st.Search));
            const bool searchActive = ImGui::IsItemActive();
            if (EqTextFont()) ImGui::PopFont();
            ImGui::PopStyleColor(4);
            ImGui::PopStyleVar();
            ImGui::PopItemWidth();

            if (searchActive)
                st.HeaderControl = true;

            if (st.Search[0] != 0)
            {
                const float clearSize = portfolio::s(24.f);
                const ImVec2 cmin(fmax.x - clearSize - portfolio::s(12.f), fmin.y + (searchH - clearSize) * 0.5f);
                ImGui::SetCursorScreenPos(cmin);
                bool chov = false;
                if (EqPress("##ethnir_sclear", ImVec2(clearSize, clearSize), &chov))
                    st.Search[0] = 0;
                EqDrawGlyph(dl, ICON_FA_TIMES, cmin + ImVec2(clearSize * 0.5f, clearSize * 0.5f), clearSize * 0.5f,
                    EqColA(pal.textDim, chov ? 1.0f : 0.8f));
            }
        }

        if (st.Dragging && !io.MouseDown[0]) st.Dragging = false;
        if (!st.Dragging && io.MouseClicked[0] && ImGui::IsMouseHoveringRect(h0, h1) && !st.HeaderControl)
        {
            st.Dragging = true;
            st.DragRef  = io.MousePos;
        }
        if (st.Dragging)
        {

            const ImVec2 move = io.MousePos - st.DragRef;
            if (move.x != 0.0f || move.y != 0.0f)
            {
                st.WinPos += ImVec2(move.x * st.DragScaleX, move.y * st.DragScaleY);
                EqClampMenuPos(st, winSize, io);
                ImGui::SetWindowPos(st.WinPos);
            }
            st.DragRef = io.MousePos;
        }

        const ImVec2 s0(p0.x + kPad, p0.y + kPad + kBrandH);
        const ImVec2 s1(p0.x + kPad + kSideW, p1.y - kPad);
        EqDrawSidebar(st, s0, s1);

        const ImVec2 c0(h0.x, h1.y + 14.0f);
        const ImVec2 c1(p1.x - kPad, p1.y - kPad);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, fade);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2, 2));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 4.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarRounding, 4.0f);
        ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, ImVec4(1, 1, 1, 0.20f));
        ImGui::SetCursorScreenPos(c0);
        ImGui::BeginChild("##ethnir_content", c1 - c0, false, ImGuiWindowFlags_NoBackground);
        ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2(0.0f, (1.0f - fade) * 8.0f));

        const bool filtering = st.Search[0] != 0;
        const bool tabFilters = st.ActiveTab >= 0 && st.ActiveTab <= 3;
        if (filtering && tabFilters && !st.LastSearchHit)
        {
            ImDrawList* cdl = ImGui::GetWindowDrawList();
            const ImVec2 avail = ImGui::GetContentRegionAvail();
            const ImVec2 origin = ImGui::GetCursorScreenPos();
            const ImVec2 card(ImMin(avail.x, 340.0f), 104.0f);
            const ImVec2 cmin(origin.x + (avail.x - card.x) * 0.5f, origin.y + (avail.y - card.y) * 0.5f);
            cdl->AddRectFilled(cmin, cmin + card, EqCol(pal.cardBg), 14.0f);
            cdl->AddRect(cmin + ImVec2(0.5f, 0.5f), cmin + card - ImVec2(0.5f, 0.5f), EqCol(pal.cardEdge), 14.0f);
            EqDrawGlyph(cdl, ICON_FA_SEARCH, ImVec2(cmin.x + card.x * 0.5f, cmin.y + 32.0f), 14.0f, EqColA(pal.textFaint, 0.95f));
            const char* line1 = "NO MATCHES";
            const ImVec2 t1 = EqLabelSize(line1, 13.0f);
            EqDrawLabel(cdl, ImVec2(cmin.x + (card.x - t1.x) * 0.5f, cmin.y + 50.0f), line1, EqCol(pal.text), 13.0f);
            const char* line2 = "Try a different keyword.";
            const ImVec2 t2 = EqLabelSize(line2, 11.0f);
            EqDrawLabel(cdl, ImVec2(cmin.x + (card.x - t2.x) * 0.5f, cmin.y + 72.0f), line2, EqCol(pal.textFaint), 11.0f);
        }
        else if (st.DrawTab)
        {
            st.DrawTab(st.ActiveTab);
            EqEndColumns();
        }
        ImGui::EndChild();
        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(5);

        if (tabFilters) st.LastSearchHit = st.FrameSearchHit;
        st.FrameSearchHit = false;
        st.LastRowCount = st.FrameRowCount;

        ImGui::End();

        if (st.ShowSettingsPanel && !st.Closing) EqDrawSettingsPanel(st);
        if (st.HelpOpen && !st.Closing)
        {
            const bool helpHovered = EqDrawHelpCard(st, ImVec2(s0.x + 27.0f, s1.y - 27.0f), s0);
            if (ImGui::IsMouseClicked(0) && !st.HelpChipPressedFrame && !helpHovered)
                st.HelpOpen = false;
        }
        st.HelpChipPressedFrame = false;
        EqFilterOn() = true;

        if (st.AutoSave && st.OnSave && !st.Closing)
        {
            if (st.Dirty && !st.InputActive)
            {
                st.DirtyTimer += dt;
                if (st.DirtyTimer >= st.SaveDebounce)
                {
                    st.Dirty = false;
                    st.DirtyTimer = 0.0f;
                    st.OnSave();
                    st.SaveFlash = 1.6f;
                }
            }
            else
            {
                st.DirtyTimer = 0.0f;
            }
        }

        if (finishClose)
        {
            st.Closing = false;
            st.TrafficPressed = 0;
            st.WasOpen = false;
            st.WinPosInit = false;
        }

        ImGui::PopStyleVar();
    }
}
