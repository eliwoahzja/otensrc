#pragma once
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_settings.h"
#include "Icon.h"
#include <functional>
#include <cmath>
#include <cstring>
#include <cctype>

// ============================================================================
//  Equinox menu shell — glass sidebar menu for the in-game overlay.
//
//  Design system:
//    * Translucent glass cards (kCard / kCardHover) on a blurred backdrop.
//    * One accent color (runtime theme hue) drives highlights, fills and glow.
//    * Every state change animates with exponential damping (EqDamp), so the
//      UI feels smooth even on a touch screen.
//
//  Public API (unchanged, consumed by Main.cpp / runtime_preview_menu.h):
//    MenuState, Render(), RowToggle(), RowSlider(), ComboRow(),
//    SectionLabel(), BeginGroupCard(), EndGroupCard(), EqPassFilter()
// ============================================================================

namespace equinox
{
    struct MenuState
    {
        // ---- public state (set/read by the host app) ----
        bool        Open         = true;
        int         ActiveTab    = 0;
        char        Search[64]   = "";
        const char* TitleText    = "CODM: Garnoe | v1.0.87";
        const char* SubtitleText = "White Crowns Official";
        ImTextureID Backdrop     = nullptr;
        int         HeaderPressed  = -1;
        int         TrafficPressed = -1;
        std::function<void(int tab)> DrawTab;

        // ---- internal animation / bookkeeping ----
        int         LastTab = -1;
        float       Fade    = 1.0f;   // tab-switch content fade 0..1
        float       Appear  = 0.0f;   // menu open animation 0..1
        bool        WasOpen = false;
        ImVec2      WinPos  = ImVec2(0.0f, 0.0f);
        bool        WinPosInit = false;
        bool        Dragging   = false;
        bool        FrameSearchHit  = false; // any row matched the search this frame
        bool        LastSearchHit   = true;  // search had matches on the last filtered tab
    };

    // ---- palette -------------------------------------------------------------
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

    // ---- core helpers ----------------------------------------------------------
    inline MenuState*& EqState() { static MenuState* s = nullptr; return s; }
    inline ImFont* EqTextFont() { if (font::inter_semibold) return font::inter_semibold; if (F50) return F50; return ImGui::GetFont(); }
    inline ImFont* EqTitleFont() { if (F50) return F50; return EqTextFont(); }
    inline ImFont* EqIconFont() { if (F107) return F107; return EqTextFont(); }
    inline float EqDamp(float a, float b, float k, float dt) { return b + (a - b) * std::exp(-k * dt); }
    inline ImVec4 EqMix(const ImVec4& a, const ImVec4& b, float t) { return ImVec4(ImLerp(a.x, b.x, t), ImLerp(a.y, b.y, t), ImLerp(a.z, b.z, t), ImLerp(a.w, b.w, t)); }
    inline ImVec2 EqMixVec2(const ImVec2& a, const ImVec2& b, float t) { return ImVec2(ImLerp(a.x, b.x, t), ImLerp(a.y, b.y, t)); }
    inline ImU32 EqCol(const ImVec4& c) { return ImGui::GetColorU32(c); }
    inline ImU32 EqColA(const ImVec4& c, float a) { return ImGui::GetColorU32(ImVec4(c.x, c.y, c.z, c.w * a)); }
    inline ImU32 EqAccent() { return main_runtime_theme::GetAccentU32(); }
    inline ImVec4 EqAccentVec() { return ImGui::ColorConvertU32ToFloat4(EqAccent()); }
    inline ImU32 EqAccentA(float a) { ImVec4 v = EqAccentVec(); v.w = a; return ImGui::GetColorU32(v); }

    // Search filter: case-insensitive substring match against the row label.
    // Also records whether anything matched, so Render() can show a
    // "no matches" placeholder when the search filters everything out.
    inline bool EqPassFilter(const char* label)
    {
        MenuState* st = EqState();
        if (!st || st->Search[0] == 0) { if (st) st->FrameSearchHit = true; return true; }
        char a[128], b[128];
        int i = 0; for (; i < 127 && label[i]; ++i) a[i] = (char)toupper((unsigned char)label[i]); a[i] = 0;
        i = 0; for (; i < 127 && st->Search[i]; ++i) b[i] = (char)toupper((unsigned char)st->Search[i]); b[i] = 0;
        const bool hit = strstr(a, b) != nullptr;
        if (hit) st->FrameSearchHit = true;
        return hit;
    }

    // ---- text / glyph helpers ---------------------------------------------------
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

