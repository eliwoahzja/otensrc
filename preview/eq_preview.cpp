// Headless preview harness for the Equinox menu.
//
// Renders the REAL src/main/jni/ImGui/equinox_menu.h using the project's own
// ImGui, fonts and glyph set. The game-side dependencies (Config, IL2Cpp,
// memory patches, GLES runtime) are not involved -- we only need enough of the
// host app to satisfy the menu's externs and give it representative content.
//
// Build: see build.sh next to this file.

#include "Call_ImGui.h"

#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <cmath>

// ---- font data, straight from the project --------------------------------
#include "../src/main/jni/Fonts/fonts.h"
#include "../src/main/jni/Fonts/SPECIAL.h"
#include "../src/main/jni/Fonts/Iconcpp.h"

#include "equinox_menu.h"

// ---- host-app symbols the menu header expects -----------------------------
// imgui_settings.h declares these extern; Main.cpp normally defines them.
float menu[4] = { 0.0f / 255.0f, 212.0f / 255.0f, 255.0f / 255.0f, 1.0f };
ImFont* F50 = nullptr;
ImFont* F107 = nullptr;

namespace font {
ImFont* inter_semibold = nullptr;
}

// ---------------------------------------------------------------------------
// Sample content, mirroring the shape of the real tabs so the layout is
// exercised realistically (sections, group cards, toggles, sliders, combos).
// ---------------------------------------------------------------------------
struct SampleConfig {
    bool line = true, box = false, skeleton = false, health = true;
    bool name = true, distance = false, count = true, alert = false;

    bool aim360 = true, bulletTrack = false;
    float aimAssist = 35.0f, fov = 45.0f;
    int  location = 0, trigger = 1, targetBy = 0;

    bool blueprint = true, hitbox = false, recoil = true, spread = false;
    bool shake = false, overheat = false, parachute = true, flash = false;
    bool firerate = true, diving = false, reload = true;

    float snowSpeed = 40.0f, slideDistance = 12.0f, speedHack = 1.5f, highJump = 2.0f;
};

static SampleConfig g_cfg;
static const char* kBoxTypes[]   = { "Fill", "Outline", "Corner", "3D" };
static const char* kLinePos[]    = { "Top", "Mid", "Bottom" };
static const char* kHealthPos[]  = { "Top", "Side" };
static const char* kEspStyles[]  = { "None", "3D Sphere", "Player Signal" };
static const char* kTargets[]    = { "Head", "Chest", "Body" };
static const char* kTriggers[]   = { "None", "Shooting", "Scoping" };
static const char* kTargetBy[]   = { "Distance", "FOV" };
static int g_boxType = 1, g_linePos = 1, g_healthPos = 1, g_espStyle = 0;

// Screen rect of the first ComboRow hitbox, captured from live ImGui state
// during layout so the popup-open shot clicks the real widget.
static ImVec2 g_comboMin(0.0f, 0.0f);
static ImVec2 g_comboMax(0.0f, 0.0f);
static bool g_comboRectValid = false;

static void DrawEspTab() {
    using namespace equinox;
    SectionLabel("ESP");
    BeginGroupCard("pv_esp");
    RowToggle(ICON_FA_EYE, "ESP Line", &g_cfg.line);
    RowToggle(ICON_FA_EYE, "ESP Box", &g_cfg.box);
    RowToggle(ICON_FA_EYE, "ESP Skeleton", &g_cfg.skeleton);
    RowToggle(ICON_FA_EYE, "ESP Health", &g_cfg.health);
    RowToggle(ICON_FA_EYE, "ESP Name", &g_cfg.name);
    RowToggle(ICON_FA_EYE, "ESP Distance", &g_cfg.distance);
    RowToggle(ICON_FA_EYE, "ESP Count", &g_cfg.count);
    RowToggle(ICON_FA_EYE, "360 Alert", &g_cfg.alert);
    EndGroupCard();

    SectionLabel("ESP OPTIONS");
    BeginGroupCard("pv_esp_opts");
    ComboRow(ICON_FA_SLIDERS_H, "Box Type", &g_boxType, kBoxTypes, IM_ARRAYSIZE(kBoxTypes));
    ComboRow(ICON_FA_SLIDERS_H, "Line Position", &g_linePos, kLinePos, IM_ARRAYSIZE(kLinePos));
    ComboRow(ICON_FA_SLIDERS_H, "Health Position", &g_healthPos, kHealthPos, IM_ARRAYSIZE(kHealthPos));
    ComboRow(ICON_FA_SLIDERS_H, "ESP Style", &g_espStyle, kEspStyles, IM_ARRAYSIZE(kEspStyles));
    EndGroupCard();
}

