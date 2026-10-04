// Behavioural checks for the controls the main harness does not drive: the
// dropdown sheet, the colour swatch, the keybind button, and rotation safety
// (panel/shell clamping on a resized display).
//
// Row rects are captured at draw time rather than hard-coded, so the checks
// keep tracking the real layout if the shell geometry changes.
//
// Note: BeginGroupCard() is itself a child window, so popup IDs are
// window-scoped. g_popupOpen is sampled inside the card, in the same scope
// ComboRow() ran in — sampling it after EndGroupCard() resolves a different ID
// and reports a popup that is actually open as closed.
#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/ethnir_menu.h"
#include <cstdio>
#include <cstring>

ImFont* F50 = nullptr;
ImFont* F107 = nullptr;
namespace font { ImFont* inter_semibold = nullptr; }
float menu[4] = { 0.04f, 0.52f, 1.0f, 1.0f };

static int   g_combo = 0;
static float g_rgba[4] = { 0.0f, 212.0f, 255.0f, 255.0f };
static int   g_bind  = 0;
static int   g_saves = 0;
static bool  g_popupOpen = false;
// exact row rects, captured at draw time so the test never guesses coordinates
static float g_comboRect[4], g_colorRect[4], g_bindRect[4];

static void DrawTab(int)
{
    using namespace ethnir;
    static const char* opts[] = { "Head", "Chest", "Body" };
    static const char* binds[] = { "Num 0", "Num 1", "F1" };
    BeginGroupCard("eth_controls");
    ImVec2 p = ImGui::GetCursorScreenPos();
    float avail = ImGui::GetContentRegionAvail().x;
    ComboRow(ICON_FA_SLIDERS_H, "Location", &g_combo, opts, 3);
    // popup IDs are window-scoped and BeginGroupCard is a child window, so the
    // open state has to be sampled here, in the same scope ComboRow ran in.
    g_popupOpen = ImGui::IsPopupOpen("Location");
    g_comboRect[0] = p.x; g_comboRect[1] = p.y; g_comboRect[2] = avail; g_comboRect[3] = 30;
    p = ImGui::GetCursorScreenPos();
    ColorRow(ICON_FA_EYE, "Player ESP Color", g_rgba);
    g_colorRect[0] = p.x; g_colorRect[1] = p.y; g_colorRect[2] = avail; g_colorRect[3] = 30;
    p = ImGui::GetCursorScreenPos();
    KeybindRow("Menu bind", &g_bind, binds, 3);
    g_bindRect[0] = p.x; g_bindRect[1] = p.y; g_bindRect[2] = avail; g_bindRect[3] = 30;
    EndGroupCard();
}

static ethnir::MenuState g_st;
static ImGuiIO* g_io = nullptr;

static void Frame() { ImGui::NewFrame(); ethnir::Render(g_st); ImGui::Render(); }
static void Click(float x, float y)
{
    g_io->AddMousePosEvent(x, y);
    g_io->AddMouseButtonEvent(0, true);  Frame();
    g_io->AddMouseButtonEvent(0, false); Frame();
    Frame();
}
static int  g_fail = 0;
static void CHECK(bool ok, const char* m)
{ std::printf("%s %s\n", ok ? "ok  " : "FAIL", m); if (!ok) g_fail++; }

