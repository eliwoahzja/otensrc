#pragma once
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_settings.h"
#include "Icon.h"
#include <functional>
#include <cmath>
#include <cstring>
#include <cctype>
#include <cstdio>

// ============================================================================
//  ETHNIR — iOS Settings-style shell for the in-game overlay.
//  Consumed by Main.cpp / runtime_preview_menu.h through the row helpers.
// ============================================================================

namespace ethnir
{
    struct MenuState
    {
        bool        Open          = true;
        int         ActiveTab     = 1;          // host tab id, see kTabs
        char        Search[64]    = "";
        const char* TitleText     = "ETHNIR";
        const char* SubtitleText  = "MOD MENU";
        ImTextureID Backdrop      = nullptr;    // optional wallpaper texture

        int HeaderPressed  = -1;                // 0 = Save
        int TrafficPressed = -1;                // 0 = minimise finished (host collapses to its pill)

        bool  Dark              = true;         // dark mode first
        bool  ShowSettingsPanel = true;
        bool  HelpOpen          = false;
        float AnimSpeed         = 1.0f;         // 0.5 .. 2.0, scales every damped rate
        int   AccentIndex       = 0;            // index into kAccents
        int   MenuBind          = 0;            // index into kMenuBinds

        bool  AutoSave          = true;
        float SaveDebounce      = 0.5f;         // seconds of idle before OnSave()
        bool  Dirty             = false;        // a control changed since the last save
        bool  InputActive       = false;        // a slider/drag is currently held
        float DirtyTimer        = 0.0f;
        std::function<void()> OnSave;           // host: writes the config atomically

        std::function<void(int tab)> DrawTab;

        int    LastTab  = -1;
        float  Fade     = 1.0f;                 // tab-switch content fade 0..1
        float  Appear   = 0.0f;                 // open/close animation 0..1
        // backdrop bloom phase, advanced by the real frame delta
        float  BackdropPhase = 0.0f;
        bool   WasOpen  = false;
        bool   Closing  = false;                // playing the fade-out before TrafficPressed
        ImVec2 WinPos   = ImVec2(0.0f, 0.0f);
        bool   WinPosInit = false;
        bool   Dragging   = false;
        ImVec2 PanelPos   = ImVec2(0.0f, 0.0f);
        bool   PanelPosInit = false;
        bool   PanelDragging = false;
        float  SaveFlash  = 0.0f;               // "Saved" chip timer
        // Touch space -> GL space. Touch positions come from the game in its
        // own pixel space while io.DisplaySize is the raw EGL surface, so a raw
        // MouseDelta under-moves the window. The host writes the measured ratio
        // per axis here; 1.0 when both spaces agree.
        float  DragScaleX = 1.0f;
        float  DragScaleY = 1.0f;
        // true when this frame's press landed on a real header control
        bool   HeaderControl = false;
        // pointer position when the drag started; the window is moved by the
        // delta since this, never by io.MouseDelta, whose first frame after a
        // press still carries the position from before the press
        ImVec2 DragRef   = ImVec2(0.0f, 0.0f);
        bool   FrameSearchHit = false;
        bool   LastSearchHit  = true;
        bool   HelpChipPressedFrame = false;   // the "?" chip was tapped this frame
        int    FrameRowCount  = 0;
        int    LastRowCount   = 0;
    };

    constexpr float kWinW     = 880.0f;
    constexpr float kWinH     = 520.0f;
    constexpr float kPad      = 14.0f;
    constexpr float kBrandH   = 30.0f;
    constexpr float kSideW    = 200.0f;
    constexpr float kColGap   = 16.0f;
    constexpr float kRadius   = 22.0f;
    constexpr float kRowH     = 30.0f;         // toggle / combo / colour rows
    constexpr float kSliderH  = 32.0f;         // slider rows

    // iOS system colours
    constexpr ImVec4 kSwitchOn(0.204f, 0.780f, 0.349f, 1.0f);   // #34C759 green switches