    // ---- toggle switch ------------------------------------------------------------
    inline bool EqToggleSwitch(const char* id, bool* v, ImVec2 center)
    {
        ImGuiWindow* w = ImGui::GetCurrentWindow();
        ImGuiStorage* stg = ImGui::GetStateStorage();
        const ImGuiID sid = w->GetID(id);
        const ImVec2 size(36.0f, 19.0f);
        const ImVec2 mn(center.x - size.x * 0.5f, center.y - size.y * 0.5f);
        ImGui::SetCursorScreenPos(mn);
        ImGui::InvisibleButton(id, size);
        const bool clicked = ImGui::IsItemClicked();
        const bool hovered = ImGui::IsItemHovered();
        if (clicked) *v = !*v;
        float t = EqDamp(stg->GetFloat(sid, *v ? 1.0f : 0.0f), *v ? 1.0f : 0.0f, 16.0f, ImGui::GetIO().DeltaTime);
        stg->SetFloat(sid, t);
        ImDrawList* dl = ImGui::GetWindowDrawList();
        // soft accent glow while on
        if (t > 0.02f)
        {
            const ImVec4 acc = EqAccentVec();
            dl->AddRectFilled(mn - ImVec2(3, 3), mn + size + ImVec2(3, 3),
                              EqColA(ImVec4(acc.x, acc.y, acc.z, 1.0f), 0.12f * t), (size.y + 6.0f) * 0.5f);
        }
        dl->AddRectFilled(mn, mn + size, EqCol(EqMix(kSwitchOff, EqAccentVec(), t)), size.y * 0.5f);
        dl->AddRect(mn + ImVec2(0.5f, 0.5f), mn + size - ImVec2(0.5f, 0.5f),
                    EqColA(kEdge, 0.6f + 0.8f * t), size.y * 0.5f);
        const float kr = 6.5f + (hovered ? 0.8f : 0.0f) + t * 0.5f;
        const ImVec2 kc(ImLerp(mn.x + size.y * 0.5f, mn.x + size.x - size.y * 0.5f, t), center.y);
        dl->AddCircleFilled(kc + ImVec2(0.0f, 0.5f), kr, IM_COL32(0, 0, 0, 60), 20);   // knob drop shadow
        dl->AddCircleFilled(kc, kr, IM_COL32(247, 250, 255, 255), 20);
        return clicked;
    }

    // ---- section label with accent tick ---------------------------------------------
    inline void SectionLabel(const char* text)
    {
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImDrawList* dl = ImGui::GetWindowDrawList();
        dl->AddRectFilled(ImVec2(p.x, p.y + 3.0f), ImVec2(p.x + 2.5f, p.y + 13.0f), EqAccentA(0.85f), 1.5f);
        EqDrawLabel(dl, ImVec2(p.x + 9.0f, p.y), text, EqCol(kTextLo), 11.0f);
        ImGui::SetCursorScreenPos(ImVec2(p.x, p.y + 17.0f));
    }