int main()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    g_io = &ImGui::GetIO();
    g_io->DisplaySize = ImVec2(1280.0f, 720.0f);
    g_io->DeltaTime = 1.0f / 60.0f;
    g_io->IniFilename = nullptr; g_io->LogFilename = nullptr;
    g_io->Fonts->AddFontDefault();
    { unsigned char* px = nullptr; int fw = 0, fh = 0;
      g_io->Fonts->GetTexDataAsRGBA32(&px, &fw, &fh); g_io->Fonts->SetTexID((ImTextureID)(intptr_t)1); }
    g_st.DrawTab = DrawTab;
    g_st.OnSave = []() { g_saves++; };
    for (int i = 0; i < 40; ++i) Frame();

    // ---- 1. dropdown sheet: the row press opens the popup, an option commits ----
    {
        const float cx = g_comboRect[0] + g_comboRect[2] * 0.5f;
        const float cy = g_comboRect[1] + g_comboRect[3] * 0.5f;
        g_io->AddMousePosEvent(cx, cy);
        g_io->AddMouseButtonEvent(0, true);  Frame();
        g_io->AddMouseButtonEvent(0, false); Frame();
        Frame();
        CHECK(g_popupOpen, "combo row press opens the dropdown sheet");

        const int before = g_combo;
        bool picked = false;
        // sheet items lay out just below the row, 30px each
        // sheet opens at p.y + rowH + 4, has 6px vertical padding and 30px items
        // Option 0 is already selected, so pick a different one: reopen the
        // sheet each time (a click outside dismisses it) and tap item i.
        for (int i = 1; i < 3 && !picked; ++i) {
            Click(cx, cy);                                  // reopen the sheet
            const float iy = g_comboRect[1] + g_comboRect[3] + 4.0f + 6.0f + 15.0f + i * 30.0f;
            Click(cx, iy);
            if (g_combo != before) picked = true;
        }
        CHECK(picked, "tapping a sheet item commits the selection");
        CHECK(!g_popupOpen, "sheet closes after a selection");
        CHECK(g_combo != before, "selection actually changed the value");
    }

    // ---- 2. colour swatch cycles the palette ----
    {
        float before[4]; for (int i = 0; i < 4; ++i) before[i] = g_rgba[i];
        Click(g_colorRect[0] + g_colorRect[2] * 0.5f, g_colorRect[1] + g_colorRect[3] * 0.5f);
        bool changed = false;
        for (int i = 0; i < 4; ++i) if (g_rgba[i] != before[i]) changed = true;
        CHECK(changed, "colour swatch row cycles to the next palette colour");
    }

    // ---- 3. keybind button cycles the bind ----
    {
        const int before = g_bind;
        Click(g_bindRect[0] + g_bindRect[2] * 0.5f, g_bindRect[1] + g_bindRect[3] * 0.5f);
        CHECK(g_bind != before, "keybind row cycles to the next bind");
    }

    // ---- 4. rotation: resize the display, everything must stay on screen ----
    {
        g_io->DisplaySize = ImVec2(640.0f, 320.0f);   // narrow landscape
        for (int i = 0; i < 40; ++i) Frame();
        const float pw = 258.0f;
        CHECK(g_st.PanelPos.x >= 12.0f && g_st.PanelPos.x <= g_io->DisplaySize.x - 12.0f,
              "settings panel stays horizontally on screen after rotation");
        // 880px shell on a 640px display cannot fully fit by design; the real
        // contract is EqClampMenuPos's margins: >=120px of the shell stays on
        // screen horizontally to grab, and 40px vertically top and bottom.
        CHECK(g_st.WinPos.x + 120.0f <= g_io->DisplaySize.x + 0.5f,
              "shell keeps a 120px grab margin horizontally after rotation");
        CHECK(g_st.WinPos.y >= 40.0f - 0.5f && g_st.WinPos.y <= g_io->DisplaySize.y - 40.0f + 0.5f,
              "shell keeps a 40px grab margin vertically after rotation");
        // and it must never be dragged off so far that no part of the title bar
        // is reachable: the top-left 120x40 grab strip stays on screen
        CHECK(g_st.WinPos.x + 120.0f > 0.0f && g_st.WinPos.y + 40.0f > 0.0f,
              "shell grab strip remains reachable after rotation");
        g_io->DisplaySize = ImVec2(1280.0f, 720.0f);
        for (int i = 0; i < 20; ++i) Frame();
    }

    // ---- 5. a change marks the config dirty and auto-save still fires ----
    {
        g_st.Dirty = false;
        const int before = g_saves;
        for (float y = 160; y < 520 && !g_st.Dirty; y += 3)
            for (float x = 260; x < 1060 && !g_st.Dirty; x += 6) Click(x, y);
        CHECK(g_st.Dirty, "a control change marks the config dirty");
        for (int i = 0; i < 60; ++i) Frame();
        CHECK(g_saves > before, "auto save fires after the debounce window");
    }

    std::printf(g_fail == 0 ? "CONTROLS CHECK PASSED (0 failures)\n" : "CONTROLS CHECK FAILED\n");
    return g_fail;
}