static void DrawAimTab() {
    using namespace equinox;
    SectionLabel("AIMBOTS");
    BeginGroupCard("pv_aim");
    RowToggle(ICON_FA_CROSSHAIRS, "Aimbot 360", &g_cfg.aim360);
    RowToggle(ICON_FA_CROSSHAIRS, "Bullet Track", &g_cfg.bulletTrack);
    RowSlider(ICON_FA_CROSSHAIRS, "Aim Assist Size", &g_cfg.aimAssist, 0.0f, 100.0f, "%.0f");
    EndGroupCard();
SectionLabel("COMBAT OPTIONS");
    BeginGroupCard("pv_combat");
    // Capture where the first combo row will land before it is submitted.
    // ComboRow submits an InvisibleButton of height 30 at the cursor, so the
    // cursor plus the full content width is exactly the widget's hitbox. The ESP
    // tab's combos scroll below the window bottom, so the AIM tab is used instead.
    {
        const ImVec2 c = ImGui::GetCursorScreenPos();
        const float w = ImGui::GetContentRegionAvail().x;
        g_comboMin = c;
        g_comboMax = ImVec2(c.x + w, c.y + 30.0f);
        g_comboRectValid = true;
    }
    ComboRow(ICON_FA_CROSSHAIRS, "Location", &g_cfg.location, kTargets, IM_ARRAYSIZE(kTargets));
    ComboRow(ICON_FA_CROSSHAIRS, "Trigger", &g_cfg.trigger, kTriggers, IM_ARRAYSIZE(kTriggers));
    ComboRow(ICON_FA_CROSSHAIRS, "Target By", &g_cfg.targetBy, kTargetBy, IM_ARRAYSIZE(kTargetBy));
    RowSlider(ICON_FA_CROSSHAIRS, "FOV Size", &g_cfg.fov, 0.0f, 100.0f, "%.0f");
    EndGroupCard();
}

static void DrawMemoryTab() {
    using namespace equinox;
    SectionLabel("MEMORY HACKS");
    BeginGroupCard("pv_mem");
    RowToggle(ICON_FA_BOLT, "Unlock Blueprint", &g_cfg.blueprint);
    RowToggle(ICON_FA_BOLT, "Hitbox", &g_cfg.hitbox);
    RowToggle(ICON_FA_BOLT, "No Recoil", &g_cfg.recoil);
    RowToggle(ICON_FA_BOLT, "No Spread", &g_cfg.spread);
    RowToggle(ICON_FA_BOLT, "No Shake", &g_cfg.shake);
    RowToggle(ICON_FA_BOLT, "No Overheat", &g_cfg.overheat);
    RowToggle(ICON_FA_BOLT, "No Parachute", &g_cfg.parachute);
    RowToggle(ICON_FA_BOLT, "Anti Flashbang", &g_cfg.flash);
    RowToggle(ICON_FA_BOLT, "Firerate", &g_cfg.firerate);
    RowToggle(ICON_FA_BOLT, "Fast Dive", &g_cfg.diving);
    RowToggle(ICON_FA_BOLT, "Fast Reload", &g_cfg.reload);
    EndGroupCard();

    SectionLabel("MOVEMENT SETTINGS");
    BeginGroupCard("pv_move");
    RowSlider(ICON_FA_SLIDERS_H, "Snowboard Speed", &g_cfg.snowSpeed, 0.0f, 100.0f, "%.1f");
    RowSlider(ICON_FA_SLIDERS_H, "Slide Distance", &g_cfg.slideDistance, 0.0f, 30.0f, "%.1f");
    RowSlider(ICON_FA_SLIDERS_H, "SpeedHack", &g_cfg.speedHack, 0.5f, 2.0f, "%.1fx");
    RowSlider(ICON_FA_SLIDERS_H, "High Jump", &g_cfg.highJump, 0.5f, 5.0f, "%.2fx");
    EndGroupCard();
}