    struct AccentDef { const char* name; float hue; };
    static const AccentDef kAccents[] =
    {
        { "Blue",   0.5833f },      // #0A84FF with ApplyAccentFromHue()'s saturation
        { "Teal",   0.4900f },
        { "Green",  0.3600f },
        { "Gold",   0.1150f },
        { "Pink",   0.9300f },
        { "Violet", 0.7400f },
    };
    static const int kAccentCount = IM_ARRAYSIZE(kAccents);

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
        p.base      = ImVec4(0.035f, 0.040f, 0.055f, 0.88f);
        p.scrim     = ImVec4(0.010f, 0.012f, 0.020f, 0.46f);
        p.text      = ImVec4(0.965f, 0.972f, 0.985f, 1.00f);
        p.textDim   = ImVec4(0.760f, 0.785f, 0.840f, 1.00f);
        p.textFaint = ImVec4(0.550f, 0.580f, 0.645f, 1.00f);
        p.cardBg    = ImVec4(1.000f, 1.000f, 1.000f, 0.050f);
        p.cardEdge  = ImVec4(1.000f, 1.000f, 1.000f, 0.070f);
        p.glassRim  = ImVec4(1.000f, 1.000f, 1.000f, 1.00f);
        p.hover     = ImVec4(1.000f, 1.000f, 1.000f, 0.070f);
        p.side      = ImVec4(0.000f, 0.000f, 0.000f, 0.180f);
        p.sideEdge  = ImVec4(1.000f, 1.000f, 1.000f, 0.055f);
        p.switchOff = ImVec4(1.000f, 1.000f, 1.000f, 0.170f);
        p.track     = ImVec4(1.000f, 1.000f, 1.000f, 0.150f);
        p.popupBg   = ImVec4(0.075f, 0.082f, 0.098f, 0.97f);
        p.sep       = ImVec4(1.000f, 1.000f, 1.000f, 0.050f);
        return p;
    }

    inline Palette EqPalLight()
    {
        Palette p;
        p.base      = ImVec4(0.945f, 0.955f, 0.975f, 0.90f);
        p.scrim     = ImVec4(1.000f, 1.000f, 1.000f, 0.55f);
        p.text      = ImVec4(0.070f, 0.080f, 0.110f, 1.00f);
        p.textDim   = ImVec4(0.355f, 0.385f, 0.450f, 1.00f);
        p.textFaint = ImVec4(0.510f, 0.540f, 0.600f, 1.00f);
        p.cardBg    = ImVec4(1.000f, 1.000f, 1.000f, 0.720f);
        p.cardEdge  = ImVec4(0.000f, 0.000f, 0.000f, 0.055f);
        p.glassRim  = ImVec4(1.000f, 1.000f, 1.000f, 1.00f);
        p.hover     = ImVec4(0.000f, 0.000f, 0.000f, 0.040f);
        p.side      = ImVec4(1.000f, 1.000f, 1.000f, 0.520f);
        p.sideEdge  = ImVec4(0.000f, 0.000f, 0.000f, 0.055f);
        p.switchOff = ImVec4(0.000f, 0.000f, 0.000f, 0.140f);
        p.track     = ImVec4(0.000f, 0.000f, 0.000f, 0.120f);
        p.popupBg   = ImVec4(1.000f, 1.000f, 1.000f, 0.98f);
        p.sep       = ImVec4(0.000f, 0.000f, 0.000f, 0.045f);
        return p;
    }

    inline MenuState*& EqState() { static MenuState* s = nullptr; return s; }
    inline Palette EqPal() { MenuState* s = EqState(); return (s && !s->Dark) ? EqPalLight() : EqPalDark(); }

    inline ImFont* EqTextFont() { if (font::inter_semibold) return font::inter_semibold; if (F50) return F50; return ImGui::GetFont(); }
    inline ImFont* EqTitleFont() { if (F50) return F50; return EqTextFont(); }
    inline ImFont* EqIconFont() { if (F107) return F107; return EqTextFont(); }

    // every damped rate goes through here, so the panel's Animation speed
    // scales hover, pill, fade and switch motion at once
    inline float EqRate(float k) { MenuState* s = EqState(); return k * (s ? ImMax(0.15f, s->AnimSpeed) : 1.0f); }
    inline float EqDamp(float a, float b, float k, float dt) { return b + (a - b) * std::exp(-EqRate(k) * dt); }
    inline float EqEase(float t) { return t * t * (3.0f - 2.0f * t); }

    // Persistent animation slot keyed by ImGuiID: damped toward the target on
    // delta time, so nothing flickers and nothing resets between frames.
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
    inline ImU32 EqAccent() { return main_runtime_theme::GetAccentU32(); }
    inline ImVec4 EqAccentVec() { return ImGui::ColorConvertU32ToFloat4(EqAccent()); }
    inline ImU32 EqAccentA(float a) { ImVec4 v = EqAccentVec(); v.w = a; return ImGui::GetColorU32(v); }

    // iOS glass edge: a specular rim that is bright along the top and left and
    // fades out toward the bottom and right. Four thin bars rather than one
    // gradient-filled rounded rect, so the panel fill underneath is never
    // painted twice and never doubles up its alpha.
    inline void EqDrawGlassRim(ImDrawList* dl, const ImVec2& p0, const ImVec2& p1, float R, const Palette& pal)
    {
        const float t = 1.6f;
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
        MenuState* s = EqState();
        index = ImClamp(index, 0, kAccentCount - 1);
        if (s) s->AccentIndex = index;
        main_runtime_theme::g_menuHue = kAccents[index].hue;
        main_runtime_theme::ApplyAccentFromHue();
        if (s) s->Dirty = true;
    }

    // "Label##id" -> "Label", so an internal id can never reach AddText
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

    // ButtonBehavior's default flags (PressedOnClickRelease) commit only on
    // release over the same rect, so a drag that becomes a scroll flips nothing.
    inline bool EqPress(const char* id, const ImVec2& size, bool* outHovered = nullptr, bool* outHeld = nullptr)
    {
        ImGuiWindow* w = ImGui::GetCurrentWindow();
        const ImGuiID wid = w->GetID(id);
        const ImVec2 p = ImGui::GetCursorScreenPos();
        bool hovered = false, held = false;
        const bool pressed = ImGui::ButtonBehavior(ImRect(p, p + size), wid, &hovered, &held, ImGuiButtonFlags_None);
        // ButtonBehavior runs without ItemAdd, so NewFrame() would see the item
        // die and clear ActiveId mid-drag. Keep the id alive by hand.
        ImGui::KeepAliveID(wid);
        if (outHovered) *outHovered = hovered;
        if (outHeld) *outHeld = held;
        if (held && EqState()) EqState()->InputActive = true;
        return pressed;
    }

    // Eq* prefix is mandatory: ImGui already exports BeginColumns/NextColumn/
    // EndColumns, and tab bodies import both namespaces, so an unqualified call
    // is ambiguous and the NDK build fails.
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
        c.W = ImFloor((avail - gap) * 0.5f);
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

    // Case-insensitive substring match on the row label, counting every row that
    // asked so the hint can read "explore N functions...".
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

    // iOS switch; the row around it owns the hit test
    inline void EqDrawSwitch(ImDrawList* dl, const char* id, bool on, ImVec2 center, float dt, bool hovered)
    {
        const float t = EqAnim(id, on ? 1.0f : 0.0f, 18.0f, dt, on ? 1.0f : 0.0f);
        const Palette pal = EqPal();
        const ImVec2 size(36.0f, 20.0f);
        const ImVec2 mn(center.x - size.x * 0.5f, center.y - size.y * 0.5f);
        dl->AddRectFilled(mn, mn + size, EqCol(EqMix(pal.switchOff, kSwitchOn, t)), size.y * 0.5f);
        if (hovered)
            dl->AddRect(mn + ImVec2(0.5f, 0.5f), mn + size - ImVec2(0.5f, 0.5f), EqColA(pal.cardEdge, 1.6f), size.y * 0.5f);
        const float kr = 8.0f + (hovered ? 0.6f : 0.0f);
        const ImVec2 kc(ImLerp(mn.x + size.y * 0.5f, mn.x + size.x - size.y * 0.5f, t), center.y);
        dl->AddCircleFilled(kc + ImVec2(0.0f, 0.6f), kr, IM_COL32(0, 0, 0, 70), 24);
        dl->AddCircleFilled(kc, kr, IM_COL32(252, 253, 255, 255), 24);
    }

    // hairline derived from the row rects, never hand-placed
    inline int& EqCardRowIndex() { static int i = 0; return i; }

    inline void EqRowSeparator(ImDrawList* dl, const ImVec2& rowMin, const ImVec2& rowMax)
    {
        const Palette pal = EqPal();
        const float inset = 13.0f;
        dl->AddLine(ImVec2(rowMin.x + inset, rowMin.y - 1.0f), ImVec2(rowMax.x - 2.0f, rowMin.y - 1.0f), EqCol(pal.sep), 1.0f);
    }

    inline void SectionLabel(const char* text)
    {
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImDrawList* dl = ImGui::GetWindowDrawList();
        const Palette pal = EqPal();
        EqDrawTracked(dl, EqTextFont(), 10.0f, ImVec2(p.x + 2.0f, p.y + 3.0f), EqCol(pal.textFaint), text, 1.1f);
        ImGui::SetCursorScreenPos(ImVec2(p.x, p.y + 21.0f));
    }

    // Inset card that hugs its rows: Begin() with the child flags keeps the
    // auto-fit, BeginChild() with a zero height would stretch to the pane.
    inline void BeginGroupCard(const char* id)
    {
        ImGui::PushStyleColor(ImGuiCol_ChildBg, EqPal().cardBg);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 14.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 9.0f));
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
        ImDrawList* dl = ImGui::GetWindowDrawList();
        const ImVec2 wmin = ImGui::GetWindowPos();
        const ImVec2 wmax = wmin + ImGui::GetWindowSize();
        dl->AddRect(wmin + ImVec2(0.5f, 0.5f), wmax - ImVec2(0.5f, 0.5f), EqCol(EqPal().cardEdge), 14.0f);
        ImGui::EndChild();
        ImGui::PopStyleVar(4);
        ImGui::PopStyleColor();
        ImGui::SetCursorScreenPos(ImGui::GetCursorScreenPos() + ImVec2(0.0f, 12.0f));
    }

    inline bool RowToggle(const char* icon, const char* label, bool* v)
    {
        (void)icon;   // rows stay icon-free: glyphs live in the sidebar and header
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
        if (hovered) dl->AddRectFilled(p, p + ImVec2(w, h), EqCol(pal.hover), 9.0f);
        const float ty = p.y + (h - 13.0f) * 0.5f;
        EqDrawLabel(dl, ImVec2(p.x + 3.0f, ty), clean, EqCol(pal.text), 13.0f);
        EqDrawSwitch(dl, label, *v, ImVec2(p.x + w - 20.0f, p.y + h * 0.5f), ImGui::GetIO().DeltaTime, hovered);

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
        const float trackW = 84.0f;
        const float trackX = p.x + w - trackW;
        const float trackY = p.y + h * 0.5f;

        char clean[128];
        EqStripId(label, clean, IM_ARRAYSIZE(clean));
        bool hovered = false, held = false;
        EqPress(label, ImVec2(w, h), &hovered, &held);

        bool changed = false;
        if (held)     // drag while the finger is down; the value commits live
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
        const ImVec2 vs = EqLabelSize(buf, 12.5f);
        ImDrawList* dl = ImGui::GetWindowDrawList();
        if (EqCardRowIndex()++ > 0) EqRowSeparator(dl, p, p + ImVec2(w, h));
        if (hovered) dl->AddRectFilled(p, p + ImVec2(w, h), EqCol(pal.hover), 9.0f);
        EqDrawLabel(dl, ImVec2(p.x + 3.0f, p.y + (h - 13.0f) * 0.5f), clean,
                    EqCol(hovered || held ? pal.text : pal.textDim), 13.0f);
        EqDrawLabel(dl, ImVec2(trackX - 10.0f - vs.x, p.y + (h - 12.5f) * 0.5f), buf, EqAccent(), 12.5f);

        dl->AddRectFilled(ImVec2(trackX, trackY - 2.0f), ImVec2(trackX + trackW, trackY + 2.0f), EqCol(pal.track), 2.0f);
        const float fill = ImSaturate((shown - v_min) / ImMax(0.0001f, v_max - v_min)) * trackW;
        if (fill > 0.5f)
            dl->AddRectFilled(ImVec2(trackX, trackY - 2.0f), ImVec2(trackX + fill, trackY + 2.0f), EqAccent(), 2.0f);
        const ImVec2 kc(trackX + fill, trackY);
        if (hovered || held) dl->AddCircleFilled(kc, 8.0f, EqAccentA(0.22f), 20);
        dl->AddCircleFilled(kc, held ? 5.5f : 4.5f, IM_COL32(252, 253, 255, 250), 20);

        ImGui::SetCursorScreenPos(ImVec2(p.x, p.y + h + 2.0f));
        return changed;
    }

    // iOS dropdown sheet; flips above the row when the pane bottom is close
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
        const float h = kRowH;

        char clean[128];
        EqStripId(label, clean, IM_ARRAYSIZE(clean));
        bool hovered = false;
        const bool pressed = EqPress(label, ImVec2(w, h), &hovered);

        ImDrawList* dl = ImGui::GetWindowDrawList();
        if (EqCardRowIndex()++ > 0) EqRowSeparator(dl, p, p + ImVec2(w, h));
        if (hovered) dl->AddRectFilled(p, p + ImVec2(w, h), EqCol(pal.hover), 9.0f);
        EqDrawLabel(dl, ImVec2(p.x + 3.0f, p.y + (h - 13.0f) * 0.5f), clean, EqCol(pal.textDim), 13.0f);
        const char* preview = items[*current];
        const ImVec2 ps = EqLabelSize(preview, 12.5f);
        EqDrawLabel(dl, ImVec2(p.x + w - ps.x - 24.0f, p.y + (h - 12.5f) * 0.5f), preview, EqCol(pal.text), 12.5f);

        const bool open = ImGui::IsPopupOpen(label);
        char cid[160];
        ImFormatString(cid, IM_ARRAYSIZE(cid), "%s##chev", label);
        const float ct = EqAnim(cid, open ? 1.0f : 0.0f, 18.0f, io.DeltaTime, 0.0f);
        const float cy = p.y + h * 0.5f;
        const float a = ImLerp(1.6f, -1.6f, ct);
        const ImU32 chev = EqColA(pal.textFaint, 0.85f + 0.15f * ct);
        dl->AddLine(ImVec2(p.x + w - 14.0f, cy - a), ImVec2(p.x + w - 10.0f, cy + a), chev, 1.4f);
        dl->AddLine(ImVec2(p.x + w - 10.0f, cy + a), ImVec2(p.x + w - 6.0f, cy - a), chev, 1.4f);

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

    // label left, hex + chip right; tap cycles the palette
    inline bool ColorRow(const char* icon, const char* label, float* rgba /* 0..255 */)
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
        if (hovered) dl->AddRectFilled(p, p + ImVec2(w, h), EqCol(pal.hover), 9.0f);
        EqDrawLabel(dl, ImVec2(p.x + 3.0f, p.y + (h - 13.0f) * 0.5f), clean, EqCol(pal.textDim), 13.0f);

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

    // label left, N segments in a rounded track right
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

        // The row-wide press must stop short of the segment track: ButtonBehavior
        // claims ActiveId on the first press under the cursor, so a rect spanning
        // the segments would starve them and the control would be dead.
        bool rowHovered = false;
        const float rowPressW = ImMax(24.0f, trackX - 6.0f - p.x);
        EqPress(label, ImVec2(rowPressW, h), &rowHovered);
        ImDrawList* dl = ImGui::GetWindowDrawList();
        if (EqCardRowIndex()++ > 0) EqRowSeparator(dl, p, p + ImVec2(w, h));
        if (rowHovered) dl->AddRectFilled(p, p + ImVec2(rowPressW, h), EqCol(pal.hover), 9.0f);
        EqDrawLabel(dl, ImVec2(p.x + 3.0f, p.y + (h - 13.0f) * 0.5f), clean, EqCol(pal.textDim), 13.0f);

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

    // label left, bordered key chip right; tap cycles the bind
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
        if (hovered) dl->AddRectFilled(p, p + ImVec2(w, h), EqCol(pal.hover), 9.0f);
        EqDrawLabel(dl, ImVec2(p.x + 3.0f, p.y + (h - 13.0f) * 0.5f), clean, EqCol(pal.textDim), 13.0f);
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

    // Shell base: optional backdrop, animated accent bloom, fixed scrim.
    // Plain rounded rects only — four draw calls, nothing to allocate.
    inline void EqDrawShellBase(ImDrawList* dl, const ImVec2& p0, const ImVec2& p1, float R, const Palette& pal, ImTextureID backdrop)
    {
        if (backdrop != nullptr)
        {
            dl->AddImageRounded(backdrop, p0, p1, ImVec2(0.0f, 0.0f), ImVec2(1.0f, 1.0f), IM_COL32(255, 255, 255, 255), R);
        }
        else
        {
            dl->AddRectFilled(p0, p1, EqCol(pal.base), R);

            // Breathing top-down accent gradient. Every band spans the full
            // width; narrowing them left a half-width block, not a gradient.
            const float phase = EqBackdropPhase();
            const float breathe = 0.5f + 0.5f * ImSin(phase);
            const float h = p1.y - p0.y;
            const float w = p1.x - p0.x;

            struct Slab { float depth; float alpha; };
            const Slab slabs[3] = { { 0.42f, 0.055f }, { 0.26f, 0.045f }, { 0.12f, 0.035f } };
            for (int i = 0; i < 3; ++i)
            {
                const float depth = slabs[i].depth * (0.88f + 0.24f * breathe);
                const float alpha = slabs[i].alpha * (0.70f + 0.55f * breathe);
                dl->AddRectFilled(p0, ImVec2(p1.x, p0.y + h * depth), EqAccentA(alpha), R);
            }

            // Drifting highlight, anchored to p1.x so it never spills past the corner
            const float sway = 0.5f + 0.5f * ImSin(phase * 0.61f + 1.7f);
            const float x = p0.x + w * (0.62f - 0.22f * sway);
            dl->AddRectFilled(ImVec2(x, p0.y), ImVec2(p1.x, p0.y + h * 0.22f), EqAccentA(0.030f), R);
        }
        dl->AddRectFilled(p0, p1, EqCol(pal.scrim), R);
        EqDrawGlassRim(dl, p0, p1, R, pal);
    }

    inline void EqDrawSidebar(MenuState& st, const ImVec2& s0, const ImVec2& s1)
    {
        ImDrawList* dl = ImGui::GetWindowDrawList();
        const Palette pal = EqPal();

        // Own BeginChild pane: it clips drawing AND input to its rect
        ImGui::SetCursorScreenPos(s0);
        ImGui::BeginChild("##ethnir_nav", s1 - s0, false, ImGuiWindowFlags_NoBackground);
        dl = ImGui::GetWindowDrawList();
        dl->AddRectFilled(s0, s1, EqCol(pal.side), 16.0f);
        dl->AddRect(s0 + ImVec2(0.5f, 0.5f), s1 - ImVec2(0.5f, 0.5f), EqCol(pal.sideEdge), 16.0f);

        const float itemTop = s0.y + 12.0f;
        const float itemPitch = 38.0f;
        const float itemH = 34.0f;

        int activeRow = 0;
        for (int i = 0; i < kTabCount; ++i)
            if (kTabs[i].tab == st.ActiveTab) activeRow = i;

        // sliding accent highlight, animated in the sidebar's own storage
        const float hlTarget = itemTop + activeRow * itemPitch;
        const float hlY = EqAnim("##ethnir_hl", hlTarget, 14.0f, ImGui::GetIO().DeltaTime, hlTarget);
        dl->AddRectFilled(ImVec2(s0.x + 8.0f, hlY), ImVec2(s1.x - 8.0f, hlY + itemH), EqColA(pal.text, 0.10f), 11.0f);
        dl->AddRectFilled(ImVec2(s0.x + 8.0f, hlY + 8.0f), ImVec2(s0.x + 11.0f, hlY + itemH - 8.0f), EqAccent(), 1.5f);

        for (int i = 0; i < kTabCount; ++i)
        {
            const ImVec2 tmin(s0.x + 8.0f, itemTop + i * itemPitch);
            const ImVec2 tmax(s1.x - 8.0f, tmin.y + itemH);
            ImGui::SetCursorScreenPos(tmin);
            char id[32];
            ImFormatString(id, IM_ARRAYSIZE(id), "##ethnir_tab%d", i);
            bool hov = false;
            const bool pressed = EqPress(id, tmax - tmin, &hov);
            const bool act = (kTabs[i].tab == st.ActiveTab);
            if (pressed && !act) { st.ActiveTab = kTabs[i].tab; st.Fade = 0.0f; }

            char hid[36];
            ImFormatString(hid, IM_ARRAYSIZE(hid), "##ethnir_hot%d", i);
            const float hot = EqAnim(hid, hov ? 1.0f : 0.0f, 12.0f, ImGui::GetIO().DeltaTime, act ? 1.0f : 0.0f);
            if (!act && hot > 0.01f)
                dl->AddRectFilled(tmin, tmax, EqColA(pal.text, 0.05f * hot), 11.0f);

            EqDrawGlyph(dl, kTabs[i].glyph, ImVec2(tmin.x + 20.0f, (tmin.y + tmax.y) * 0.5f), 13.0f,
                        act ? EqAccent() : EqCol(EqMix(pal.textFaint, pal.text, hot)));
            EqDrawLabel(dl, ImVec2(tmin.x + 38.0f, (tmin.y + tmax.y) * 0.5f - 6.5f), kTabs[i].label,
                        EqCol(act ? pal.text : EqMix(pal.textDim, pal.text, hot)), 13.0f);
            if (act)
                dl->AddCircleFilled(ImVec2(tmax.x - 12.0f, (tmin.y + tmax.y) * 0.5f), 2.4f, EqAccent(), 12);
        }

        // help chip, bottom-left of the sidebar pane
        const ImVec2 chipC(s0.x + 27.0f, s1.y - 27.0f);
        ImGui::SetCursorScreenPos(chipC - ImVec2(13, 13));
        bool chipHov = false;
        const bool chipPressed = EqPress("##ethnir_help", ImVec2(26, 26), &chipHov);
        if (chipPressed)
        {
            st.HelpOpen = !st.HelpOpen;
            st.HelpChipPressedFrame = true;   // don't dismiss the card on the same tap
        }
        if (chipHov || st.HelpOpen) dl->AddCircleFilled(chipC, 14.0f, EqColA(pal.text, 0.08f), 24);
        dl->AddCircle(chipC, 13.0f, EqColA(pal.text, st.HelpOpen ? 0.22f : 0.14f), 24, 1.0f);
        EqDrawLabel(dl, ImVec2(chipC.x - 4.0f, chipC.y - 7.0f), "?", EqCol(st.HelpOpen ? pal.text : pal.textDim), 13.0f);
        EqDrawLabel(dl, ImVec2(chipC.x + 20.0f, chipC.y - 5.5f), st.SubtitleText, EqCol(pal.textFaint), 10.0f);

        ImGui::EndChild();
    }

    inline void EqDrawSettingsPanel(MenuState& st)
    {
        const Palette pal = EqPal();
        const ImGuiIO& io = ImGui::GetIO();
        const ImVec2 size(258.0f, 0.0f);
        if (!st.PanelPosInit)
        {
            st.PanelPos = ImVec2(ImMin(st.WinPos.x + kWinW + 18.0f, io.DisplaySize.x - size.x - 12.0f),
                                 st.WinPos.y + 26.0f);
            st.PanelPosInit = true;
        }
        // keep the panel on screen (rotation-safe); narrow displays pin it left
        const float panelMaxX = io.DisplaySize.x - size.x - 12.0f;
        st.PanelPos.x = (panelMaxX > 12.0f) ? ImClamp(st.PanelPos.x, 12.0f, panelMaxX) : 12.0f;
        st.PanelPos.y = ImClamp(st.PanelPos.y, 12.0f, ImMax(12.0f, io.DisplaySize.y - 60.0f));

        ImGui::SetNextWindowPos(st.PanelPos, ImGuiCond_Always);
        ImGui::SetNextWindowSize(size, ImGuiCond_Always);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(14.0f, 12.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));
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
        const float R = 18.0f;
        dl->AddShadowRect(p0, p1, IM_COL32(0, 0, 0, 120), 26.0f, ImVec2(0, 8), 0, R);
        dl->AddRectFilled(p0, p1, EqCol(pal.popupBg), R);
        dl->AddRectFilled(p0, ImVec2(p1.x, p0.y + 44.0f), EqColA(EqAccentVec(), 0.10f), R);
        dl->AddRect(p0 + ImVec2(0.5f, 0.5f), p1 - ImVec2(0.5f, 0.5f), EqCol(pal.cardEdge), R);
        EqDrawGlassRim(dl, p0, p1, R, pal);

        // title + drag strip
        EqDrawTracked(dl, EqTextFont(), 12.0f, ImVec2(p0.x + 16.0f, p0.y + 15.0f), EqCol(pal.text), "QUICK SETTINGS", 0.8f);
        const ImVec2 dragMin(p0.x + 12.0f, p0.y + 8.0f), dragMax(p1.x - 12.0f, p0.y + 38.0f);
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
        ImGui::SetCursorScreenPos(ImVec2(p0.x + 14.0f, p0.y + 46.0f));

        // rows (search filter is off here: the panel keeps its own controls)
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

            // accent swatches — each swatch is its own hit rect, so there is no
            // row-wide press here (the first hit test in a frame owns the hover)
            ImDrawList* cdl = ImGui::GetWindowDrawList();
            const float w = ImGui::GetContentRegionAvail().x;
            const float h = kRowH;
            const ImVec2 p = ImGui::GetCursorScreenPos();
            if (EqCardRowIndex()++ > 0) EqRowSeparator(cdl, p, p + ImVec2(w, h));
            EqDrawLabel(cdl, ImVec2(p.x + 3.0f, p.y + (h - 13.0f) * 0.5f), "Accent color", EqCol(pal.textDim), 13.0f);
            const float sw = 16.0f, gap = 7.0f;
            const float total = kAccentCount * sw + (kAccentCount - 1) * gap;
            const float x0 = p.x + w - total;
            for (int i = 0; i < kAccentCount; ++i)
            {
                const ImVec2 c(x0 + i * (sw + gap) + sw * 0.5f, p.y + h * 0.5f);
                float r = 0, g = 0, b = 0;
                ImGui::ColorConvertHSVtoRGB(kAccents[i].hue, 0.96f, 1.0f, r, g, b);
                char aid[32];
                ImFormatString(aid, IM_ARRAYSIZE(aid), "##accent%d", i);
                ImGui::SetCursorScreenPos(c - ImVec2(9, 9));
                bool hov = false;
                if (EqPress(aid, ImVec2(18, 18), &hov))
                    EqApplyAccentIndex(i);
                if (i == st.AccentIndex)
                    cdl->AddCircle(c, 9.0f, EqColA(ImVec4(r, g, b, 1.0f), 0.9f), 24, 1.6f);
                cdl->AddCircleFilled(c, hov ? 7.0f : 6.0f, EqCol(ImVec4(r, g, b, 1.0f)), 24);
            }
            ImGui::SetCursorScreenPos(ImVec2(p.x, p.y + h + 2.0f));

            KeybindRow("Menu bind", &st.MenuBind, kMenuBinds, kMenuBindCount);
        }
        EndGroupCard();
        EqFilterOn() = true;

        ImGui::End();
    }

    // Returns true while the pointer is over the card, so Render() can dismiss
    // it on an outside tap.
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
        EqDrawGlassRim(dl, p0, p1, R, pal);

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
            st.WinPosInit = false;      // reappear centred next time
            st.Appear = 0.0f;
            st.Fade = 1.0f;
            EqFilterOn() = true;
            return;
        }

        ImGuiIO& io = ImGui::GetIO();
        const float dt = io.DeltaTime;
        const Palette pal = EqPal();

        // ---- open / close animation (fade + scale + slide) ----
        if (!st.WasOpen) { st.Appear = 0.0f; st.Fade = 0.0f; st.LastTab = st.ActiveTab; }
        st.WasOpen = true;
        bool finishClose = false;
        if (st.Closing)
        {
            // TrafficPressed only once the fade-out has finished
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
        // delta-timed, so the bloom looks the same at 30 and 120 fps
        st.BackdropPhase += dt * 0.9f;
        if (st.BackdropPhase > 6.2831853f) st.BackdropPhase -= 6.2831853f;

        // ---- window ----
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

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, appear);      // stays pushed until the end
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
        ImGui::PopStyleVar(2);      // padding + spacing only; Alpha is still on the stack

        ImDrawList* dl = ImGui::GetWindowDrawList();
        const ImVec2 p0 = ImGui::GetWindowPos();
        const ImVec2 p1 = p0 + ImGui::GetWindowSize();

        // ---- shell base (drawn slightly scaled during the open/close spring) ----
        {
            const ImVec2 mid = (p0 + p1) * 0.5f;
            const float sc = 0.94f + 0.06f * appear;
            const ImVec2 b0 = mid + (p0 - mid) * sc;
            const ImVec2 b1 = mid + (p1 - mid) * sc;
            dl->AddShadowRect(b0, b1, IM_COL32(0, 0, 0, (int)(120 * appear)), 30.0f, ImVec2(0, 10), 0, kRadius);
            EqDrawShellBase(dl, b0, b1, kRadius, pal, st.Backdrop);
            dl->AddRect(b0 + ImVec2(0.5f, 0.5f), b1 - ImVec2(0.5f, 0.5f), EqCol(pal.cardEdge), kRadius);
        }

        // ---- wordmark (top-left, above the sidebar) ----
        const ImVec2 brandPos(p0.x + kPad + 6.0f, p0.y + kPad + 3.0f);
        EqDrawTracked(dl, EqTitleFont(), 14.0f, brandPos, EqCol(pal.text), st.TitleText, 1.6f);
        {
            const float bw = EqTrackedWidth(EqTitleFont(), 14.0f, st.TitleText, 1.6f);
            dl->AddRectFilled(ImVec2(brandPos.x + bw + 8.0f, brandPos.y + 6.0f),
                              ImVec2(brandPos.x + bw + 8.0f + 6.0f, brandPos.y + 9.0f), EqAccent(), 1.5f);
        }

        // ---- top bar: Save pill · search · FPS · gear · minimise ----
        // Marks whether this frame's press landed on a real header control.
        // The drag test below cannot use IsAnyItemHovered(): the search field
        // and its clear button register items that span the strip, which left
        // the window undraggable wherever they reach.
        st.HeaderControl = false;
        const ImVec2 h0(p0.x + kPad + kSideW + 16.0f, p0.y + kPad + 2.0f);
        const ImVec2 h1(p1.x - kPad, h0.y + 30.0f);
        const float contentW = h1.x - h0.x;

        if (!st.Closing)
        {
            // Save pill
            const ImVec2 bmin = h0, bmax = h0 + ImVec2(88.0f, 30.0f);
            ImGui::SetCursorScreenPos(bmin);
            bool saveHov = false, saveHeld = false;
            if (EqPress("##ethnir_save", bmax - bmin, &saveHov, &saveHeld))
            {
                st.HeaderPressed = 0;
                st.SaveFlash = 1.6f;
                st.Dirty = false;
                st.DirtyTimer = 0.0f;
            }
            dl->AddRectFilled(bmin, bmax, EqCol(saveHeld || saveHov ? pal.hover : pal.cardBg), 15.0f);
            dl->AddRect(bmin + ImVec2(0.5f, 0.5f), bmax - ImVec2(0.5f, 0.5f), EqCol(pal.cardEdge), 15.0f);
            if (st.SaveFlash > 0.0f)
            {
                const float t = ImSaturate(st.SaveFlash);
                EqDrawGlyph(dl, ICON_FA_CHECK, ImVec2(bmin.x + 19.0f, (bmin.y + bmax.y) * 0.5f), 11.5f, EqAccentA(0.45f + 0.55f * t));
                EqDrawLabel(dl, ImVec2(bmin.x + 32.0f, (bmin.y + bmax.y) * 0.5f - 6.5f), "Saved", EqAccent(), 13.0f);
            }
            else
            {
                EqDrawGlyph(dl, ICON_FA_SAVE, ImVec2(bmin.x + 19.0f, (bmin.y + bmax.y) * 0.5f), 11.5f,
                            EqCol(saveHov ? pal.text : pal.textDim));
                EqDrawLabel(dl, ImVec2(bmin.x + 32.0f, (bmin.y + bmax.y) * 0.5f - 6.5f), "Save",
                            EqCol(saveHov ? pal.text : pal.textDim), 13.0f);
            }

            // FPS + gear + minimise on the right
            char fpsbuf[24];
            ImFormatString(fpsbuf, IM_ARRAYSIZE(fpsbuf), "%d FPS", (int)(io.Framerate + 0.5f));
            const ImVec2 fts = EqLabelSize(fpsbuf, 11.0f);
            EqDrawLabel(dl, ImVec2(h1.x - 30.0f - 6.0f - 36.0f - 12.0f - fts.x, h0.y + 15.0f - fts.y * 0.5f),
                        fpsbuf, EqCol(pal.textFaint), 11.0f);

            struct HeaderBtn { const char* id; const char* glyph; };
            const HeaderBtn btns[2] = { { "##ethnir_gear", ICON_FA_COG }, { "##ethnir_min", ICON_FA_MINUS } };
            for (int i = 0; i < 2; ++i)
            {
                const float bx = h1.x - 30.0f - i * 36.0f;
                const ImVec2 bmin(bx, h0.y), bmax(bx + 30.0f, h0.y + 30.0f);
                ImGui::SetCursorScreenPos(bmin);
                bool hov = false, held = false;
                const bool pressed = EqPress(btns[i].id, ImVec2(30, 30), &hov, &held);
                const bool on = (i == 0 && st.ShowSettingsPanel);
                if (pressed)
                {
                    if (i == 0) st.ShowSettingsPanel = !st.ShowSettingsPanel;
                    else st.Closing = true;    // fade out, then report TrafficPressed
                }
                const ImVec4 btnBg = on ? ImVec4(pal.text.x, pal.text.y, pal.text.z, 0.12f) : (hov ? pal.hover : pal.cardBg);
                dl->AddRectFilled(bmin, bmax, EqCol(btnBg), 15.0f);
                dl->AddRect(bmin + ImVec2(0.5f, 0.5f), bmax - ImVec2(0.5f, 0.5f), EqCol(pal.cardEdge), 15.0f);
                EqDrawGlyph(dl, btns[i].glyph, (bmin + bmax) * 0.5f, 12.5f, EqCol(on || hov ? pal.text : pal.textDim));
            }

            // search (centred in the content column)
            const float searchW = ImMin(300.0f, ImMax(170.0f, contentW - 330.0f));
            const ImVec2 fmin(h0.x + (contentW - searchW) * 0.5f, h0.y);
            const ImVec2 fmax(fmin.x + searchW, h0.y + 30.0f);
            if (io.MouseClicked[0])
            {
                if (ImGui::IsMouseHoveringRect(bmin, bmax))                        st.HeaderControl = true;
                if (ImGui::IsMouseHoveringRect(fmin, fmax))                        st.HeaderControl = true;
                if (ImGui::IsMouseHoveringRect(ImVec2(fmax.x + 8.0f, h0.y), h1))   st.HeaderControl = true;
            }
            dl->AddRectFilled(fmin, fmax, EqCol(pal.cardBg), 15.0f);
            EqDrawGlyph(dl, ICON_FA_SEARCH, ImVec2(fmin.x + 17.0f, (fmin.y + fmax.y) * 0.5f), 11.0f, EqColA(pal.textFaint, 0.95f));

            char hint[64];
            ImFormatString(hint, IM_ARRAYSIZE(hint), "explore %d functions...", st.LastRowCount);
            ImGui::SetCursorScreenPos(ImVec2(fmin.x + 30.0f, fmin.y + (30.0f - ImGui::GetFrameHeight()) * 0.5f));
            ImGui::PushItemWidth(searchW - 46.0f);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0, 0, 0, 0));
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 6));
            ImGui::InputTextWithHint("##ethnir_search", hint, st.Search, IM_ARRAYSIZE(st.Search));
            const bool searchActive = ImGui::IsItemActive();
            ImGui::PopStyleVar();
            ImGui::PopStyleColor(3);
            ImGui::PopItemWidth();
            if (searchActive)
                dl->AddRect(fmin + ImVec2(0.5f, 0.5f), fmax - ImVec2(0.5f, 0.5f), EqAccentA(0.55f), 15.0f);
            else
                dl->AddRect(fmin + ImVec2(0.5f, 0.5f), fmax - ImVec2(0.5f, 0.5f), EqCol(pal.cardEdge), 15.0f);
            if (st.Search[0] != 0)
            {
                const ImVec2 cmin(fmax.x - 24.0f, fmin.y + 7.0f);
                ImGui::SetCursorScreenPos(cmin);
                ImGui::InvisibleButton("##ethnir_sclear", ImVec2(16, 16));
                const bool chov = ImGui::IsItemHovered();
                if (ImGui::IsItemClicked()) st.Search[0] = 0;
                if (chov) dl->AddCircleFilled(ImVec2(cmin.x + 8, cmin.y + 8), 8.0f, EqColA(pal.text, 0.10f), 20);
                EqDrawGlyph(dl, ICON_FA_TIMES, ImVec2(cmin.x + 8, cmin.y + 8), 8.5f, EqColA(pal.textDim, chov ? 1.0f : 0.8f));
            }
        }

        // ---- window dragging (header strip, when no widget is under it) ----
        if (st.Dragging && !io.MouseDown[0]) st.Dragging = false;
        if (!st.Dragging && io.MouseClicked[0] && ImGui::IsMouseHoveringRect(h0, h1) && !st.HeaderControl)
        {
            st.Dragging = true;
            st.DragRef  = io.MousePos;
        }
        if (st.Dragging)
        {
            // Anchored to the press, so the frame the drag starts on cannot
            // inherit a MouseDelta measured from wherever the pointer was before.
            const ImVec2 move = io.MousePos - st.DragRef;
            if (move.x != 0.0f || move.y != 0.0f)
            {
                st.WinPos += ImVec2(move.x * st.DragScaleX, move.y * st.DragScaleY);
                EqClampMenuPos(st, winSize, io);
                ImGui::SetWindowPos(st.WinPos);
            }
            st.DragRef = io.MousePos;
        }

        // ---- sidebar: its own BeginChild pane, clipped to its own rect ----
        const ImVec2 s0(p0.x + kPad, p0.y + kPad + kBrandH);
        const ImVec2 s1(p0.x + kPad + kSideW, p1.y - kPad);
        EqDrawSidebar(st, s0, s1);

        // ---- content: separate BeginChild pane ----
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

        // ---- floating panel + help card (drawn outside the shell window) ----
        if (st.ShowSettingsPanel && !st.Closing) EqDrawSettingsPanel(st);
        if (st.HelpOpen && !st.Closing)
        {
            const bool helpHovered = EqDrawHelpCard(st, ImVec2(s0.x + 27.0f, s1.y - 27.0f), s0);
            if (ImGui::IsMouseClicked(0) && !st.HelpChipPressedFrame && !helpHovered)
                st.HelpOpen = false;    // the chip toggles itself; outside taps dismiss
        }
        st.HelpChipPressedFrame = false;
        EqFilterOn() = true;

        // ---- auto save: debounce, never mid-drag (runs after every pane drew) ----
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
            st.TrafficPressed = 0;      // host collapses to its pill now
            st.WasOpen = false;
            st.WinPosInit = false;
        }

        ImGui::PopStyleVar();   // Alpha
    }
}