    // ---- glass group card --------------------------------------------------------------
    inline void BeginGroupCard(const char* id)
    {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, kCard);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 10.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 8.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 4.0f));
        // BeginChild() with a zero height makes this ImGui version fill the whole
        // remaining pane: BeginChildEx() folds the 0 into the available height and
        // then SetNextWindowSize() marks that height as API-driven, so
        // ImGuiWindowFlags_AlwaysAutoResize never shrinks it back down. Every card
        // then stretches to the bottom of the content pane and pushes the next
        // section off-screen. Calling Begin() with the child flags directly keeps
        // the auto-fit, so the card hugs its rows.
        ImGuiWindow* parent = ImGui::GetCurrentWindow();
        char name[160];
        ImFormatString(name, IM_ARRAYSIZE(name), "%s/eqcard_%08X", parent->Name, parent->GetID(id));
        // Fix the width through the API and leave the height to auto-fit: a zero Y
        // means "height not set by the API", so AlwaysAutoResize still hugs the rows.
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetContentRegionAvail().x, 0.0f));
        ImGui::Begin(name, nullptr,
                     ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                     ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
                     ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar |
                     ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_ChildWindow |
                     ImGuiWindowFlags_AlwaysAutoResize);
    }

    inline void EndGroupCard()
    {
        // The card must still be the current window here: GetWindowDrawList() is
        // only valid before EndChild(), and issuing commands afterwards draws into
        // a child draw list the frame has already left behind. Draw the hairline
        // border (it also makes the card read on any backdrop) while we are still
        // inside, then close out the window and unwind the styles.
        ImDrawList* dl = ImGui::GetWindowDrawList();
        const ImVec2 wmin = ImGui::GetWindowPos();
        const ImVec2 wmax = wmin + ImGui::GetWindowSize();
        dl->AddRect(wmin + ImVec2(0.5f, 0.5f), wmax - ImVec2(0.5f, 0.5f), EqColA(kEdge, 0.7f), 10.0f);
        ImGui::EndChild();
        ImGui::PopStyleVar(4);
        ImGui::PopStyleColor();
        ImGui::Dummy(ImVec2(0, 2));
    }

    // ---- rows -----------------------------------------------------------------------------
    inline bool RowToggle(const char* icon, const char* label, bool* v)
    {
        if (!EqPassFilter(label)) return false;
        ImVec2 p = ImGui::GetCursorScreenPos();
        const float w = ImGui::GetContentRegionAvail().x;
        const float h = 30.0f;
        ImGui::InvisibleButton(label, ImVec2(w, h));
        const bool hovered = ImGui::IsItemHovered();
        const bool rowClicked = ImGui::IsItemClicked();
        ImDrawList* dl = ImGui::GetWindowDrawList();
        if (hovered) dl->AddRectFilled(p + ImVec2(2, 1), p + ImVec2(w - 2, h - 1), EqCol(kCardHover), 9.0f);
        // icon lights up in the accent color while the feature is enabled
        if (icon) EqDrawGlyph(dl, icon, ImVec2(p.x + 12.0f, p.y + h * 0.5f), 12.0f,
                              *v ? EqAccent() : EqCol(kTextLo));
        const float labelT = *v ? 1.0f : (hovered ? 0.4f : 0.0f);
        EqDrawLabel(dl, ImVec2(p.x + (icon ? 30.0f : 10.0f), p.y + (h - 13.0f) * 0.5f), label,
                    EqCol(EqMix(kTextLo, kTextHi, labelT)), 13.0f);
        char swid[160];
        ImFormatString(swid, IM_ARRAYSIZE(swid), "%s##sw", label);
        const bool switchClicked = EqToggleSwitch(swid, v, ImVec2(p.x + w - 26.0f, p.y + h * 0.5f));
        // the whole row is a hit target — friendlier on touch screens
        if (rowClicked && !switchClicked) *v = !*v;
        ImGui::SetCursorScreenPos(ImVec2(p.x, p.y + h + 2.0f));
        return rowClicked || switchClicked;
    }

    inline bool RowSlider(const char* icon, const char* label, float* v, float v_min, float v_max, const char* fmt)
    {
        if (!EqPassFilter(label)) return false;
        ImGuiIO& io = ImGui::GetIO();
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
            float t = ImSaturate((io.MousePos.x - trackX) / trackW);
            float nv = v_min + t * (v_max - v_min);
            changed = (nv != *v);
            *v = nv;
        }
        // damped display value so the fill glides instead of snapping
        ImGuiWindow* win = ImGui::GetCurrentWindow();
        ImGuiStorage* stg = ImGui::GetStateStorage();
        const ImGuiID aid = win->GetID(label);
        float shown = EqDamp(stg->GetFloat(aid, *v), *v, active ? 40.0f : 18.0f, io.DeltaTime);
        stg->SetFloat(aid, shown);

        char buf[32];
        ImFormatString(buf, IM_ARRAYSIZE(buf), fmt, *v);
        const ImVec2 vs = EqLabelSize(buf, 12.0f);
        const ImVec4 acc = EqAccentVec();
        if (icon) EqDrawGlyph(dl, icon, ImVec2(p.x + 12.0f, p.y + 10.0f), 11.0f,
                              hovered || active ? EqAccent() : EqCol(kTextLo));
        EqDrawLabel(dl, ImVec2(p.x + (icon ? 26.0f : 2.0f), p.y + 3.0f), label,
                    hovered || active ? EqCol(kTextHi) : EqCol(kTextLo), 12.5f);
        // value pill
        dl->AddRectFilled(ImVec2(p.x + w - vs.x - 15.0f, p.y + 1.0f), ImVec2(p.x + w - 2.0f, p.y + 17.0f),
                          EqColA(kEdge, 0.55f), 8.0f);
        EqDrawLabel(dl, ImVec2(p.x + w - vs.x - 8.5f, p.y + 3.0f), buf, EqAccent(), 12.0f);

        const float ty = p.y + 25.0f;
        dl->AddRectFilled(ImVec2(trackX, ty - 2.0f), ImVec2(trackX + trackW, ty + 2.0f), EqColA(kSwitchOff, 0.8f), 2.0f);
        const float fill = ImSaturate((shown - v_min) / ImMax(0.0001f, v_max - v_min)) * trackW;
        if (fill > 0.5f)
        {
            dl->AddRectFilled(ImVec2(trackX, ty - 2.0f), ImVec2(trackX + fill, ty + 2.0f), EqAccent(), 2.0f);
            dl->AddRectFilled(ImVec2(trackX + 2.0f, ty - 1.2f), ImVec2(ImMax(trackX + 2.0f, trackX + fill - 3.0f), ty - 0.4f),
                              EqColA(kTextHi, 0.30f), 1.0f); // glossy highlight
        }
        const ImVec2 kc(trackX + fill, ty);
        if (active || hovered)
            dl->AddCircleFilled(kc, 11.0f, EqColA(ImVec4(acc.x, acc.y, acc.z, 1.0f), active ? 0.28f : 0.15f), 20);
        dl->AddCircleFilled(kc + ImVec2(0.0f, 0.5f), active ? 6.5f : 5.5f, IM_COL32(0, 0, 0, 60), 20);
        dl->AddCircleFilled(kc, active ? 6.5f : 5.5f, IM_COL32(247, 250, 255, 255), 20);
        ImGui::SetCursorScreenPos(ImVec2(p.x, p.y + h + 2.0f));
        return changed;
    }

    inline bool ComboRow(const char* icon, const char* label, int* current, const char* const* items, int count)
    {
        if (!EqPassFilter(label)) return false;
        if (count <= 0) return false;
        *current = ImClamp(*current, 0, count - 1);
        ImGuiIO& io = ImGui::GetIO();
        ImVec2 p = ImGui::GetCursorScreenPos();
        const float w = ImGui::GetContentRegionAvail().x;
        const float h = 30.0f;
        ImGui::InvisibleButton(label, ImVec2(w, h));
        const bool hovered = ImGui::IsItemHovered();
        const bool clicked = ImGui::IsItemClicked();
        ImDrawList* dl = ImGui::GetWindowDrawList();
        dl->AddRectFilled(p, p + ImVec2(w, h), EqCol(hovered ? kCardHover : kCard), 9.0f);
        dl->AddRect(p + ImVec2(0.5f, 0.5f), p + ImVec2(w - 0.5f, h - 0.5f), EqColA(kEdge, hovered ? 1.3f : 0.6f), 9.0f);
        if (icon) EqDrawGlyph(dl, icon, ImVec2(p.x + 14.0f, p.y + h * 0.5f), 11.0f, EqCol(kTextLo));
        EqDrawLabel(dl, ImVec2(p.x + (icon ? 28.0f : 12.0f), p.y + (h - 12.5f) * 0.5f), label, EqCol(kTextLo), 12.5f);
        const char* preview = items[*current];
        const ImVec2 ps = EqLabelSize(preview, 12.5f);
        EqDrawLabel(dl, ImVec2(p.x + w - ps.x - 22.0f, p.y + (h - 12.5f) * 0.5f), preview, EqAccent(), 12.5f);

        // chevron that flips smoothly when the popup opens
        ImGuiWindow* win = ImGui::GetCurrentWindow();
        ImGuiStorage* stg = ImGui::GetStateStorage();
        const ImGuiID cid = win->GetID(label);
        const bool open = ImGui::IsPopupOpen(label);
        float ct = EqDamp(stg->GetFloat(cid, 0.0f), open ? 1.0f : 0.0f, 18.0f, io.DeltaTime);
        stg->SetFloat(cid, ct);
        const float cy = p.y + h * 0.5f;
        const float a = ImLerp(2.0f, -2.0f, ct);
        const ImU32 chevCol = EqColA(kTextLo, 0.8f + 0.2f * ct);
        dl->AddLine(ImVec2(p.x + w - 14.0f, cy - a), ImVec2(p.x + w - 10.0f, cy + a), chevCol, 1.4f);
        dl->AddLine(ImVec2(p.x + w - 10.0f, cy + a), ImVec2(p.x + w - 14.0f, cy - a), chevCol, 1.4f);

        if (clicked) ImGui::OpenPopup(label);
        bool changed = false;
        const float itemH = 26.0f;
        const float popupH = count * itemH + 14.0f;
        ImVec2 ppos(p.x, p.y + h + 4.0f);
        if (ppos.y + popupH > io.DisplaySize.y - 8.0f)       // flip above when near the bottom
            ppos.y = p.y - popupH - 4.0f;
        ImGui::SetNextWindowPos(ppos);
        ImGui::SetNextWindowSize(ImVec2(w, 0.0f));
        ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.07f, 0.07f, 0.10f, 0.98f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1, 1, 1, 0.10f));
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(6.0f, 6.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 2.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, 10.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_PopupBorderSize, 1.0f);
        if (ImGui::BeginPopup(label))
        {
            for (int i = 0; i < count; ++i)
            {
                ImGui::PushID(i);
                const bool sel = (*current == i);
                ImGui::PushStyleColor(ImGuiCol_Text, sel ? EqAccent() : EqCol(kTextHi));
                if (ImGui::Selectable(items[i], false, 0, ImVec2(0.0f, itemH)))
                {
                    *current = i;
                    changed = true;
                }
                ImGui::PopStyleColor();
                const bool hov = ImGui::IsItemHovered();
                const ImVec2 imin = ImGui::GetItemRectMin();
                const ImVec2 imax = ImGui::GetItemRectMax();
                ImDrawList* pdl = ImGui::GetWindowDrawList();
                if (hov) pdl->AddRectFilled(imin, imax, EqColA(kTextHi, 0.07f), 8.0f);
                if (sel)
                    EqDrawGlyph(pdl, ICON_FA_CHECK, ImVec2(imax.x - 16.0f, (imin.y + imax.y) * 0.5f), 11.0f, EqAccent());
                ImGui::PopID();
            }
            ImGui::EndPopup();
        }
        ImGui::PopStyleVar(4);
        ImGui::PopStyleColor(5);
        ImGui::SetCursorScreenPos(ImVec2(p.x, p.y + h + 4.0f));
        return changed;
    }

    // ---- tab definitions ------------------------------------------------------------------
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

    inline void EqClampMenuPos(MenuState& st, const ImVec2& size, const ImGuiIO& io)
    {
        const float dw = ImMax(io.DisplaySize.x, 1.0f);
        const float dh = ImMax(io.DisplaySize.y, 1.0f);
        const float xMin = 120.0f - size.x, xMax = dw - 120.0f;
        const float yMin = 48.0f, yMax = dh - 48.0f;
        st.WinPos.x = ImClamp(st.WinPos.x, ImMin(xMin, xMax), ImMax(xMin, xMax));
        st.WinPos.y = ImClamp(st.WinPos.y, ImMin(yMin, yMax), ImMax(yMin, yMax));
    }

    inline void Render(MenuState& st)
    {
        EqState() = &st;
        st.HeaderPressed = -1;
        st.TrafficPressed = -1;
        if (!st.Open)
        {
            st.WasOpen = false;
            st.WinPosInit = false;   // reappear centered next time
            return;
        }
        ImGuiIO& io = ImGui::GetIO();

        // ---- open animation ----
        if (!st.WasOpen) { st.Appear = 0.0f; st.Fade = 0.0f; st.LastTab = st.ActiveTab; }
        st.WasOpen = true;
        st.Appear = ImMin(1.0f, st.Appear + io.DeltaTime / 0.22f);
        const float appear = st.Appear * st.Appear * (3.0f - 2.0f * st.Appear);

        // ---- tab-switch fade ----
        st.ActiveTab = ImClamp(st.ActiveTab, 0, kTabCount - 1);
        if (st.LastTab != st.ActiveTab) { st.LastTab = st.ActiveTab; st.Fade = 0.0f; }
        st.Fade = ImMin(1.0f, st.Fade + io.DeltaTime / 0.18f);
        const float fade = st.Fade * st.Fade * (3.0f - 2.0f * st.Fade);

        // ---- window sizing / placement (position survives dragging) ----
        ImVec2 winSize(780.0f, 480.0f);
        winSize.x = ImMin(winSize.x, io.DisplaySize.x - 16.0f);
        winSize.y = ImMin(winSize.y, io.DisplaySize.y - 16.0f);
        winSize.x = ImMax(winSize.x, 320.0f);
        winSize.y = ImMax(winSize.y, 240.0f);
        if (!st.WinPosInit)
        {
            const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            st.WinPos = ImVec2(center.x - winSize.x * 0.5f, center.y - winSize.y * 0.5f);
            st.WinPosInit = true;
        }
        EqClampMenuPos(st, winSize, io);

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, appear);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
        ImGui::SetNextWindowPos(st.WinPos + ImVec2(0.0f, (1.0f - appear) * 26.0f), ImGuiCond_Always);
        ImGui::SetNextWindowSize(winSize, ImGuiCond_Always);
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
        const ImVec4 acc = EqAccentVec();

        // ---- shell: layered shadow, backdrop, glass ----
        dl->AddShadowRect(p0, p1, IM_COL32(0, 0, 0, 130), 34.0f, ImVec2(0, 10), 0, R);
        dl->AddShadowRect(p0, p1, IM_COL32(0, 0, 0, 90), 12.0f, ImVec2(0, 2), 0, R);
        if (st.Backdrop)
            dl->AddImageRounded(st.Backdrop, p0, p1, ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 255), R);
        else
            dl->AddRectFilled(p0, p1, IM_COL32(22, 22, 28, 150), R);
        dl->AddRectFilled(p0, p1, EqCol(kGlass), R);
        dl->AddRectFilledMultiColor(p0, ImVec2(p1.x, p0.y + 130.0f),
                                    EqColA(acc, 0.10f), EqColA(acc, 0.10f),
                                    IM_COL32(255, 255, 255, 0), IM_COL32(255, 255, 255, 0));
        dl->AddRect(p0 + ImVec2(0.5f, 0.5f), p1 - ImVec2(0.5f, 0.5f), EqCol(kEdge), R);
        dl->AddRectFilled(ImVec2(p0.x + 26.0f, p0.y + 1.0f), ImVec2(p1.x - 26.0f, p0.y + 2.5f), EqAccentA(0.50f), 1.0f);

        // ---- sidebar ----
        const ImVec2 s0 = p0 + ImVec2(10, 10);
        const ImVec2 s1 = ImVec2(p0.x + 196.0f, p1.y - 10.0f);
        dl->AddRectFilled(s0, s1, EqCol(kSidebar), 14.0f);
        dl->AddLine(ImVec2(s1.x + 0.5f, s0.y + 8.0f), ImVec2(s1.x + 0.5f, s1.y - 8.0f), EqColA(kEdge, 0.5f));

        // logo block
        dl->AddRectFilledMultiColor(ImVec2(s0.x + 14, s0.y + 14), ImVec2(s0.x + 54, s0.y + 54),
                                    EqCol(EqMix(acc, ImVec4(1, 1, 1, 1), 0.25f)), EqCol(acc),
                                    EqCol(EqMix(acc, ImVec4(0, 0, 0, 1), 0.35f)), EqCol(EqMix(acc, ImVec4(0, 0, 0, 1), 0.1f)));
        dl->AddRect(ImVec2(s0.x + 14, s0.y + 14), ImVec2(s0.x + 54, s0.y + 54), IM_COL32(255, 255, 255, 60), 9.0f);
        {
            ImVec2 c(s0.x + 34, s0.y + 34);
            ImVec2 crown[7] =
            {
                ImVec2(c.x - 7.0f, c.y + 5.0f), ImVec2(c.x - 7.0f, c.y - 3.0f), ImVec2(c.x - 2.5f, c.y + 1.0f),
                ImVec2(c.x, c.y - 6.0f), ImVec2(c.x + 2.5f, c.y + 1.0f), ImVec2(c.x + 7.0f, c.y - 3.0f),
                ImVec2(c.x + 7.0f, c.y + 5.0f)
            };
            dl->AddConvexPolyFilled(crown, 7, IM_COL32(255, 255, 255, 240));
        }
        dl->AddText(EqTitleFont(), 15.0f, ImVec2(s0.x + 64, s0.y + 18), EqCol(kTextHi), "Equinox");
        dl->AddText(EqTextFont(), 8.5f, ImVec2(s0.x + 64, s0.y + 38), EqColA(kTextLo, 0.85f), "MOD MENU");
        dl->AddLine(ImVec2(s0.x + 12, s0.y + 66), ImVec2(s1.x - 12, s0.y + 66), EqColA(kEdge, 0.6f));

        // ---- navigation tabs (animated hover + sliding accent pill) ----
        const float tabTop = s0.y + 74.0f;
        const float tabH = 30.0f;
        const float tabPitch = 36.0f;
        ImGuiStorage* shellStg = ImGui::GetStateStorage();
        const float pillTargetY = tabTop + (float)st.ActiveTab * tabPitch + tabH * 0.5f;
        const ImGuiID pillId = ImGui::GetID("##eq_pill_y");
        const float pillY = EqDamp(shellStg->GetFloat(pillId, pillTargetY), pillTargetY, 14.0f, io.DeltaTime);
        shellStg->SetFloat(pillId, pillY);

        float ty = tabTop;
        for (int i = 0; i < kTabCount; ++i)
        {
            const ImVec2 tmin(s0.x + 10, ty), tmax(s1.x - 10, ty + tabH);
            ImGui::SetCursorScreenPos(tmin);
            char id[32];
            ImFormatString(id, IM_ARRAYSIZE(id), "##eqtab%d", i);
            ImGui::InvisibleButton(id, tmax - tmin);
            const bool hov = ImGui::IsItemHovered();
            const bool act = (st.ActiveTab == i);
            if (ImGui::IsItemClicked()) st.ActiveTab = i;

            char hid[24];
            ImFormatString(hid, IM_ARRAYSIZE(hid), "##eqhot%d", i);
            const ImGuiID hotId = ImGui::GetID(hid);
            float hot = EqDamp(shellStg->GetFloat(hotId, act ? 1.0f : 0.0f), hov ? 1.0f : 0.0f, 12.0f, io.DeltaTime);
            shellStg->SetFloat(hotId, hot);

            if (act)
            {
                dl->AddRectFilled(tmin, tmax, EqCol(kCardHover), 9.0f);
                dl->AddRectFilled(tmin, tmax, EqColA(acc, 0.10f), 9.0f);
                dl->AddRect(tmin + ImVec2(0.5f, 0.5f), tmax - ImVec2(0.5f, 0.5f), EqColA(kEdge, 1.4f), 9.0f);
            }
            else if (hot > 0.01f)
            {
                dl->AddRectFilled(tmin, tmax, EqColA(kTextHi, 0.055f * hot), 9.0f);
            }
            const float lit = ImMax(act ? 1.0f : 0.0f, hot * 0.6f);
            EqDrawGlyph(dl, kTabs[i].glyph, ImVec2(tmin.x + 16, tmin.y + tabH * 0.5f), 13.0f,
                        act ? EqAccent() : EqCol(EqMix(kTextLo, kTextHi, lit)));
            EqDrawLabel(dl, ImVec2(tmin.x + 32, tmin.y + (tabH - 12.0f) * 0.5f), kTabs[i].label,
                        EqCol(EqMix(kTextLo, kTextHi, act ? 1.0f : hot)), 11.5f);
            ty += tabPitch;
        }
        // sliding accent pill on the left gutter
        dl->AddRectFilled(ImVec2(s0.x + 3.0f, pillY - 10.0f), ImVec2(s0.x + 5.5f, pillY + 10.0f), EqAccent(), 1.5f);
        dl->AddRectFilled(ImVec2(s0.x + 1.5f, pillY - 13.0f), ImVec2(s0.x + 7.0f, pillY + 13.0f), EqAccentA(0.16f), 2.5f);

        // ---- footer status card (pulsing status dot) ----
        {
            const ImVec2 f0(s0.x + 8, s1.y - 52), f1(s1.x - 8, s1.y - 8);
            dl->AddRectFilled(f0, f1, EqCol(kCard), 11.0f);
            dl->AddRect(f0 + ImVec2(0.5f, 0.5f), f1 - ImVec2(0.5f, 0.5f), EqColA(kEdge, 0.7f), 11.0f);
            dl->AddCircleFilled(ImVec2(f0.x + 22, (f0.y + f1.y) * 0.5f), 12.0f, EqCol(EqMix(acc, ImVec4(0, 0, 0, 1), 0.45f)));
            EqDrawLabel(dl, ImVec2(f0.x + 18, (f0.y + f1.y) * 0.5f - 5.0f), "W", IM_COL32(255, 255, 255, 220), 10.0f);
            EqDrawLabel(dl, ImVec2(f0.x + 40, f0.y + 9), st.TitleText, EqCol(kTextHi), 10.5f);
            EqDrawLabel(dl, ImVec2(f0.x + 40, f0.y + 24), st.SubtitleText, EqCol(kTextLo), 9.5f);
            const float pulse = 0.5f + 0.5f * std::sin((float)ImGui::GetTime() * 2.2f);
            const ImVec2 dotC(f1.x - 10, f0.y + 10);
            dl->AddCircleFilled(dotC, 5.5f + pulse * 1.5f, EqColA(kGreen, 0.18f), 20);
            dl->AddCircleFilled(dotC, 3.0f, EqColA(kGreen, 0.65f + 0.35f * pulse), 20);
        }

        // ---- header: search + quick actions + fps + traffic lights ----
        const ImVec2 h0(s1.x + 14, p0.y + 10), h1(p1.x - 14, p0.y + 54);
        {
            const ImVec2 fmin = h0 + ImVec2(0, 7), fmax = fmin + ImVec2(190, 30);
            dl->AddRectFilled(fmin, fmax, EqCol(kCard), 10.0f);
            EqDrawGlyph(dl, ICON_FA_SEARCH, ImVec2(fmin.x + 14, (fmin.y + fmax.y) * 0.5f), 10.5f, EqColA(kTextLo, 0.9f));

            const bool hasQuery = st.Search[0] != 0;
            ImGui::SetCursorScreenPos(ImVec2(fmin.x + 27.0f, fmin.y + (30.0f - ImGui::GetFrameHeight()) * 0.5f));
            ImGui::PushItemWidth(hasQuery ? 126.0f : 150.0f);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 6));
            ImGui::InputTextWithHint("##eq_search", "Search features", st.Search, IM_ARRAYSIZE(st.Search));
            const bool searchActive = ImGui::IsItemActive();
            ImGui::PopStyleVar();
            ImGui::PopStyleColor(3);
            ImGui::PopItemWidth();
            // focus ring + glow
            if (searchActive)
            {
                dl->AddRect(fmin + ImVec2(0.5f, 0.5f), fmax - ImVec2(0.5f, 0.5f), EqAccentA(0.65f), 10.0f);
                dl->AddRect(fmin - ImVec2(1.5f, 1.5f), fmax + ImVec2(1.5f, 1.5f), EqAccentA(0.16f), 12.0f);
            }
            else
            {
                dl->AddRect(fmin + ImVec2(0.5f, 0.5f), fmax - ImVec2(0.5f, 0.5f), EqColA(kEdge, 0.7f), 10.0f);
            }

            // clear button
            if (hasQuery)
            {
                const ImVec2 cmin(fmax.x - 24.0f, fmin.y + 7.0f);
                ImGui::SetCursorScreenPos(cmin);
                ImGui::InvisibleButton("##eq_sclear", ImVec2(16, 16));
                const bool chov = ImGui::IsItemHovered();
                if (ImGui::IsItemClicked()) st.Search[0] = 0;
                if (chov) dl->AddCircleFilled(ImVec2(cmin.x + 8, cmin.y + 8), 8.0f, EqColA(kTextHi, 0.10f), 20);
                EqDrawGlyph(dl, ICON_FA_TIMES, ImVec2(cmin.x + 8, cmin.y + 8), 8.5f, EqColA(kTextLo, chov ? 1.0f : 0.8f));
            }

            // quick action buttons
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
                if (hov && !held)
                    dl->AddRect(bmin + ImVec2(0.5f, 0.5f), bmax - ImVec2(0.5f, 0.5f), EqColA(kEdge, 1.0f), 10.0f);
                EqDrawGlyph(dl, kHeaderIcons[i], (bmin + bmax) * 0.5f + ImVec2(0.0f, held ? 1.0f : 0.0f), 13.0f,
                            hov ? EqCol(kTextHi) : EqCol(kTextLo));
            }
            // FPS pill (only when there is room for it)
            const int fps = (int)(io.Framerate + 0.5f);
            char fpsbuf[24];
            ImFormatString(fpsbuf, IM_ARRAYSIZE(fpsbuf), "FPS %d", fps);
            const ImVec2 fts = EqLabelSize(fpsbuf, 10.0f);
            const float pillW = fts.x + 30.0f;
            const ImVec2 fpmin(h1.x - 60.0f - pillW, fmin.y), fpmax(fpmin.x + pillW, fmin.y + 30.0f);
            if (fpmin.x > h0.x + 352.0f)
            {
                dl->AddRectFilled(fpmin, fpmax, EqCol(kCard), 10.0f);
                dl->AddRect(fpmin + ImVec2(0.5f, 0.5f), fpmax - ImVec2(0.5f, 0.5f), EqColA(kEdge, 0.7f), 10.0f);
                const ImVec4 fcol = fps >= 55 ? kGreen : (fps >= 30 ? kYellow : kRed);
                EqDrawGlyph(dl, ICON_FA_TACHOMETER_ALT, ImVec2(fpmin.x + 13, (fpmin.y + fpmax.y) * 0.5f), 10.5f, EqCol(fcol));
                EqDrawLabel(dl, ImVec2(fpmin.x + 23, (fpmin.y + fpmax.y) * 0.5f - fts.y * 0.5f), fpsbuf, EqCol(kTextLo), 10.0f);
            }

            // traffic lights (0 = hide, 1 = visuals, 2 = toggle theme — handled by Main.cpp)
            const ImVec4 dots[3] = { kRed, kYellow, kGreen };
            for (int i = 0; i < 3; ++i)
            {
                const ImVec2 c(h1.x - 46.0f + i * 16.0f, h0.y + 22.0f);
                ImGui::SetCursorScreenPos(c - ImVec2(6, 6));
                char id[32];
                ImFormatString(id, IM_ARRAYSIZE(id), "##eqtl%d", i);
                ImGui::InvisibleButton(id, ImVec2(12, 12));
                const bool hov = ImGui::IsItemHovered();
                if (ImGui::IsItemClicked()) st.TrafficPressed = i;
                dl->AddCircleFilled(c, hov ? 6.0f : 5.0f, EqCol(dots[i]), 20);
                if (hov)
                    dl->AddCircle(c, 8.5f, EqColA(ImVec4(dots[i].x, dots[i].y, dots[i].z, 1.0f), 0.35f), 20, 1.2f);
            }
        }        // ---- window dragging (header strip, when not over a widget)----
        if (st.Dragging && !io.MouseDown[0]) st.Dragging = false;
        if (!st.Dragging && io.MouseClicked[0] && ImGui::IsMouseHoveringRect(h0, h1) && !ImGui::IsAnyItemHovered())
            st.Dragging = true;
        if (st.Dragging && (io.MouseDelta.x != 0.0f || io.MouseDelta.y != 0.0f))
        {
            st.WinPos += io.MouseDelta;
            EqClampMenuPos(st, winSize, io);
            ImGui::SetWindowPos(st.WinPos);
        }

        // ---- content area ----
        const ImVec2 c0(s1.x + 14, p0.y + 62), c1(p1.x - 14, p1.y - 14);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, fade);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(2, 4));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 6));
        ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarSize, 4.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarRounding, 4.0f);
        ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, ImVec4(0, 0, 0, 0));
        ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, ImVec4(1, 1, 1, 0.22f));
        // Anchor the child at c0. The header strip and the sidebar tabs above
        // both call SetCursorScreenPos, so the cursor is left somewhere in the
        // sidebar; without this the content pane renders underneath it.
        ImGui::SetCursorScreenPos(c0);
        ImGui::BeginChild("##eq_content", c1 - c0, false, ImGuiWindowFlags_NoBackground);
        ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2(0, (1.0f - fade) * 10.0f));

        const bool filtering = st.Search[0] != 0;
        const bool tabFilters = st.ActiveTab >= 0 && st.ActiveTab <= 3;
        if (filtering && tabFilters && !st.LastSearchHit)
        {
            // search filtered out every row on this tab — show a friendly placeholder
            ImDrawList* cdl = ImGui::GetWindowDrawList();
            const ImVec2 avail = ImGui::GetContentRegionAvail();
            const ImVec2 origin = ImGui::GetCursorScreenPos();
            const ImVec2 card(ImMin(avail.x, 340.0f), 100.0f);
            const ImVec2 cmin(origin.x + (avail.x - card.x) * 0.5f, origin.y + (avail.y - card.y) * 0.5f);
            cdl->AddRectFilled(cmin, cmin + card, EqColA(kTextHi, 0.03f), 12.0f);
            cdl->AddRect(cmin + ImVec2(0.5f, 0.5f), cmin + card - ImVec2(0.5f, 0.5f), EqColA(kEdge, 0.6f), 12.0f);
            EqDrawGlyph(cdl, ICON_FA_SEARCH, ImVec2(cmin.x + card.x * 0.5f, cmin.y + 30.0f), 14.0f, EqColA(kTextLo, 0.9f));
            const char* line1 = "NO MATCHES";
            const ImVec2 s1t = EqLabelSize(line1, 13.0f);
            EqDrawLabel(cdl, ImVec2(cmin.x + (card.x - s1t.x) * 0.5f, cmin.y + 47.0f), line1, EqCol(kTextHi), 13.0f);
            const char* line2 = "Try a different keyword.";
            const ImVec2 s2t = EqLabelSize(line2, 11.0f);
            EqDrawLabel(cdl, ImVec2(cmin.x + (card.x - s2t.x) * 0.5f, cmin.y + 68.0f), line2, EqCol(kTextLo), 11.0f);
        }
        else if (st.DrawTab)
        {
            st.DrawTab(st.ActiveTab);
        }
        ImGui::EndChild();
        ImGui::PopStyleColor(2);
        ImGui::PopStyleVar(5);

        // roll the search hit flag over for the next frame (only on tabs that filter)
        if (tabFilters) st.LastSearchHit = st.FrameSearchHit;
        st.FrameSearchHit = false;

        ImGui::End();
        ImGui::PopStyleVar(); // Alpha
    }
}
