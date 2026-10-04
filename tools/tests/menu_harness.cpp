// Headless compile + behaviour check for the ETHNIR shell (v2: release-commit
// rows, child panes, auto save, segmented control, closing animation).
// ImGui's IM_ASSERT aborts on unbalanced Begin/End or PushStyleVar/Pop, so
// every frame below is also a structural test of the shell.
#define IMGUI_DEFINE_MATH_OPERATORS
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/ethnir_menu.h"
#include <cstdio>
#include <cstring>
#include <cmath>

ImFont* F50 = nullptr;
ImFont* F107 = nullptr;
namespace font { ImFont* inter_semibold = nullptr; }
float menu[4] = { 0.04f, 0.52f, 1.0f, 1.0f };

static int g_fail = 0;
static void CHECK(bool ok, const char* what)
{
    std::printf("%s %s\n", ok ? "ok  " : "FAIL", what);
    if (!ok) g_fail++;
}

static bool  tg[16]  = { true, false, true, true, false, true, false, false,
                         true, false, true, false, true, false, true, false };
static float sl[4]   = { 50.0f, 35.0f, 1.0f, 4.0f };
static int   cb[4]   = { 0, 1, 0, 2 };
static float col4[4] = { 0.0f, 212.0f, 255.0f, 255.0f };
static int   seg     = 0;
static char  lb[16][32];
static int   g_saves = 0;

static void FakeTab(int tab)
{
    using namespace ethnir;
    static const char* optsA[] = { "Head", "Chest", "Body" };
    static const char* optsB[] = { "None", "Shooting", "Scoping" };

    if (tab == 0 || tab == 1 || tab == 2)
    {
        BeginColumns();
        SectionLabel("COLUMN A");
        BeginGroupCard("eth_a");
        for (int i = 0; i < 8; ++i)
        {
            std::snprintf(lb[i], sizeof(lb[i]), "Row %d##r%d", i, i);   // id suffix must never print
            RowToggle(ICON_FA_EYE, lb[i], &tg[i]);
        }
        EndGroupCard();
        NextColumn();
        SectionLabel("COLUMN B");
        BeginGroupCard("eth_b");
        ComboRow(ICON_FA_SLIDERS_H, "Mode", &cb[0], optsA, 3);
        ComboRow(ICON_FA_SLIDERS_H, "Trigger", &cb[1], optsB, 3);
        RowSlider(ICON_FA_SLIDERS_H, "Smoothing", &sl[0], 0.0f, 100.0f, "%.0f");
        RowSlider(ICON_FA_SLIDERS_H, "Distance", &sl[1], 0.0f, 100.0f, "%.0f");
        ColorRow(ICON_FA_EYE, "ESP colour", col4);
        EndGroupCard();
        EndColumns();
    }
    else if (tab == 3)
    {
        SectionLabel("SKINS");
        BeginGroupCard("eth_skins");
        for (int i = 8; i < 12; ++i)
        {
            std::snprintf(lb[i], sizeof(lb[i]), "Camo %d", i - 8);
            RowToggle(nullptr, lb[i], &tg[i]);
        }
        EndGroupCard();
    }
    else if (tab == 4)
    {
        SectionLabel("CHANGELOG");
        BeginGroupCard("eth_misc");
        RowSlider(ICON_FA_COG, "Speed", &sl[2], 0.5f, 2.0f, "%.1fx");
        EndGroupCard();
    }
    else
    {
        SectionLabel("CONFIG");
        BeginGroupCard("eth_cfg");
        static const char* sizes[] = { "Small", "Big" };
        SegmentedRow("UI size", &seg, sizes, 2);
        RowSlider(ICON_FA_COG, "UI scale", &sl[3], 0.5f, 4.0f, "%.2fx");
        EndGroupCard();
    }
}

static ethnir::MenuState g_st;
static ImGuiIO* g_io = nullptr;

static void Frame()
{
    ImGui::NewFrame();
    ethnir::Render(g_st);
    ImGui::Render();
}

static void Press(float x, float y)
{
    g_io->AddMousePosEvent(x, y);
    g_io->AddMouseButtonEvent(0, true);
    Frame();
}

static void Release()
{
    g_io->AddMouseButtonEvent(0, false);
    Frame();
}

static void Click(float x, float y)
{
    Press(x, y);
    Release();
}