static void DrawSkinsTab() {
    using namespace equinox;
    SectionLabel("CAMO MODIFIER");
    BeginGroupCard("pv_camo");
    RowToggle(nullptr, "Default / OFF", &g_cfg.line);
    RowToggle(nullptr, "Diamond Camo", &g_cfg.box);
    RowToggle(nullptr, "Red Sprite Camo", &g_cfg.skeleton);
    EndGroupCard();
    ImGui::Dummy(ImVec2(0, 4));
    ImGui::TextDisabled("Only applies to [M] Mythic and [L] Legendary weapon skins.");
}

static void DispatchTab(int tab) {
    switch (tab) {
    case 0: DrawEspTab(); break;
    case 1: DrawAimTab(); break;
    case 2: DrawMemoryTab(); break;
    case 3: DrawSkinsTab(); break;
    default: break;
    }
}

// ---------------------------------------------------------------------------
// Offscreen GL ES via surfaceless EGL (llvmpipe), read back to PNG.
// ---------------------------------------------------------------------------
static EGLDisplay g_dpy = EGL_NO_DISPLAY;
static EGLSurface g_surf = EGL_NO_SURFACE;
static EGLContext g_ctx = EGL_NO_CONTEXT;

static bool InitGL(int w, int h) {
    g_dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (g_dpy == EGL_NO_DISPLAY) return false;
    EGLint maj, min;
    if (!eglInitialize(g_dpy, &maj, &min)) return false;
    EGLint cfgAttr[] = {
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8, EGL_ALPHA_SIZE, 8,
        EGL_NONE
    };
    EGLConfig cfg; EGLint n = 0;
    if (!eglChooseConfig(g_dpy, cfgAttr, &cfg, 1, &n) || n < 1) return false;
    eglBindAPI(EGL_OPENGL_ES_API);
    EGLint pb[] = { EGL_WIDTH, w, EGL_HEIGHT, h, EGL_NONE };
    g_surf = eglCreatePbufferSurface(g_dpy, cfg, pb);
    if (g_surf == EGL_NO_SURFACE) return false;
    EGLint ctxAttr[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    g_ctx = eglCreateContext(g_dpy, cfg, EGL_NO_CONTEXT, ctxAttr);
    if (g_ctx == EGL_NO_CONTEXT) return false;
    return eglMakeCurrent(g_dpy, g_surf, g_surf, g_ctx);
}

// Minimal PNG writer (RGBA8, no interlace), using stored (uncompressed) deflate
// blocks so we do not need to link zlib.
struct PngWriter {
    FILE* f = nullptr;
    explicit PngWriter(const char* path) : f(fopen(path, "wb")) {}
    bool ok() const { return f != nullptr; }
    ~PngWriter() { if (f) fclose(f); }

    static unsigned int Crc(const unsigned char* buf, size_t len, unsigned int crc) {
        static unsigned int table[256];
        static bool init = false;
        if (!init) {
            for (unsigned int n = 0; n < 256; ++n) {
                unsigned int v = n;
                for (int k = 0; k < 8; ++k) v = (v & 1) ? (0xEDB88320u ^ (v >> 1)) : (v >> 1);
                table[n] = v;
            }
            init = true;
        }
        for (size_t i = 0; i < len; ++i) crc = table[(crc ^ buf[i]) & 0xFF] ^ (crc >> 8);
        return crc;
    }

    static void Be32(unsigned int v, unsigned char* out) {
        out[0] = (v >> 24) & 0xFF; out[1] = (v >> 16) & 0xFF;
        out[2] = (v >> 8) & 0xFF;  out[3] = v & 0xFF;
    }

    void Chunk(const char* type, const unsigned char* data, unsigned int len) {
        unsigned char hdr[4]; Be32(len, hdr); fwrite(hdr, 1, 4, f);
        unsigned int crc = 0xFFFFFFFFu;
        for (int i = 0; i < 4; ++i) {
            unsigned char c = (unsigned char)type[i];
            fwrite(&c, 1, 1, f);
            crc = Crc(&c, 1, crc);
        }
        if (len) { fwrite(data, 1, len, f); crc = Crc(data, len, crc); }
        crc ^= 0xFFFFFFFFu;
        unsigned char cb[4]; Be32(crc, cb); fwrite(cb, 1, 4, f);
    }

    bool Write(int w, int h, const std::vector<unsigned char>& rgba) {
        if (!f) return false;
        const unsigned char sig[8] = { 137, 'P', 'N', 'G', 13, 10, 26, 10 };
        fwrite(sig, 1, 8, f);

        unsigned char ihdr[13];
        Be32((unsigned int)w, ihdr); Be32((unsigned int)h, ihdr + 4);
        ihdr[8] = 8; ihdr[9] = 6; ihdr[10] = 0; ihdr[11] = 0; ihdr[12] = 0;
        Chunk("IHDR", ihdr, 13);

        std::vector<unsigned char> raw;
        raw.reserve((size_t)h * ((size_t)w * 4 + 1));
        for (int y = 0; y < h; ++y) {
            raw.push_back(0);
            const unsigned char* row = rgba.data() + (size_t)y * w * 4;
            raw.insert(raw.end(), row, row + (size_t)w * 4);
        }

        std::vector<unsigned char> z;
        z.push_back(0x78); z.push_back(0x01);
        size_t pos = 0;
        while (pos < raw.size()) {
            size_t n = raw.size() - pos;
            if (n > 65535) n = 65535;
            const bool last = (pos + n == raw.size());
            z.push_back(last ? 1 : 0);
            z.push_back((unsigned char)(n & 0xFF));
            z.push_back((unsigned char)((n >> 8) & 0xFF));
            z.push_back((unsigned char)((~n) & 0xFF));
            z.push_back((unsigned char)(((~n) >> 8) & 0xFF));
            z.insert(z.end(), raw.begin() + pos, raw.begin() + pos + n);
            pos += n;
        }
        unsigned int a = 1, b = 0;
        for (size_t i = 0; i < raw.size(); ++i) {
            a = (a + raw[i]) % 65521;
            b = (b + a) % 65521;
        }
        unsigned char ab[4]; Be32((b << 16) | a, ab);
        z.insert(z.end(), ab, ab + 4);
        Chunk("IDAT", z.data(), (unsigned int)z.size());
        Chunk("IEND", nullptr, 0);
        fflush(f);
        return true;
    }
};

static std::vector<unsigned char> ReadPixelsFlipV(int w, int h) {
    std::vector<unsigned char> src((size_t)w * h * 4);
    glReadPixels(0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, src.data());
    std::vector<unsigned char> dst((size_t)w * h * 4);
    for (int y = 0; y < h; ++y)
        memcpy(dst.data() + (size_t)y * w * 4,
               src.data() + (size_t)(h - 1 - y) * w * 4, (size_t)w * 4);
    return dst;
}

// A cheap synthetic "gameplay" backdrop so the glass transparency reads
// correctly in the screenshot.
static void DrawBackdrop() {
    ImDrawList* dl = ImGui::GetBackgroundDrawList();
    const ImVec2 s = ImGui::GetIO().DisplaySize;
    dl->AddRectFilled(ImVec2(0, 0), s, IM_COL32(28, 32, 26, 255));
    dl->AddRectFilledMultiColor(ImVec2(0, s.y * 0.62f), s,
                                IM_COL32(86, 78, 60, 255), IM_COL32(70, 64, 50, 255),
                                IM_COL32(46, 44, 38, 255), IM_COL32(52, 48, 40, 255));
    dl->AddRectFilledMultiColor(ImVec2(0, s.y * 0.58f), ImVec2(s.x, s.y * 0.66f),
                                IM_COL32(150, 130, 90, 0), IM_COL32(170, 150, 105, 90),
                                IM_COL32(170, 150, 105, 90), IM_COL32(150, 130, 90, 0));
    const float base = s.y * 0.62f;
    float x = 0.0f;
    int i = 0;
    while (x < s.x) {
        const float w = 40.0f + ((i * 37) % 5) * 16.0f;
        const float h = 60.0f + ((i * 53) % 7) * 34.0f;
        dl->AddRectFilled(ImVec2(x, base - h), ImVec2(x + w - 6.0f, base),
                          IM_COL32(38, 40, 36, 255));
        for (float wy = base - h + 10.0f; wy < base - 10.0f; wy += 18.0f)
            for (float wx = x + 8.0f; wx < x + w - 16.0f; wx += 16.0f)
                if (((int)(wx + wy) % 5) < 2)
                    dl->AddRectFilled(ImVec2(wx, wy), ImVec2(wx + 7.0f, wy + 10.0f),
                                      IM_COL32(210, 190, 120, 190));
        x += w;
        ++i;
    }
    auto figure = [&](float fx, float fy, float scale, ImU32 col) {
        dl->AddRectFilled(ImVec2(fx - 5 * scale, fy - 30 * scale),
                          ImVec2(fx + 5 * scale, fy), col, 3.0f);
        dl->AddRectFilled(ImVec2(fx - 3 * scale, fy - 38 * scale),
                          ImVec2(fx + 3 * scale, fy - 28 * scale), col, 2.0f);
        // a couple of "ESP" boxes over the figures so the menu is clearly an overlay
        dl->AddRect(ImVec2(fx - 16 * scale, fy - 44 * scale),
                    ImVec2(fx + 16 * scale, fy + 2 * scale), col, 2.0f);
    };
    figure(s.x * 0.22f, base - 4.0f, 1.6f, IM_COL32(255, 90, 90, 255));
    figure(s.x * 0.74f, base - 2.0f, 1.2f, IM_COL32(120, 220, 255, 255));
}

// ---------------------------------------------------------------------------

int main(int argc, char** argv) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    const int W = 1080, H = 1920;
    io.DisplaySize = ImVec2((float)W, (float)H);
    io.DeltaTime = 1.0f / 60.0f;
    // The harness keeps its own layout state, so do not let ImGui read or write
    // an imgui.ini next to the repo.
    io.IniFilename = nullptr;

    if (!InitGL(W, H)) { fprintf(stderr, "FATAL: EGL init failed\n"); return 1; }
    printf("GL_VERSION  = %s\n", glGetString(GL_VERSION));
    printf("GL_RENDERER = %s\n", glGetString(GL_RENDERER));

    ImGui_ImplOpenGL3_Init("#version 300 es");

    // The font blobs are declared const in headers, but ImGui casts away const
    // and frees them unless FontDataOwnedByAtlas is false -- and even then the
    // ownership flags differ per overload. Copy into heap buffers we own so
    // nothing ever tries to free static storage.
    auto CopyBytes = [](const void* src, size_t n) -> void* {
        void* p = malloc(n);
        memcpy(p, src, n);
        return p;
    };

    ImFontConfig interCfg;
    interCfg.MergeMode = false;
    interCfg.PixelSnapH = true;
    interCfg.FontDataOwnedByAtlas = true;
    font::inter_semibold = io.Fonts->AddFontFromMemoryTTF(
        CopyBytes(inter_semibold, sizeof(inter_semibold)), sizeof(inter_semibold), 16.0f, &interCfg);

    ImFontConfig pageCfg;
    pageCfg.MergeMode = false;
    pageCfg.PixelSnapH = true;
    pageCfg.FontDataOwnedByAtlas = true;
    io.Fonts->AddFontFromMemoryTTF(
        CopyBytes(icomoon_page, sizeof(icomoon_page)), sizeof(icomoon_page), 18.0f, &pageCfg);

    static const ImWchar iconRanges[] = { 0xe000, 0xf8ff, 0 };
    ImFontConfig iconCfg;
    iconCfg.MergeMode = true;
    iconCfg.PixelSnapH = true;
    iconCfg.OversampleH = 2.5f;
    iconCfg.OversampleV = 2.5f;
    iconCfg.FontDataOwnedByAtlas = true;
    F107 = io.Fonts->AddFontFromMemoryCompressedTTF(
        CopyBytes(font_awesome_data1, (size_t)font_awesome_size1),
        (int)font_awesome_size1, 25.0f, &iconCfg, iconRanges);

    F50 = io.Fonts->AddFontFromMemoryTTF(
        CopyBytes(F50_data, F50_size), F50_size, 30.0f, nullptr,
        io.Fonts->GetGlyphRangesDefault());
    if (!F107) F107 = font::inter_semibold;
    if (font::inter_semibold) io.FontDefault = font::inter_semibold;
    // Main.cpp builds the atlas explicitly; keep that order so the harness
    // exercises the same first-frame path as the device.
    io.Fonts->Build();
    ImGui_ImplOpenGL3_CreateFontsTexture();

    if (!font::inter_semibold) { fprintf(stderr, "FATAL: inter font missing\n"); return 1; }
    if (!F50)                 { fprintf(stderr, "FATAL: F50 missing\n"); return 1; }
    if (!F107)                { fprintf(stderr, "FATAL: F107 icon font missing\n"); return 1; }
    printf("fonts: inter=%p F50=%p F107=%p\n",
           (void*)font::inter_semibold, (void*)F50, (void*)F107);

    equinox::MenuState menu;
    menu.DrawTab = DispatchTab;

    const char* outDir = (argc > 1) ? argv[1] : ".";
    char path[512];
    int failures = 0;

    struct Shot { const char* name; int tab; const char* search; int frames; };
    static const Shot kShots[] = {
        { "01_tab_aim",     1, "",    60 },
        { "02_tab_esp",     0, "",    30 },
        { "03_tab_memory",  2, "",    30 },
        { "04_tab_skins",   3, "",    30 },
        { "05_search_hit",  0, "esp", 40 },
        { "06_search_none", 0, "zzzz",40 },
        { "07_combo_open",  1, "",    40 },
    };

    for (size_t si = 0; si < IM_ARRAYSIZE(kShots); ++si) {
        const Shot& s = kShots[si];
        menu.ActiveTab = s.tab;
        snprintf(menu.Search, sizeof(menu.Search), "%s", s.search);

        for (int f = 0; f < s.frames; ++f) {
            main_runtime_theme::ApplyAccentFromHue();
            main_runtime_theme::ApplyThemeState();

            // Feed synthetic input BEFORE NewFrame(): ImGui only drains the event
            // queue inside NewFrame, so events pushed afterwards would be applied
            // one frame late and the click would never land.
            // The popup shot presses the real combo hitbox (rect captured from the
            // previous frame's layout) and then holds the mouse over it.
            const bool isComboShot = (strcmp(s.name, "07_combo_open") == 0);
            const bool pressFrame  = isComboShot && (f == s.frames - 3);
            const bool holdFrame   = isComboShot && (f >= s.frames - 2);
            if (pressFrame || holdFrame) {
                if (g_comboRectValid) {
                    io.AddMousePosEvent((g_comboMin.x + g_comboMax.x) * 0.5f,
                                        (g_comboMin.y + g_comboMax.y) * 0.5f);
                    io.AddMouseButtonEvent(0, pressFrame);
                }
            } else {
                io.AddMousePosEvent(-1.0f, -1.0f);
                io.AddMouseButtonEvent(0, false);
            }

            ImGui_ImplOpenGL3_NewFrame();
            ImGui::NewFrame();
            DrawBackdrop();

            equinox::Render(menu);

            ImGui::Render();
            glViewport(0, 0, W, H);
            glClearColor(0.10f, 0.11f, 0.09f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glFinish();
        }

        snprintf(path, sizeof(path), "%s/%s.png", outDir, s.name);
        std::vector<unsigned char> px = ReadPixelsFlipV(W, H);
        PngWriter pw(path);
        if (!pw.ok() || !pw.Write(W, H, px)) {
            fprintf(stderr, "FAIL: could not write %s\n", path);
            ++failures;
            continue;
        }
        bool varied = false;
        for (size_t i = 0; i + 4 < px.size(); i += 4 * 997)
            if (px[i] != px[0] || px[i + 1] != px[1] || px[i + 2] != px[2]) { varied = true; break; }
        if (!varied) { fprintf(stderr, "FAIL: %s looks blank\n", path); ++failures; }
        else printf("wrote %s\n", path);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    eglDestroyContext(g_dpy, g_ctx);
    eglDestroySurface(g_dpy, g_surf);
    eglTerminate(g_dpy);

    if (failures) { fprintf(stderr, "%d shot(s) failed\n", failures); return 1; }
    printf("ALL SHOTS OK\n");
    return 0;
}