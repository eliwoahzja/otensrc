#pragma once
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_settings.h"
#include "Icon.h"
#include <functional>
#include <cmath>
#include <cstring>
#include <cctype>

namespace equinox
{
    struct MenuState
    {
        bool        Open         = true;
        int         ActiveTab    = 0;
        char        Search[64]   = "";
        const char* TitleText    = "CODM: Garnoe | v1.0.87";
        const char* SubtitleText = "White Crowns Official";
        ImTextureID Backdrop     = nullptr;
        int         HeaderPressed  = -1;
        int         TrafficPressed = -1;
        std::function<void(int tab)> DrawTab;
        int         LastTab = -1;
        float       Fade    = 1.0f;
    };

    constexpr ImVec4 kGlass(0.10f, 0.10f, 0.13f, 0.62f);
    constexpr ImVec4 kSidebar(0.05f, 0.05f, 0.08f, 0.42f);
    constexpr ImVec4 kCard(1.00f, 1.00f, 1.00f, 0.055f);
    constexpr ImVec4 kCardHover(1.00f, 1.00f, 1.00f, 0.100f);
    constexpr ImVec4 kEdge(1.00f, 1.00f, 1.00f, 0.100f);
    constexpr ImVec4 kSwitchOff(1.00f, 1.00f, 1.00f, 0.220f);
    constexpr ImVec4 kTextHi(0.93f, 0.94f, 0.97f, 1.00f);
    constexpr ImVec4 kTextLo(0.66f, 0.69f, 0.76f, 1.00f);
    constexpr ImVec4 kRed(0.95f, 0.30f, 0.26f, 1.00f);
    constexpr ImVec4 kYellow(0.98f, 0.73f, 0.20f, 1.00f);
    constexpr ImVec4 kGreen(0.30f, 0.80f, 0.36f, 1.00f);

    inline MenuState*& EqState() { static MenuState* s = nullptr; return s; }
    inline ImFont* EqTextFont() { if (font::inter_semibold) return font::inter_semibold; if (F50) return F50; return ImGui::GetFont(); }
    inline ImFont* EqTitleFont() { if (F50) return F50; return EqTextFont(); }
    inline ImFont* EqIconFont() { if (F107) return F107; return EqTextFont(); }
    inline float EqDamp(float a, float b, float k, float dt) { return b + (a - b) * std::exp(-k * dt); }
    inline ImVec4 EqMix(const ImVec4& a, const ImVec4& b, float t) { return ImVec4(ImLerp(a.x, b.x, t), ImLerp(a.y, b.y, t), ImLerp(a.z, b.z, t), ImLerp(a.w, b.w, t)); }
    inline ImU32 EqCol(const ImVec4& c) { return ImGui::GetColorU32(c); }
    inline ImU32 EqAccent() { return main_runtime_theme::GetAccentU32(); }
    inline ImVec4 EqAccentVec() { return ImGui::ColorConvertU32ToFloat4(EqAccent()); }

    inline bool EqPassFilter(const char* label)
    {
        MenuState* st = EqState();
        if (!st || st->Search[0] == 0) return true;
        char a[128], b[128];
        int i = 0; for (; i < 127 && label[i]; ++i) a[i] = (char)toupper((unsigned char)label[i]); a[i] = 0;
        i = 0; for (; i < 127 && st->Search[i]; ++i) b[i] = (char)toupper((unsigned char)st->Search[i]); b[i] = 0;
        return strstr(a, b) != nullptr;
    }