int main()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    g_io = &ImGui::GetIO();
    g_io->DisplaySize = ImVec2(1280.0f, 720.0f);
    g_io->DeltaTime = 1.0f / 60.0f;
    g_io->IniFilename = nullptr;
    g_io->LogFilename = nullptr;
    g_io->Fonts->AddFontDefault();
    {
        unsigned char* px = nullptr; int fw = 0, fh = 0;
        g_io->Fonts->GetTexDataAsRGBA32(&px, &fw, &fh);
        g_io->Fonts->SetTexID((ImTextureID)(intptr_t)1);
    }

    g_st.DrawTab = FakeTab;
    g_st.OnSave = []() { g_saves++; };

    const float expectX = (1280.0f - 880.0f) * 0.5f;
    const float expectY = (720.0f - 520.0f) * 0.5f;
    for (int i = 0; i < 40; ++i) Frame();
    CHECK(std::fabs(g_st.Appear - 1.0f) < 0.001f, "open animation settles at 1");
    CHECK(std::fabs(g_st.Fade - 1.0f) < 0.001f, "tab fade settles at 1");
    CHECK(std::fabs(g_st.WinPos.x - expectX) < 1.0f && std::fabs(g_st.WinPos.y - expectY) < 1.0f,
          "window is centred on first open");
    CHECK(ImGui::GetDrawData()->CmdListsCount > 1, "shell + content produced draw commands");
    CHECK(g_st.LastRowCount > 0, "row counter reported rows for the search hint");

    // separate BeginChild panes for sidebar and content (names carry an id
    // suffix in this ImGui version, so match by substring)
    {
        bool navFound = false, contentFound = false;
        ImGuiContext* ctx = ImGui::GetCurrentContext();
        for (int wi = 0; wi < ctx->Windows.Size; ++wi)
        {
            const char* n = ctx->Windows[wi]->Name;
            if (n == nullptr) continue;
            if (strstr(n, "##ethnir_nav") != nullptr) navFound = true;
            if (strstr(n, "##ethnir_content") != nullptr) contentFound = true;
        }
        CHECK(navFound, "sidebar runs in its own child pane");
        CHECK(contentFound, "content runs in its own child pane");
    }

    // label ids are stripped before drawing
    {
        char out[128];
        ethnir::EqStripId("Aimbone##combo_1", out, 128);
        CHECK(std::strcmp(out, "Aimbone") == 0, "EqStripId removes the ##id suffix");
        ethnir::EqStripId("Plain", out, 128);
        CHECK(std::strcmp(out, "Plain") == 0, "EqStripId leaves plain labels alone");
    }

    // sidebar navigation: row i=1 = Players (tab 0)
    Click(300.0f, 211.0f);
    for (int i = 0; i < 20; ++i) Frame();
    CHECK(g_st.ActiveTab == 0, "clicking the Players row activates tab 0");
    CHECK(g_st.LastTab == 0, "tab switch tracked for the fade");

    // toggle commits on release
    const bool before = tg[0];
    Click(600.0f, 205.0f);
    for (int i = 0; i < 3; ++i) Frame();
    CHECK(tg[0] != before, "tapping a row flips its toggle on release");
    CHECK(g_st.Dirty, "the change marks the config dirty for auto save");

    // press, drag away, release: nothing commits (scroll safety)
    const bool before2 = tg[1];
    Press(600.0f, 237.0f);
    g_io->AddMousePosEvent(900.0f, 320.0f);
    Frame();
    g_io->AddMouseButtonEvent(0, false);
    Frame();
    for (int i = 0; i < 3; ++i) Frame();
    CHECK(tg[1] == before2, "dragging off a pressed row never flips it");

    // slider drag on the right column track
    const float beforeSlider = sl[0];
    Press(974.0f, 270.0f);
    CHECK(g_st.InputActive, "holding a slider reports InputActive (auto save pauses)");
    g_io->AddMousePosEvent(1044.0f, 270.0f);
    Frame();
    Release();
    for (int i = 0; i < 3; ++i) Frame();
    CHECK(sl[0] != beforeSlider, "dragging the track changes the slider value");
    CHECK(!g_st.InputActive, "InputActive clears on release");

    // auto save: debounced, never mid-drag
    const int savesBefore = g_saves;
    g_st.Dirty = true;
    for (int i = 0; i < 40; ++i) Frame();               // 0.66s of idle
    CHECK(g_saves == savesBefore + 1, "auto save fires once after the 500ms debounce");

    g_st.Dirty = true;
    Press(1000.0f, 270.0f);                             // hold on the track, away from the panel
    for (int i = 0; i < 60; ++i) { g_io->AddMousePosEvent(1000.0f - i, 270.0f); Frame(); }
    CHECK(g_saves == savesBefore + 1, "auto save never fires while a drag is held");
    Release();
    for (int i = 0; i < 45; ++i) Frame();
    CHECK(g_saves >= savesBefore + 2, "auto save fires after the drag ends");

    // search filter
    std::strncpy(g_st.Search, "Row 3", sizeof(g_st.Search) - 1);
    for (int i = 0; i < 3; ++i) Frame();
    CHECK(g_st.LastRowCount > 1, "hint keeps counting every function while filtering");
    CHECK(g_st.LastSearchHit, "search reports a hit");
    const bool row0Before = tg[0];
    Click(600.0f, 205.0f);                              // "Row 0" is filtered out
    for (int i = 0; i < 3; ++i) Frame();
    CHECK(tg[0] == row0Before, "filtered-out rows are not clickable");
    std::strncpy(g_st.Search, "zzz", sizeof(g_st.Search) - 1);
    for (int i = 0; i < 3; ++i) Frame();
    CHECK(!g_st.LastSearchHit, "search reports no hit for a missing keyword");
    g_st.Search[0] = 0;
    for (int i = 0; i < 3; ++i) Frame();

    // header: Save pill (one-frame pulse, read right after the release frame)
    Click(460.0f, 131.0f);
    CHECK(g_st.HeaderPressed == 0, "Save reports HeaderPressed 0 on release");
    CHECK(g_st.SaveFlash > 0.0f, "Save shows the confirmation chip");
    for (int i = 0; i < 3; ++i) Frame();
    CHECK(!g_st.Dirty, "manual save clears the dirty flag");

    // gear toggles the quick settings panel
    Click(1050.0f, 131.0f);
    Frame();
    CHECK(!g_st.ShowSettingsPanel, "gear toggles the settings panel off");
    Click(1050.0f, 131.0f);
    Frame();
    CHECK(g_st.ShowSettingsPanel, "gear toggles the settings panel back on");

    // segmented control in the panel: second segment = Light
    Click(1200.0f, 196.0f);
    Frame();
    CHECK(!g_st.Dark, "panel segmented control switches to Light");
    Click(1120.0f, 196.0f);
    Frame();
    CHECK(g_st.Dark, "panel segmented control switches back to Dark");

    // minimise: fades out first, then reports TrafficPressed exactly when the
    // appear animation has decayed to zero
    Click(1014.0f, 131.0f);
    bool sawTraffic = false;
    for (int i = 0; i < 40; ++i)
    {
        Frame();
        if (g_st.TrafficPressed == 0 && g_st.Appear <= 0.0f) sawTraffic = true;
    }
    CHECK(sawTraffic, "minimise reports TrafficPressed after the fade-out");

    g_st.Open = true;
    for (int i = 0; i < 30; ++i) Frame();
    CHECK(std::fabs(g_st.Appear - 1.0f) < 0.001f, "reopens and re-runs the appear animation");

    // help chip
    Click(241.0f, 579.0f);
    Frame();
    CHECK(g_st.HelpOpen, "help chip opens the help card");
    Click(700.0f, 400.0f);
    Frame();
    CHECK(!g_st.HelpOpen, "tapping outside closes the help card");

    // accent + theme plumbing
    const float hueBefore = main_runtime_theme::g_menuHue;
    ethnir::EqApplyAccentIndex(3);
    CHECK(main_runtime_theme::g_menuHue != hueBefore, "accent index rewrites the theme hue");
    CHECK(std::fabs(main_runtime_theme::g_menuHue - ethnir::kAccents[3].hue) < 0.0001f, "accent hue matches the preset");
    CHECK(menu[0] > 0.90f && menu[2] < 0.45f, "gold accent reached the host menu[] colour");
    ethnir::EqApplyAccentIndex(0);
    CHECK(std::fabs(menu[0] - 10.0f / 255.0f) < 0.02f && std::fabs(menu[1] - 132.0f / 255.0f) < 0.02f,
          "default accent is iOS blue #0A84FF");
    CHECK(std::fabs(ethnir::kSwitchOn.z - 0.349f) < 0.001f, "switches use iOS green #34C759");

    g_st.Dark = false;
    for (int i = 0; i < 10; ++i) Frame();
    g_st.Dark = true;
    for (int i = 0; i < 5; ++i) Frame();
    CHECK(true, "light palette renders");

    g_st.Backdrop = nullptr;
    for (int i = 0; i < 3; ++i) Frame();
    g_st.Backdrop = (ImTextureID)(intptr_t)1;
    for (int i = 0; i < 3; ++i) Frame();
    g_st.Backdrop = nullptr;
    CHECK(true, "both backdrop paths render (gradient fallback + bound texture)");

    g_st.AnimSpeed = 0.5f;
    for (int i = 0; i < 5; ++i) Frame();
    g_st.AnimSpeed = 2.0f;
    for (int i = 0; i < 5; ++i) Frame();
    g_st.AnimSpeed = 1.0f;
    CHECK(true, "animation speed 0.5x and 2.0x render");

    for (int t = 0; t < 6; ++t)
    {
        g_st.ActiveTab = t;
        for (int i = 0; i < 6; ++i) Frame();
    }
    CHECK(seg == 0 || seg == 1, "segmented control stayed consistent across tabs");
    CHECK(true, "all six tabs render");

    // hit-test sweep: no asserts from overlapping hit boxes
    g_st.ShowSettingsPanel = true;
    g_st.HelpOpen = true;
    for (float y = 110.0f; y < 620.0f; y += 40.0f)
        for (float x = 210.0f; x < 1270.0f; x += 40.0f)
            Click(x, y);
    CHECK(true, "hit-test sweep over the shell and panel completed");

    std::printf("\n%s (%d failure%s)\n", g_fail == 0 ? "ETHNIR CHECK PASSED" : "ETHNIR CHECK FAILED",
                g_fail, g_fail == 1 ? "" : "s");
    ImGui::DestroyContext();
    return g_fail == 0 ? 0 : 1;
}
