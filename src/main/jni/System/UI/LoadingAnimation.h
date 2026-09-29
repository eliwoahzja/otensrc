#pragma once

#include "ImGui/Call_ImGui.h"
#include "../../ImGui/imgui_settings.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cstdio>
#include <string>
#include <vector>

extern ImFont* F50;
namespace font { extern ImFont* inter_semibold; }

// ═══════════════════════════════════════════════════════════════════
//  PSYCHE — GLITCH TERMINAL LOADER
//  Concept: corrupted holographic data panel
//  No circles. No hex grids. Pure raw terminal energy.
// ═══════════════════════════════════════════════════════════════════

namespace ui_loading {

// ── Internal state ──────────────────────────────────────────────────
static bool  showLoadingAnimation     = false;
static float loadingAnimationTimer    = 0.0f;
static float loadingAnimationDuration = 6.0f;
static float animationTime            = 0.0f;
static bool  seededRandom             = false;

// Glitch blocks — random screen-tear rectangles
struct GlitchBlock {
    float x, y, w, h;
    float life, maxLife;
    float offsetX;
    ImU32 col;
};
static std::vector<GlitchBlock> glitchBlocks;
static float glitchSpawnTimer = 0.0f;

// Data stream columns — falling characters
struct DataCol {
    float x;
    std::vector<float> charY;
    std::vector<int>   charIdx;
    std::vector<float> charLife;
    float speed;
    float spawnTimer;
};
static std::vector<DataCol> dataCols;

// Scan bars — horizontal sweeping lines
struct ScanBar {
    float y;
    float speed;
    float alpha;
    float width;
};
static std::vector<ScanBar> scanBars;

// Typewriter decrypt state per step label
struct DecryptState {
    char  display[64];
    float timer;
    int   revealedChars;
    bool  done;
};
static DecryptState decryptStates[3];
static const char* stepLabels[3] = {
    "LICENSE_VALIDATION.exe",
    "MODULE_SYNC.dll",
    "INTERFACE_BOOTSTRAP.sys"
};
static float stepThresholds[3] = { 0.0f, 0.34f, 0.68f };

// Random char pool for decrypt effect
static const char kGlitchChars[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*<>?/|\\[]{}~";

inline void SeedOnce() {
    if (!seededRandom) { std::srand((unsigned)std::time(nullptr)); seededRandom = true; }
}
inline float Rnd01()  { return (std::rand() % 10000) / 10000.0f; }
inline float RndRange(float a, float b) { return a + Rnd01() * (b - a); }
inline int   RndInt(int n) { return std::rand() % n; }

// ── Easing ──────────────────────────────────────────────────────────
inline float EaseOutExpo(float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return t >= 1.0f ? 1.0f : 1.0f - std::pow(2.0f, -10.0f * t);
}
inline float EaseInOutQuart(float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return t < 0.5f ? 8.0f * t * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 4.0f) / 2.0f;
}

// ── Init ─────────────────────────────────────────────────────────────
inline void InitLoader(float panelW, float panelH) {
    SeedOnce();
    glitchBlocks.clear();
    dataCols.clear();
    scanBars.clear();

    // Data stream columns across panel
    const int colCount = (int)(panelW / 18.0f);
    for (int i = 0; i < colCount; ++i) {
        DataCol col;
        col.x         = i * 18.0f + RndRange(0.0f, 8.0f);
        col.speed     = RndRange(35.0f, 110.0f);
        col.spawnTimer= RndRange(0.0f, 1.2f);
        const int len = RndInt(10) + 4;
        for (int j = 0; j < len; ++j) {
            col.charY.push_back(RndRange(-panelH, 0.0f) - j * 14.0f);
            col.charIdx.push_back(RndInt((int)sizeof(kGlitchChars) - 1));
            col.charLife.push_back(RndRange(0.0f, 1.0f));
        }
        dataCols.push_back(col);
    }

    // Horizontal scan bars
    for (int i = 0; i < 3; ++i) {
        ScanBar sb;
        sb.y     = RndRange(0.0f, panelH);
        sb.speed = RndRange(40.0f, 120.0f) * (RndInt(2) ? 1.0f : -1.0f);
        sb.alpha = RndRange(0.04f, 0.11f);
        sb.width = RndRange(panelH * 0.12f, panelH * 0.28f);
        scanBars.push_back(sb);
    }

    // Init decrypt states
    for (int i = 0; i < 3; ++i) {
        std::memset(decryptStates[i].display, 0, sizeof(decryptStates[i].display));
        decryptStates[i].timer        = 0.0f;
        decryptStates[i].revealedChars= 0;
        decryptStates[i].done         = false;
    }
}

inline void Start(float durationSeconds = 6.0f) {
    showLoadingAnimation     = true;
    loadingAnimationTimer    = 0.0f;
    loadingAnimationDuration = std::max(0.4f, durationSeconds);
    animationTime            = 0.0f;
    glitchSpawnTimer         = 0.0f;
    // Panel size estimate for init — actual layout computed in RenderWindow
    InitLoader(680.0f, 360.0f);
}

inline bool  IsActive()    { return showLoadingAnimation; }
inline float GetProgress() {
    if (loadingAnimationDuration <= 0.0f) return 1.0f;
    return std::clamp(loadingAnimationTimer / loadingAnimationDuration, 0.0f, 1.0f);
}

// ══════════════════════════════════════════════════════════════════
//  DRAW HELPERS
// ══════════════════════════════════════════════════════════════════

// Diagonal slash — draws a filled parallelogram (screen-tear style)
inline void DrawSlashRect(ImDrawList* draw,
                          float x, float y, float w, float h,
                          float slant, ImU32 col)
{
    // slant = how far the top-right shifts right vs bottom-right
    draw->AddQuadFilled(
        ImVec2(x + slant, y),
        ImVec2(x + w + slant, y),
        ImVec2(x + w, y + h),
        ImVec2(x, y + h),
        col);
}

// Dashed horizontal line
inline void DrawDashedLine(ImDrawList* draw,
                           ImVec2 p0, ImVec2 p1,
                           ImU32 col, float thick,
                           float dashLen, float gapLen)
{
    const float dx   = p1.x - p0.x;
    const float dy   = p1.y - p0.y;
    const float total= std::sqrt(dx * dx + dy * dy);
    const float nx   = dx / total, ny = dy / total;
    float dist = 0.0f;
    bool  dash = true;
    while (dist < total) {
        const float segLen = std::min(dash ? dashLen : gapLen, total - dist);
        if (dash) {
            draw->AddLine(
                ImVec2(p0.x + nx * dist,           p0.y + ny * dist),
                ImVec2(p0.x + nx * (dist + segLen), p0.y + ny * (dist + segLen)),
                col, thick);
        }
        dist += segLen;
        dash = !dash;
    }
}

// Corner L-bracket decorator
inline void DrawCornerBracket(ImDrawList* draw, ImVec2 origin,
                               float sx, float sy,
                               float len, ImU32 col, float thick = 1.5f)
{
    draw->AddLine(origin, ImVec2(origin.x + sx * len, origin.y), col, thick);
    draw->AddLine(origin, ImVec2(origin.x, origin.y + sy * len), col, thick);
}

// Vertical bar graph (frequency/signal style)
inline void DrawSignalBars(ImDrawList* draw, ImVec2 origin,
                           float totalW, float maxH,
                           float time, ImU32 col, ImU32 colDim)
{
    const int bars = 16;
    const float bw = totalW / bars - 2.0f;
    for (int i = 0; i < bars; ++i) {
        const float x  = origin.x + i * (bw + 2.0f);
        const float h  = maxH * (0.3f + 0.7f * std::abs(
            std::sin(time * 3.1f + i * 0.72f) *
            std::cos(time * 1.8f + i * 0.41f)));
        // Dim background bar
        draw->AddRectFilled(
            ImVec2(x, origin.y - maxH),
            ImVec2(x + bw, origin.y),
            colDim, 1.0f);
        // Active bar
        draw->AddRectFilled(
            ImVec2(x, origin.y - h),
            ImVec2(x + bw, origin.y),
            col, 1.0f);
        // Top bright cap
        draw->AddRectFilled(
            ImVec2(x, origin.y - h - 2.0f),
            ImVec2(x + bw, origin.y - h),
            IM_COL32(255, 245, 180, 180), 0.0f);
    }
}

// ══════════════════════════════════════════════════════════════════
//  MAIN RENDER
// ══════════════════════════════════════════════════════════════════
inline bool RenderWindow(ImTextureID /*backgroundTexture*/ = nullptr) {
    if (!showLoadingAnimation) return false;

    ImGuiIO&     io       = ImGui::GetIO();
    const float  dt       = io.DeltaTime;
    animationTime        += dt;
    const float  progress = GetProgress();

    const ImVec2 vc = ImGui::GetMainViewport()->GetCenter();
    const ImVec2 PS(std::min(700.0f, io.DisplaySize.x - 32.0f),
                    std::min(380.0f, io.DisplaySize.y - 48.0f));

    ImGui::SetNextWindowPos(vc, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(PS, ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.0f);

    bool finished = false;

    if (ImGui::Begin("##psyche_glitch_loader", nullptr,
        ImGuiWindowFlags_NoBackground    |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoScrollbar     |
        ImGuiWindowFlags_NoTitleBar      |
        ImGuiWindowFlags_NoResize        |
        ImGuiWindowFlags_NoCollapse      |
        ImGuiWindowFlags_NoMove))
    {
        ImDrawList* draw = ImGui::GetWindowDrawList();
        const ImVec2 P   = ImGui::GetWindowPos(); // top-left of window

        // ── Accent colours (green terminal feel — swap c::accent if you want) ──
        // Using c::accent as base but shifting to feel like a holo-terminal
        // MODIFIED: Shifted to custom high-tier industrial Grey + Vivid Yellow theme
        const ImVec4 av(240.0f / 255.0f, 190.0f / 255.0f, 20.0f / 255.0f, 1.0f);   // Tech Yellow primary accent
        const ImVec4 av2(140.0f / 255.0f, 142.0f / 255.0f, 145.0f / 255.0f, 1.0f); // Mechanical Grey secondary accent

        auto A  = [&](float a) { return ImGui::GetColorU32(ImVec4(av.x,  av.y,  av.z,  a)); };
        auto A2 = [&](float a) { return ImGui::GetColorU32(ImVec4(av2.x, av2.y, av2.z, a)); };

        // ════════════════════════════════════════════════════════
        //  PANEL BACKGROUND
        // ════════════════════════════════════════════════════════

        // Deep shadow behind panel
        for (int sh = 5; sh >= 1; --sh) {
            const float e = sh * 5.0f;
            draw->AddRectFilled(
                ImVec2(P.x - e, P.y - e),
                ImVec2(P.x + PS.x + e, P.y + PS.y + e),
                IM_COL32(0, 0, 0, 14), 4.0f);
        }

        // Main BG — very dark, near-black with slight tint
        draw->AddRectFilled(P, ImVec2(P.x + PS.x, P.y + PS.y),
            IM_COL32(18, 19, 21, 248), 2.0f);

        // Diagonal stripe texture overlay (slanted grid)
        {
            const float stride = 18.0f;
            for (float sx = -PS.y; sx < PS.x + PS.y; sx += stride) {
                DrawSlashRect(draw,
                    P.x + sx, P.y, 2.0f, PS.y,
                    PS.y * 0.55f,
                    IM_COL32(255, 255, 255, 5));
            }
        }

        // Top-left accent diagonal block (visual anchor)
        DrawSlashRect(draw,
            P.x, P.y, PS.x * 0.38f, 3.0f,
            14.0f, A(0.85f));
        DrawSlashRect(draw,
            P.x, P.y + 7.0f, PS.x * 0.20f, 1.5f,
            8.0f, A(0.40f));

        // Bottom-right accent diagonal block
        DrawSlashRect(draw,
            P.x + PS.x * 0.62f, P.y + PS.y - 3.0f, PS.x * 0.38f, 3.0f,
            -14.0f, A(0.65f));

        // Outer border — thin with corner emphasis
        draw->AddRect(P, ImVec2(P.x + PS.x, P.y + PS.y),
            A2(0.20f), 0.0f, 0, 1.0f);

        // Corner brackets — all 4 corners
        DrawCornerBracket(draw, P,                                     1, 1,  22.0f, A(0.80f), 2.0f);
        DrawCornerBracket(draw, ImVec2(P.x + PS.x, P.y),             -1, 1,  22.0f, A(0.80f), 2.0f);
        DrawCornerBracket(draw, ImVec2(P.x, P.y + PS.y),              1, -1, 22.0f, A(0.80f), 2.0f);
        DrawCornerBracket(draw, ImVec2(P.x + PS.x, P.y + PS.y),     -1, -1, 22.0f, A(0.80f), 2.0f);

        // ════════════════════════════════════════════════════════
        //  DATA STREAM (matrix rain — left half bg)
        // ════════════════════════════════════════════════════════
        {
            // Clip to left portion (decorative only)
            const float streamW = PS.x * 0.42f;
            draw->PushClipRect(P, ImVec2(P.x + streamW, P.y + PS.y), true);

            ImFont* fnt = ImGui::GetFont();
            const float fSz = 10.0f;

            for (auto& col : dataCols) {
                if (col.x > streamW) continue;
                col.spawnTimer -= dt;
                for (int j = 0; j < (int)col.charY.size(); ++j) {
                    col.charY[j] += dt * col.speed;
                    col.charLife[j] -= dt * 0.3f;
                    if (col.charLife[j] < 0.0f || col.charY[j] > PS.y + fSz) {
                        col.charY[j]   = -fSz * RndRange(1.0f, 4.0f);
                        col.charIdx[j] = RndInt((int)sizeof(kGlitchChars) - 1);
                        col.charLife[j]= RndRange(0.5f, 1.0f);
                    }
                    // Randomise char occasionally
                    if (RndInt(120) == 0)
                        col.charIdx[j] = RndInt((int)sizeof(kGlitchChars) - 1);

                    const float life = col.charLife[j];
                    const bool  isHead = (j == 0);
                    const float alpha  = isHead ? life * 0.85f : life * 0.22f;
                    char ch[2] = { kGlitchChars[col.charIdx[j]], '\0' };
                    draw->AddText(fnt, fSz,
                        ImVec2(P.x + col.x, P.y + col.charY[j]),
                        isHead ? A(alpha) : A2(alpha * 0.70f),
                        ch);
                }
            }

            draw->PopClipRect();

            // Gradient fade — left stream fades into right content
            draw->AddRectFilledMultiColor(
                ImVec2(P.x + streamW * 0.55f, P.y),
                ImVec2(P.x + streamW, P.y + PS.y),
                IM_COL32(0,0,0,0), IM_COL32(0,0,0,0),
                IM_COL32(18,19,21,245), IM_COL32(18,19,21,245));
        }

        // ════════════════════════════════════════════════════════
        //  SCAN BARS (horizontal sweep)
        // ════════════════════════════════════════════════════════
        draw->PushClipRect(P, ImVec2(P.x + PS.x, P.y + PS.y), true);
        for (auto& sb : scanBars) {
            sb.y += dt * sb.speed;
            if (sb.y < 0)      sb.y = PS.y;
            if (sb.y > PS.y)   sb.y = 0;
            draw->AddRectFilledMultiColor(
                ImVec2(P.x, P.y + sb.y - sb.width),
                ImVec2(P.x + PS.x, P.y + sb.y),
                IM_COL32(0,0,0,0), IM_COL32(0,0,0,0),
                A2(sb.alpha * 1.5f), A2(sb.alpha * 1.5f));
            draw->AddRectFilledMultiColor(
                ImVec2(P.x, P.y + sb.y),
                ImVec2(P.x + PS.x, P.y + sb.y + 2.0f),
                A(sb.alpha * 2.0f), A(sb.alpha * 2.0f),
                IM_COL32(0,0,0,0), IM_COL32(0,0,0,0));
        }
        draw->PopClipRect();

        // ════════════════════════════════════════════════════════
        //  GLITCH BLOCKS (random screen tears)
        // ════════════════════════════════════════════════════════
        glitchSpawnTimer += dt;
        if (glitchSpawnTimer > RndRange(0.08f, 0.35f)) {
            glitchSpawnTimer = 0.0f;
            if (glitchBlocks.size() < 6 && RndInt(100) < 55) {
                GlitchBlock gb;
                gb.x       = RndRange(0.0f, PS.x * 0.85f);
                gb.y       = RndRange(0.0f, PS.y);
                gb.w       = RndRange(40.0f, 220.0f);
                gb.h       = RndRange(2.0f, 8.0f);
                gb.life    = 0.0f;
                gb.maxLife = RndRange(0.05f, 0.18f);
                gb.offsetX = RndRange(-12.0f, 12.0f);
                const float ga = RndRange(0.15f, 0.45f);
                gb.col     = RndInt(2) ? A(ga) : A2(ga);
                glitchBlocks.push_back(gb);
            }
        }
        {
            auto it = glitchBlocks.begin();
            while (it != glitchBlocks.end()) {
                it->life += dt;
                if (it->life >= it->maxLife) { it = glitchBlocks.erase(it); continue; }
                const float fade = 1.0f - it->life / it->maxLife;
                DrawSlashRect(draw,
                    P.x + it->x + it->offsetX, P.y + it->y,
                    it->w, it->h, 6.0f,
                    ImGui::GetColorU32(ImVec4(av.x, av.y, av.z, 0.35f * fade)));
                ++it;
            }
        }

        // ════════════════════════════════════════════════════════
        //  RIGHT CONTENT PANE
        // ════════════════════════════════════════════════════════
        const float cL = P.x + PS.x * 0.42f;
        const float cR = P.x + PS.x - 18.0f;
        const float cW = cR - cL;

        // Vertical divider (dashed)
        DrawDashedLine(draw,
            ImVec2(cL - 10.0f, P.y + 16.0f),
            ImVec2(cL - 10.0f, P.y + PS.y - 16.0f),
            A2(0.25f), 1.0f, 6.0f, 4.0f);

        ImFont* labelFont = font::inter_semibold ? font::inter_semibold : ImGui::GetFont();
        ImFont* titleFont = F50 ? F50 : labelFont;
        const float lSz  = labelFont->FontSize * 0.80f;
        const float bSz  = labelFont->FontSize * 0.88f;
        const float tSz  = (titleFont == F50) ? 13.0f : titleFont->FontSize;

        // ── System ID tag (top-left of right pane) ─────────────
        {
            const char* sysTag = "SYS::OMNI_V1";
            // Blinking cursor effect
            const bool blink = ((int)(animationTime * 2.4f) % 2 == 0);
            char tagBuf[32];
            std::snprintf(tagBuf, sizeof(tagBuf), "%s%s", sysTag, blink ? "_" : " ");
            draw->AddText(labelFont, lSz,
                ImVec2(cL, P.y + 14.0f), A(0.85f), tagBuf);
        }

        // Thin separator line under tag
        DrawDashedLine(draw,
            ImVec2(cL, P.y + 14.0f + lSz + 5.0f),
            ImVec2(cR, P.y + 14.0f + lSz + 5.0f),
            A2(0.20f), 0.8f, 8.0f, 5.0f);

        // ── TITLE (large, bold, glitchy) ───────────────────────
        const float titleY = P.y + 38.0f;
        // Shadow/glitch offset
        const float gOff = std::sin(animationTime * 18.0f) > 0.92f ? 2.0f : 0.0f;
        if (gOff > 0.0f) {
            draw->AddText(titleFont, tSz,
                ImVec2(cL + gOff + 2.0f, titleY + 1.0f),
                A(0.30f), "OMNI");
        }
        draw->AddText(titleFont, tSz,
            ImVec2(cL, titleY),
            ImGui::GetColorU32(c::text::text_active), "OMNI");

        draw->AddText(titleFont, tSz * 0.72f,
            ImVec2(cL, titleY + tSz + 3.0f),
            A2(0.60f), "HACKS // BOOT SEQUENCE");

        // ── SIGNAL BARS (audio/signal visualizer) ──────────────
        const float sigY  = titleY + tSz + 3.0f + lSz + 14.0f;
        const float sigH  = 18.0f;
        const float sigW  = cW * 0.55f;
        DrawSignalBars(draw,
            ImVec2(cL, sigY + sigH),
            sigW, sigH, animationTime,
            A(0.80f), A2(0.15f));

        // Signal label
        {
            const char* sigLabel = "NEURAL LINK ACTIVE";
            draw->AddText(labelFont, lSz * 0.85f,
                ImVec2(cL + sigW + 8.0f, sigY + sigH * 0.3f),
                A2(0.70f), sigLabel);
        }

        // ── STEP ROWS (decrypt animation) ──────────────────────
        const float stepTop = sigY + sigH + 18.0f;
        const float sH      = 38.0f;
        const float sGap    = 6.0f;

        for (int i = 0; i < 3; ++i) {
            const float nextThr = (i < 2) ? stepThresholds[i + 1] : 1.01f;
            const int   state   = (progress >= nextThr) ? 2
                                : (progress >= stepThresholds[i]) ? 1 : 0;

            // Update decrypt animation
            DecryptState& ds = decryptStates[i];
            const int labelLen = (int)std::strlen(stepLabels[i]);
            if (state >= 1 && !ds.done) {
                ds.timer += dt;
                if (ds.timer > 0.055f) {
                    ds.timer = 0.0f;
                    if (ds.revealedChars < labelLen)
                        ++ds.revealedChars;
                    else
                        ds.done = true;
                }
                // Build display string
                for (int c = 0; c < labelLen; ++c) {
                    ds.display[c] = (c < ds.revealedChars)
                        ? stepLabels[i][c]
                        : kGlitchChars[RndInt((int)sizeof(kGlitchChars) - 1)];
                }
                ds.display[labelLen] = '\0';
            } else if (state == 0) {
                // Scrambled placeholder
                for (int c = 0; c < labelLen; ++c)
                    ds.display[c] = kGlitchChars[RndInt((int)sizeof(kGlitchChars) - 1)];
                ds.display[labelLen] = '\0';
            }

            const float ry = stepTop + i * (sH + sGap);

            // Row BG — slash-style left edge
            DrawSlashRect(draw, cL, ry, cW, sH, 6.0f,
                state == 1 ? A2(0.12f) :
                state == 2 ? A2(0.08f) : IM_COL32(255,255,255,8));

            // Left accent slash bar
            DrawSlashRect(draw, cL, ry, 3.0f, sH, 3.0f,
                state == 2 ? A(0.95f) :
                state == 1 ? A2(0.80f) : A2(0.20f));

            // State indicator (right side)
            const char* stateStr = state == 2 ? "[DONE]"
                                 : state == 1 ? "[EXEC]" : "[IDLE]";
            const ImVec2 stSz = labelFont->CalcTextSizeA(lSz, FLT_MAX, 0.0f, stateStr);
            draw->AddText(labelFont, lSz,
                ImVec2(cR - stSz.x, ry + (sH - lSz) * 0.5f),
                state == 2 ? A(0.90f) :
                state == 1 ? A2(0.85f) : A2(0.35f),
                stateStr);

            // Row number
            char numBuf[8];
            std::snprintf(numBuf, sizeof(numBuf), "%02d/", i + 1);
            draw->AddText(labelFont, lSz,
                ImVec2(cL + 10.0f, ry + (sH - lSz) * 0.5f),
                A2(0.40f), numBuf);

            // Decrypted / scrambled label
            const ImU32 labelCol =
                state == 2 ? ImGui::GetColorU32(c::text::text_active) :
                state == 1 ? A(0.90f) : A2(0.40f);
            draw->AddText(labelFont, bSz,
                ImVec2(cL + 36.0f, ry + (sH - bSz) * 0.5f),
                labelCol,
                (state == 0 || (state == 1 && !ds.done)) ? ds.display
                                                          : stepLabels[i]);
        }

        // ── PROGRESS SECTION ───────────────────────────────────
        const float pbTop = stepTop + 3 * (sH + sGap) + 10.0f;

        // Percent text (large, anchored right)
        char pctBuf[16];
        std::snprintf(pctBuf, sizeof(pctBuf), "%03d%%", (int)std::lround(progress * 100.0f));
        const ImVec2 pctSz = titleFont->CalcTextSizeA(tSz * 1.4f, FLT_MAX, 0.0f, pctBuf);
        // Glitch offset on percent
        const float pGlitch = std::sin(animationTime * 22.0f) > 0.90f ? 3.0f : 0.0f;
        if (pGlitch > 0.0f)
            draw->AddText(titleFont, tSz * 1.4f,
                ImVec2(cR - pctSz.x + pGlitch, pbTop), A(0.35f), pctBuf);
        draw->AddText(titleFont, tSz * 1.4f,
            ImVec2(cR - pctSz.x, pbTop),
            ImGui::GetColorU32(c::text::text_active), pctBuf);

        // "LOADING" label left of percent
        draw->AddText(labelFont, lSz,
            ImVec2(cL, pbTop + tSz * 0.2f), A2(0.50f), "DATA TRANSFER");

        // Progress bar — segmented style (like blocks not smooth)
        const float barY  = pbTop + tSz * 1.4f + 8.0f;
        const float barH  = 6.0f;
        const float barW  = cW;
        const int   segs  = 40;
        const float segW  = barW / segs - 1.5f;
        const int   filled= (int)(EaseInOutQuart(progress) * segs);
        for (int s = 0; s < segs; ++s) {
            const float sx = cL + s * (segW + 1.5f);
            const bool  isF= s < filled;
            // Active segment — slight flicker on leading edge
            const bool  isLead = (s == filled - 1);
            const float fAlpha = isLead
                ? (0.7f + 0.3f * std::sin(animationTime * 14.0f))
                : (isF ? 0.90f : 0.08f);
            draw->AddRectFilled(
                ImVec2(sx, barY),
                ImVec2(sx + segW, barY + barH),
                isF ? A(fAlpha) : A2(0.12f), 1.0f);
        }

        // Bar glow underneath
        for (int g = 4; g >= 0; --g) {
            const float gy = 2.0f + g * 2.5f;
            const float gfilled = cL + EaseInOutQuart(progress) * barW;
            draw->AddRectFilled(
                ImVec2(cL, barY - gy),
                ImVec2(gfilled, barY + barH + gy),
                A(0.05f / (g + 1)), 2.0f);
        }

        // Status message below bar
        {
            const char* msgs[] = {
                "VERIFYING CRYPTOGRAPHIC SIGNATURE...",
                "INJECTING MODULE HOOKS...",
                "BOOTSTRAPPING RENDER INTERFACE..."
            };
            const char* msg = progress < 0.34f ? msgs[0]
                            : progress < 0.68f ? msgs[1] : msgs[2];
            draw->AddText(labelFont, lSz * 0.85f,
                ImVec2(cL, barY + barH + 7.0f),
                A2(0.45f), msg);
        }

        // ── BOTTOM STATUS LINE ─────────────────────────────────
        {
            // Left pane bottom — coordinate display
            const float bLineY = P.y + PS.y - 18.0f;
            char coordBuf[48];
            std::snprintf(coordBuf, sizeof(coordBuf),
                "T+%.2fs  MEM:%.1fMB  CPU:%.0f%%",
                animationTime,
                48.0f + std::sin(animationTime * 0.7f) * 3.0f,
                22.0f + std::abs(std::sin(animationTime * 2.1f)) * 18.0f);
            draw->AddText(labelFont, lSz * 0.80f,
                ImVec2(P.x + 14.0f, bLineY),
                A2(0.40f), coordBuf);

            // Right — version tag
            draw->AddText(labelFont, lSz * 0.80f,
                ImVec2(cR - 80.0f, bLineY),
                A2(0.35f), "BUILD 2025.06.06");
        }

        // ════════════════════════════════════════════════════════
        //  LEFT PANE — DECORATIVE GEOMETRY (not circles!)
        // ════════════════════════════════════════════════════════
        {
            const float lpCX = P.x + PS.x * 0.21f;
            const float lpCY = P.y + PS.y * 0.48f;
            const float lpSz = PS.y * 0.28f;

            // Rotating diamond (square rotated 45°)
            for (int d = 0; d < 3; ++d) {
                const float dSz  = lpSz * (0.55f + d * 0.22f);
                const float rot  = animationTime * (d % 2 == 0 ? 0.6f : -0.4f)
                                  + d * 3.14159f / 4.0f;
                const float dalpha = 0.50f - d * 0.12f;
                ImVec2 pts[4];
                const float angles[4] = {
                    rot + 0.0f,
                    rot + 3.14159f * 0.5f,
                    rot + 3.14159f,
                    rot + 3.14159f * 1.5f
                };
                for (int k = 0; k < 4; ++k)
                    pts[k] = ImVec2(lpCX + std::cos(angles[k]) * dSz,
                                    lpCY + std::sin(angles[k]) * dSz);
                draw->AddQuad(pts[0], pts[1], pts[2], pts[3],
                    d == 0 ? A(dalpha) : A2(dalpha * 0.85f),
                    d == 0 ? 2.0f : 1.2f);
            }

            // Cross-hair lines through diamond center
            const float chLen = lpSz * 1.15f;
            const float chOff = 12.0f;
            draw->AddLine(ImVec2(lpCX - chLen, lpCY),
                          ImVec2(lpCX - chOff, lpCY), A2(0.30f), 1.0f);
            draw->AddLine(ImVec2(lpCX + chOff, lpCY),
                          ImVec2(lpCX + chLen, lpCY), A2(0.30f), 1.0f);
            draw->AddLine(ImVec2(lpCX, lpCY - chLen),
                          ImVec2(lpCX, lpCY - chOff), A2(0.30f), 1.0f);
            draw->AddLine(ImVec2(lpCX, lpCY + chOff),
                          ImVec2(lpCX, lpCY + chLen), A2(0.30f), 1.0f);

            // Targeting tick marks on crosshair
            for (int t = 0; t < 4; ++t) {
                const float ta = t * 3.14159f * 0.5f + animationTime * 0.3f;
                const float tr = lpSz * 0.78f;
                const ImVec2 tc(lpCX + std::cos(ta) * tr, lpCY + std::sin(ta) * tr);
                const ImVec2 td(lpCX + std::cos(ta) * (tr + 8.0f),
                                lpCY + std::sin(ta) * (tr + 8.0f));
                draw->AddLine(tc, td, A(0.75f), 2.0f);
            }

            // Animated corner squares on diamond vertices
            {
                const float rot0 = animationTime * 0.6f;
                ImVec2 v0(lpCX + std::cos(rot0) * lpSz * 0.55f,
                          lpCY + std::sin(rot0) * lpSz * 0.55f);
                const float sq = 4.0f + std::sin(animationTime * 3.5f) * 1.5f;
                draw->AddRectFilled(
                    ImVec2(v0.x - sq, v0.y - sq),
                    ImVec2(v0.x + sq, v0.y + sq),
                    A(0.90f));
            }

            // Center reticle
            draw->AddRectFilled(
                ImVec2(lpCX - 3.0f, lpCY - 3.0f),
                ImVec2(lpCX + 3.0f, lpCY + 3.0f),
                A(0.95f));
            draw->AddRect(
                ImVec2(lpCX - 7.0f, lpCY - 7.0f),
                ImVec2(lpCX + 7.0f, lpCY + 7.0f),
                A2(0.50f), 0.0f, 0, 1.0f);

            // "LOCK" text when progress > 0.9
            if (progress > 0.90f) {
                const float lockAlpha = (progress - 0.90f) / 0.10f;
                draw->AddText(labelFont, lSz,
                    ImVec2(lpCX - 14.0f, lpCY + lpSz + 6.0f),
                    A(lockAlpha * 0.90f), "LOCKED ON");
            }

            // Scan ring (expanding then fading — like a sonar ping)
            {
                const float pingT = std::fmod(animationTime * 0.55f, 1.0f);
                const float pingR = lpSz * 0.30f + pingT * lpSz * 1.20f;
                const float pingA = (1.0f - pingT) * 0.35f;
                draw->AddCircle(ImVec2(lpCX, lpCY), pingR, A2(pingA), 48, 1.2f);
            }
        }
    }
    ImGui::End();

    loadingAnimationTimer += io.DeltaTime;
    if (loadingAnimationTimer >= loadingAnimationDuration) {
        showLoadingAnimation  = false;
        loadingAnimationTimer = 0.0f;
        finished              = true;
    }
    return finished;
}

} // namespace ui_loading