    inline void EqDrawGlyph(ImDrawList* dl, const char* glyph, ImVec2 center, float size, ImU32 col)
    {
        if (!glyph) return;
        ImFont* f = EqIconFont();
        ImVec2 ts = f->CalcTextSizeA(size, FLT_MAX, 0.0f, glyph);
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

    inline bool EqToggleSwitch(const char* id, bool* v, ImVec2 center)
    {
        ImGuiWindow* w = ImGui::GetCurrentWindow();
        ImGuiStorage* stg = ImGui::GetStateStorage();
        const ImGuiID sid = w->GetID(id);
        const ImVec2 size(34.0f, 18.0f);
        const ImVec2 mn(center.x - size.x * 0.5f, center.y - size.y * 0.5f);
        ImGui::SetCursorScreenPos(mn);
        ImGui::InvisibleButton(id, size);
        const bool clicked = ImGui::IsItemClicked();
        if (clicked) *v = !*v;
        float t = EqDamp(stg->GetFloat(sid, *v ? 1.0f : 0.0f), *v ? 1.0f : 0.0f, 14.0f, ImGui::GetIO().DeltaTime);
        stg->SetFloat(sid, t);
        ImDrawList* dl = ImGui::GetWindowDrawList();
        dl->AddRectFilled(mn, mn + size, EqCol(EqMix(kSwitchOff, EqAccentVec(), t)), size.y * 0.5f);
        dl->AddCircleFilled(ImVec2(ImLerp(mn.x + size.y * 0.5f, mn.x + size.x - size.y * 0.5f, t), center.y), 7.0f, IM_COL32(247, 250, 255, 255), 16);
        return clicked;
    }

    inline void SectionLabel(const char* text)
    {
        ImVec2 p = ImGui::GetCursorScreenPos();
        EqDrawLabel(ImGui::GetWindowDrawList(), p, text, EqCol(kTextLo), 11.0f);
        ImGui::SetCursorScreenPos(ImVec2(p.x, p.y + 17.0f));
    }

    inline void BeginGroupCard(const char* id)
    {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, kCard);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 8.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 4.0f));
        ImGui::BeginChild(id, ImVec2(ImGui::GetContentRegionAvail().x, 0.0f), false,
                          ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar);
    }
    
    inline void EndGroupCard()
    {
        ImGui::EndChild();
        ImGui::PopStyleVar(4);
        ImGui::PopStyleColor();
        ImGui::Dummy(ImVec2(0, 2));
    }

    inline bool RowToggle(const char* icon, const char* label, bool* v)
    {
        if (!EqPassFilter(label)) return false;
        ImVec2 p = ImGui::GetCursorScreenPos();
        const float w = ImGui::GetContentRegionAvail().x;
        const float h = 28.0f;
        ImGui::InvisibleButton(label, ImVec2(w, h));
        const bool hovered = ImGui::IsItemHovered();
        ImDrawList* dl = ImGui::GetWindowDrawList();
        if (hovered) dl->AddRectFilled(p + ImVec2(2, 1), p + ImVec2(w - 2, h - 1), EqCol(kCardHover), 8.0f);
        if (icon) EqDrawGlyph(dl, icon, ImVec2(p.x + 12.0f, p.y + h * 0.5f), 12.0f, EqCol(kTextLo));
        EqDrawLabel(dl, ImVec2(p.x + (icon ? 30.0f : 10.0f), p.y + (h - 13.0f) * 0.5f), label, *v ? EqCol(kTextHi) : EqCol(kTextLo), 13.0f);
        char swid[160];
        ImFormatString(swid, IM_ARRAYSIZE(swid), "%s##sw", label);
        const bool clicked = EqToggleSwitch(swid, v, ImVec2(p.x + w - 24.0f, p.y + h * 0.5f));
        ImGui::SetCursorScreenPos(ImVec2(p.x, p.y + h + 2.0f));
        return clicked;
    }

    inline bool RowSlider(const char* icon, const char* label, float* v, float v_min, float v_max, const char* fmt)
    {
        if (!EqPassFilter(label)) return false;
        ImVec2 p = ImGui::GetCursorScreenPos();
        const float w = ImGui::GetContentRegionAvail().x;
        const float h = 34.0f;
        ImGui::InvisibleButton(label, ImVec2(w, h));
        const bool active = ImGui::IsItemActive();
        const bool hovered = ImGui::IsItemHovered();
        ImDrawList* dl = ImGui::GetWindowDrawList();
        bool changed = false;
        const float trackX = p.x + 2.0f, trackW = w - 4.0f;
        if (active)
        {
            float t = ImSaturate((ImGui::GetIO().MousePos.x - trackX) / trackW);
            float nv = v_min + t * (v_max - v_min);
            changed = (nv != *v);
            *v = nv;
        }
        char buf[32];
        ImFormatString(buf, IM_ARRAYSIZE(buf), fmt, *v);
        const ImVec2 vs = EqLabelSize(buf, 12.5f);
        if (icon) EqDrawGlyph(dl, icon, ImVec2(p.x + 12.0f, p.y + 9.0f), 11.0f, EqCol(kTextLo));
        EqDrawLabel(dl, ImVec2(p.x + (icon ? 26.0f : 2.0f), p.y + 2.0f), label, hovered || active ? EqCol(kTextHi) : EqCol(kTextLo), 12.5f);
        EqDrawLabel(dl, ImVec2(p.x + w - vs.x - 2.0f, p.y + 2.0f), buf, EqAccent(), 12.5f);
        const float ty = p.y + 24.0f;
        dl->AddRectFilled(ImVec2(trackX, ty - 1.5f), ImVec2(trackX + trackW, ty + 1.5f), EqCol(ImVec4(1, 1, 1, 0.22f)), 1.5f);
        const float fill = ImSaturate((*v - v_min) / ImMax(0.0001f, v_max - v_min)) * trackW;
        if (fill > 0.5f) dl->AddRectFilled(ImVec2(trackX, ty - 1.5f), ImVec2(trackX + fill, ty + 1.5f), EqAccent(), 1.5f);
        dl->AddCircleFilled(ImVec2(trackX + fill, ty), active ? 6.5f : 5.5f, IM_COL32(247, 250, 255, 255), 16);
        ImGui::SetCursorScreenPos(ImVec2(p.x, p.y + h + 2.0f));
        return changed;
    }

    inline bool ComboRow(const char* icon, const char* label, int* current, const char* const* items, int count)
    {
        if (!EqPassFilter(label)) return false;
        if (count <= 0) return false;
        *current = ImClamp(*current, 0, count - 1);
        ImVec2 p = ImGui::GetCursorScreenPos();
        const float w = ImGui::GetContentRegionAvail().x;
        const float h = 30.0f;
        ImGui::InvisibleButton(label, ImVec2(w, h));
        const bool hovered = ImGui::IsItemHovered();
        const bool clicked = ImGui::IsItemClicked();
        ImDrawList* dl = ImGui::GetWindowDrawList();
        dl->AddRectFilled(p, p + ImVec2(w, h), EqCol(hovered ? kCardHover : kCard), 9.0f);
        dl->AddRect(p + ImVec2(0.5f, 0.5f), p + ImVec2(w - 0.5f, h - 0.5f), EqCol(ImVec4(1, 1, 1, hovered ? 0.14f : 0.06f)), 9.0f);
        if (icon) EqDrawGlyph(dl, icon, ImVec2(p.x + 14.0f, p.y + h * 0.5f), 11.0f, EqCol(kTextLo));
        EqDrawLabel(dl, ImVec2(p.x + (icon ? 28.0f : 12.0f), p.y + (h - 12.5f) * 0.5f), label, EqCol(kTextLo), 12.5f);
        const char* preview = items[*current];
        const ImVec2 ps = EqLabelSize(preview, 12.5f);
        EqDrawLabel(dl, ImVec2(p.x + w - ps.x - 22.0f, p.y + (h - 12.5f) * 0.5f), preview, EqAccent(), 12.5f);
        dl->AddLine(ImVec2(p.x + w - 14.0f, p.y + h * 0.5f - 3.5f), ImVec2(p.x + w - 10.0f, p.y + h * 0.5f), EqCol(kTextLo), 1.4f);
        dl->AddLine(ImVec2(p.x + w - 10.0f, p.y + h * 0.5f), ImVec2(p.x + w - 14.0f, p.y + h * 0.5f + 3.5f), EqCol(kTextLo), 1.4f);
        if (clicked) ImGui::OpenPopup(label);
        bool changed = false;
        ImGui::SetNextWindowPos(ImVec2(p.x, p.y + h + 4.0f));
        ImGui::SetNextWindowSize(ImVec2(w, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.07f, 0.07f, 0.10f, 0.97f));
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.22f, 0.55f, 1.00f, 0.25f));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.22f, 0.55f, 1.00f, 0.35f));
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.22f, 0.55f, 1.00f, 0.45f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(6.0f, 6.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 2.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 10.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 0.0f);
        if (ImGui::BeginPopup(label))
        {
            for (int i = 0; i < count; ++i)
            {
                if (ImGui::Selectable(items[i], *current == i)) { *current = i; changed = true; }
            }
            ImGui::EndPopup();
        }
        ImGui::PopStyleVar(4);
        ImGui::PopStyleColor(4);
        ImGui::SetCursorScreenPos(ImVec2(p.x, p.y + h + 4.0f));
        return changed;
    }

    struct TabDef { const char* glyph; const char* label; };
    static const TabDef kTabs[] =
    {
        { ICON_FA_EYE,        "VISUALS"  },
        { ICON_FA_CROSSHAIRS, "AIMBOT"   },
        { ICON_FA_BOLT,       "MEMORY"   },
        { ICON_FA_TH_LIST,    "SKINS"    },
        { ICON_FA_SLIDERS_H,  "MISC"     },
        { ICON_FA_COG,        "SETTINGS" },
    };
    static const int kTabCount = IM_ARRAYSIZE(kTabs);
    static const char* kHeaderIcons[4] = { ICON_FA_CROSSHAIRS, ICON_FA_EYE, ICON_FA_TH_LIST, ICON_FA_COG };

    inline void Render(MenuState& st)
    {
        EqState() = &st;
        st.HeaderPressed = -1;
        st.TrafficPressed = -1;
        if (!st.Open) return;
        ImGuiIO& io = ImGui::GetIO();
        st.ActiveTab = ImClamp(st.ActiveTab, 0, kTabCount - 1);
        if (st.LastTab != st.ActiveTab) { st.LastTab = st.ActiveTab; st.Fade = 0.0f; }
        st.Fade = ImMin(1.0f, st.Fade + io.DeltaTime / 0.18f);
        const float fade = st.Fade * st.Fade * (3.0f - 2.0f * st.Fade);

        ImVec2 winSize(780.0f, 480.0f);
        winSize.x = ImMin(winSize.x, io.DisplaySize.x - 16.0f);
        winSize.y = ImMin(winSize.y, io.DisplaySize.y - 16.0f);
        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(winSize, ImGuiCond_Always);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
        ImGui::Begin("##equinox_shell", nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoSavedSettings |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
        ImGui::PopStyleColor();
        ImGui::PopStyleVar();

        ImDrawList* dl = ImGui::GetWindowDrawList();
        const ImVec2 p0 = ImGui::GetWindowPos();
        const ImVec2 p1 = p0 + ImGui::GetWindowSize();
        const float R = 18.0f;

        dl->AddShadowRect(p0, p1, IM_COL32(0, 0, 0, 110), 26.0f, ImVec2(0, 6), 0, R);
        if (st.Backdrop)
            dl->AddImageRounded(st.Backdrop, p0, p1, ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 255), R);
        else
            dl->AddRectFilled(p0, p1, IM_COL32(22, 22, 28, 150), R);
        dl->AddRectFilled(p0, p1, EqCol(kGlass), R);
        dl->AddRectFilledMultiColor(p0, ImVec2(p1.x, p0.y + 130.0f),
                                    IM_COL32(255, 255, 255, 14), IM_COL32(255, 255, 255, 14),
                                    IM_COL32(255, 255, 255, 0), IM_COL32(255, 255, 255, 0));
        dl->AddRect(p0 + ImVec2(0.5f, 0.5f), p1 - ImVec2(0.5f, 0.5f), EqCol(kEdge), R);

        const ImVec2 s0 = p0 + ImVec2(10, 10);
        const ImVec2 s1 = ImVec2(p0.x + 196.0f, p1.y - 10.0f);
        dl->AddRectFilled(s0, s1, EqCol(kSidebar), 14.0f);
        dl->AddRectFilledMultiColor(ImVec2(s0.x + 12, s0.y + 12), ImVec2(s0.x + 46, s0.y + 46),
                                    EqCol(EqMix(EqAccentVec(), ImVec4(1, 1, 1, 1), 0.25f)), EqAccent(),
                                    EqCol(EqMix(EqAccentVec(), ImVec4(0, 0, 0, 1), 0.35f)), EqCol(EqMix(EqAccentVec(), ImVec4(0, 0, 0, 1), 0.1f)));
        dl->AddRect(ImVec2(s0.x + 12, s0.y + 12), ImVec2(s0.x + 46, s0.y + 46), IM_COL32(255, 255, 255, 60), 9.0f);
        {
            ImVec2 c(s0.x + 29, s0.y + 29);
            ImVec2 crown[7] =
            {
                ImVec2(c.x - 7.0f, c.y + 5.0f), ImVec2(c.x - 7.0f, c.y - 3.0f), ImVec2(c.x - 2.5f, c.y + 1.0f),
                ImVec2(c.x, c.y - 6.0f), ImVec2(c.x + 2.5f, c.y + 1.0f), ImVec2(c.x + 7.0f, c.y - 3.0f),
                ImVec2(c.x + 7.0f, c.y + 5.0f)
            };
            dl->AddConvexPolyFilled(crown, 7, IM_COL32(255, 255, 255, 240));
        }
        dl->AddText(EqTitleFont(), 15.0f, ImVec2(s0.x + 56, s0.y + 21), EqCol(kTextHi), "Equinox");

        float ty = s0.y + 62.0f;
        for (int i = 0; i < kTabCount; ++i)
        {
            const ImVec2 tmin(s0.x + 8, ty), tmax(s1.x - 8, ty + 32.0f);
            ImGui::SetCursorScreenPos(tmin);
            char id[32];
            ImFormatString(id, IM_ARRAYSIZE(id), "##eqtab%d", i);
            ImGui::InvisibleButton(id, tmax - tmin);
            const bool hov = ImGui::IsItemHovered();
            const bool act = (st.ActiveTab == i);
            if (ImGui::IsItemClicked()) st.ActiveTab = i;
            if (act || hov)
            {
                dl->AddRectFilled(tmin, tmax, EqCol(act ? kCardHover : kCard), 9.0f);
                dl->AddRect(tmin + ImVec2(0.5f, 0.5f), tmax - ImVec2(0.5f, 0.5f), EqCol(ImVec4(1, 1, 1, act ? 0.14f : 0.05f)), 9.0f);
            }
            EqDrawGlyph(dl, kTabs[i].glyph, ImVec2(tmin.x + 18, tmin.y + 16), 13.0f, act ? EqCol(kTextHi) : EqCol(kTextLo));
            EqDrawLabel(dl, ImVec2(tmin.x + 36, tmin.y + 9.5f), kTabs[i].label, act ? EqCol(kTextHi) : EqCol(kTextLo), 12.0f);
            ty += 38.0f;
        }

        {
            const ImVec2 f0(s0.x + 8, s1.y - 52), f1(s1.x - 8, s1.y - 8);
            dl->AddRectFilled(f0, f1, EqCol(kCard), 11.0f);
            dl->AddRect(f0 + ImVec2(0.5f, 0.5f), f1 - ImVec2(0.5f, 0.5f), EqCol(ImVec4(1, 1, 1, 0.07f)), 11.0f);
            dl->AddCircleFilled(ImVec2(f0.x + 22, (f0.y + f1.y) * 0.5f), 12.0f, EqCol(EqMix(EqAccentVec(), ImVec4(0, 0, 0, 1), 0.45f)));
            EqDrawLabel(dl, ImVec2(f0.x + 18, (f0.y + f1.y) * 0.5f - 5.0f), "W", IM_COL32(255, 255, 255, 220), 10.0f);
            EqDrawLabel(dl, ImVec2(f0.x + 40, f0.y + 9), st.TitleText, EqCol(kTextHi), 10.5f);
            EqDrawLabel(dl, ImVec2(f0.x + 40, f0.y + 24), st.SubtitleText, EqCol(kTextLo), 9.5f);
            dl->AddCircleFilled(ImVec2(f1.x - 10, f0.y + 10), 3.0f, EqCol(kGreen));
        }

        const ImVec2 h0(s1.x + 14, p0.y + 10), h1(p1.x - 14, p0.y + 54);
        {
            const ImVec2 fmin = h0 + ImVec2(0, 7), fmax = fmin + ImVec2(190, 30);
            dl->AddRectFilled(fmin, fmax, EqCol(kCard), 10.0f);
            dl->AddCircle(ImVec2(fmin.x + 13, fmin.y + 14), 5.0f, EqCol(kTextLo), 0, 1.4f);
            dl->AddLine(ImVec2(fmin.x + 16.5f, fmin.y + 17.5f), ImVec2(fmin.x + 20, fmin.y + 21), EqCol(kTextLo), 1.4f);
            ImGui::SetCursorScreenPos(ImVec2(fmin.x + 26.0f, fmin.y + (30.0f - ImGui::GetFrameHeight()) * 0.5f));
            ImGui::PushItemWidth(156.0f);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 6));
            ImGui::InputTextWithHint("##eq_search", "Search", st.Search, IM_ARRAYSIZE(st.Search));
            ImGui::PopStyleVar();
            ImGui::PopStyleColor();
            ImGui::PopItemWidth();

            for (int i = 0; i < 4; ++i)
            {
                const ImVec2 bmin(fmax.x + 10 + i * 36.0f, fmin.y), bmax = bmin + ImVec2(30, 30);
                ImGui::SetCursorScreenPos(bmin);
                char id[32];
                ImFormatString(id, IM_ARRAYSIZE(id), "##eqhb%d", i);
                ImGui::InvisibleButton(id, bmax - bmin);
                const bool hov = ImGui::IsItemHovered();
                const bool held = ImGui::IsItemActive();
                if (ImGui::IsItemClicked()) st.HeaderPressed = i;
                dl->AddRectFilled(bmin, bmax, EqCol(held || hov ? kCardHover : kCard), 10.0f);
                EqDrawGlyph(dl, kHeaderIcons[i], (bmin + bmax) * 0.5f, 13.0f, hov ? EqCol(kTextHi) : EqCol(kTextLo));
            }

            const ImVec4 dots[3] = { kRed, kYellow, kGreen };
            for (int i = 0; i < 3; ++i)
            {
                const ImVec2 c(h1.x - 46.0f + i * 16.0f, h0.y + 22.0f);
                ImGui::SetCursorScreenPos(c - ImVec2(6, 6));
                char id[32];
                ImFormatString(id, IM_ARRAYSIZE(id), "##eqtl%d", i);
                ImGui::InvisibleButton(id, ImVec2(12, 12));
                if (ImGui::IsItemClicked()) st.TrafficPressed = i;
                dl->AddCircleFilled(c, ImGui::IsItemHovered() ? 6.0f : 5.0f, EqCol(dots[i]), 20);
            }
        }

        {
            static bool s_drag = false;
            if (s_drag && !io.MouseDown[0]) s_drag = false;
            if (!s_drag && io.MouseClicked[0] && ImGui::IsMouseHoveringRect(h0, h1) && !ImGui::IsAnyItemHovered()) s_drag = true;
            if (s_drag) ImGui::SetWindowPos(ImGui::GetWindowPos() + io.MouseDelta);
        }

        const ImVec2 c0(s1.x + 14, p0.y + 62), c1(p1.x - 14, p1.y - 14);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, fade);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2, 4));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 6));
        ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 4.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarRounding, 4.0f);
        ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, ImVec4(1, 1, 1, 0.22f));
        ImGui::BeginChild("##eq_content", c1 - c0, false, ImGuiWindowFlags_NoBackground);
        ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2(0, (1.0f - fade) * 10.0f));
        if (st.DrawTab) st.DrawTab(st.ActiveTab);
        ImGui::EndChild();
        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(5);

        ImGui::End();
    }
}
