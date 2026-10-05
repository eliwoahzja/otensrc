#pragma once
#include "Call_ImGui.h"
#include "imgui_settings.h"
#include "ethnir_menu.h"
#include "../System/Core/SaveConfig.h"
#include "../System/UI/TextureLoader.h"
#include "../System/UI/Logo.h"
#include "../System/UI/SkinTab.h"
#include "../Fonts/Icon.h"
#include "../IMAGE/bg.h"
#include "../IMAGE/glass_bg.h"

extern ImFont* F50;
extern ImFont* F107;
extern float menu[4];
extern float SnowBsize;
extern float speedHackMultiplier;
extern float jumpHeightMultiplier;
extern float SlideRange;

namespace font {
extern ImFont* inter_semibold;
}

namespace runtime_preview_menu {

struct Texture {
    GLuint id = 0;
};

struct StateRefs {
    bool &dark;
    float &tabAlpha;
    float &tabAdd;
    int &page;
    int &activeTab;
    bool &windowCollapsed;
    bool &isMenuVisible;
    double &collapseBarLastActiveTime;
    float &collapseBarOpacityAnim;
    float &collapseBarPressAnim;
};

static Texture g_menuBackground;
static Texture g_ethnirGlassBackdrop;
static bool g_infoPopupRequested = false;
static bool g_priceListPopupRequested = false;
static bool g_popupFocusVisible = false;
static ImVec2 g_popupFocusMin(0.0f, 0.0f);
static ImVec2 g_popupFocusMax(0.0f, 0.0f);
static int g_activeChangelogTab = 0;
static int g_espBoxType = 1;
static int g_espLinePosition = 1;
static int g_espHealthPosition = 1;
static int g_espCrosshairStyle = 0;
static int g_espStyle = 0;

inline void EnsureTexture(Texture &texture, const unsigned char *bytes, int size) {
    if (texture.id != 0) {
        return;
    }
    texture.id = LoadTextureFromMemory(bytes, size);
}

inline void EnsureTexturesLoaded() {
    EnsureTexture(g_menuBackground, bg_data, (int)sizeof(bg_data));
    EnsureTexture(g_ethnirGlassBackdrop, glass_bg_data, (int)sizeof(glass_bg_data));
}

inline std::string ToUpperAscii(std::string text) {
    for (char &ch : text) {
        ch = (char)std::toupper((unsigned char)ch);
    }
    return text;
}

inline std::string BuildExpiryLabel() {
    const std::string countdown = getExpiryCountdown();
    if (!countdown.empty()) {
        return ToUpperAscii("EXPIRY " + countdown);
    }
    if (EXP == "No expiry") {
        return "NO EXPIRY";
    }
    if (!EXP.empty() && EXP != " ") {
        return ToUpperAscii(EXP);
    }
    return "NO EXPIRY";
}

inline std::string BuildFpsLabel() {
    char buffer[32] = {};
    std::snprintf(buffer, sizeof(buffer), "FPS %d", (int)std::lround(ImGui::GetIO().Framerate));
    return buffer;
}

inline void ResetPopupFocusWindow() {
    g_popupFocusVisible = false;
    g_popupFocusMin = ImVec2(0.0f, 0.0f);
    g_popupFocusMax = ImVec2(0.0f, 0.0f);
}

inline void RegisterPopupFocusWindow() {
    const ImVec2 min = ImGui::GetWindowPos();
    const ImVec2 windowSize = ImGui::GetWindowSize();
    const ImVec2 max(min.x + windowSize.x, min.y + windowSize.y);
    if (!g_popupFocusVisible) {
        g_popupFocusVisible = true;
        g_popupFocusMin = min;
        g_popupFocusMax = max;
        return;
    }
    g_popupFocusMin.x = ImMin(g_popupFocusMin.x, min.x);
    g_popupFocusMin.y = ImMin(g_popupFocusMin.y, min.y);
    g_popupFocusMax.x = ImMax(g_popupFocusMax.x, min.x);
    g_popupFocusMax.y = ImMax(g_popupFocusMax.y, min.y);
}

inline void DrawPopupBackdropFocusLayer(ImDrawList *drawList) {
    if (drawList == nullptr || !g_popupFocusVisible) {
        return;
    }
    drawList->PushClipRectFullScreen();
    const ImVec2 viewportMax = ImGui::GetIO().DisplaySize;
    const ImVec2 holeMin(
        ImClamp(g_popupFocusMin.x, 0.0f, viewportMax.x),
        ImClamp(g_popupFocusMin.y, 0.0f, viewportMax.y)
    );
    const ImVec2 holeMax(
        ImClamp(g_popupFocusMax.x, 0.0f, viewportMax.x),
        ImClamp(g_popupFocusMax.y, 0.0f, viewportMax.y)
    );
    const ImU32 dimColor = IM_COL32(8, 0, 14, 172);
    if (holeMin.y > 0.0f) drawList->AddRectFilled(ImVec2(0.0f, 0.0f), ImVec2(viewportMax.x, holeMin.y), dimColor);
    if (holeMin.x > 0.0f) drawList->AddRectFilled(ImVec2(0.0f, holeMin.y), ImVec2(holeMin.x, holeMax.y), dimColor);
    if (holeMax.x < viewportMax.x) drawList->AddRectFilled(ImVec2(holeMax.x, holeMin.y), ImVec2(viewportMax.x, holeMax.y), dimColor);
    if (holeMax.y < viewportMax.y) drawList->AddRectFilled(ImVec2(0.0f, holeMax.y), viewportMax, dimColor);
    drawList->AddRect(
        holeMin,
        holeMax,
        ImGui::GetColorU32(ImVec4(c::accent.x, c::accent.y, c::accent.z, 0.18f)),
        10.0f,
        0,
        1.0f
    );
    drawList->PopClipRect();
}

#include "runtime_preview_helpers.h"
#include "../System/UI/MiscTab.h"
#include "../System/UI/SettingsTab.h"

inline bool DrawRuntimeEspColorRow(const char *label, float *color) {
    if (label == nullptr || color == nullptr) return false;
    using namespace ethnir;

    if (!EqPassFilter(label)) return false;

    auto normalize = [](float v) {
        return (v <= 1.0f) ? ImClamp(v, 0.0f, 1.0f) : ImClamp(v / 255.0f, 0.0f, 1.0f);
    };
    const ImVec4 shown(normalize(color[0]), normalize(color[1]), normalize(color[2]), 1.0f);

    const Palette pal = EqPal();
    ImDrawList *dl = ImGui::GetWindowDrawList();
    const ImRect row = EqNextRow();

    char clean[128];
    EqStripId(label, clean, IM_ARRAYSIZE(clean));

    ImGuiID gid = ImGui::GetCurrentWindow()->GetID(label);
    ImGui::ItemAdd(row, gid);
    bool hovered = false, held = false;
    const bool pressed = ImGui::ButtonBehavior(row, gid, &hovered, &held, ImGuiButtonFlags_None);
    ImGui::KeepAliveID(gid);
    if (held) { if (EqState()) EqState()->InputActive = true; }

    EqCardRowIndex()++;
    EqRowHover(dl, row, (hovered || held) ? 0.65f : 0.0f);

    const float labelSize = portfolio::s(portfolio::row_label_font);
    const ImVec2 ts = EqTextFont()->CalcTextSizeA(labelSize, FLT_MAX, 0.0f, clean);
    EqDrawLabel(dl, ImVec2(row.Min.x + portfolio::s(portfolio::box_pad_x),
                           row.Min.y + (row.GetHeight() - ts.y) * 0.5f),
                clean, EqCol(hovered ? pal.text : pal.textDim), labelSize);

    const float sw = portfolio::s(portfolio::color_swatch_size);
    const ImVec2 swMin(row.Max.x - portfolio::s(portfolio::box_pad_x) - sw,
                       row.GetCenter().y - sw * 0.5f);
    dl->AddRectFilled(swMin, swMin + ImVec2(sw, sw),
                      ImGui::ColorConvertFloat4ToU32(shown), portfolio::s(4.0f));
    dl->AddRect(swMin, swMin + ImVec2(sw, sw),
                EqColA(pal.cardEdge, 1.2f), portfolio::s(4.0f), 0, 1.0f);

    char hex[16];
    ImFormatString(hex, IM_ARRAYSIZE(hex), "#%02X%02X%02X",
                   (int)ImClamp(color[0], 0.0f, 255.0f),
                   (int)ImClamp(color[1], 0.0f, 255.0f),
                   (int)ImClamp(color[2], 0.0f, 255.0f));
    const ImVec2 hs = EqLabelSize(hex, 12.0f);
    EqDrawLabel(dl, ImVec2(swMin.x - portfolio::s(8.0f) - hs.x,
                           row.GetCenter().y - hs.y * 0.5f),
                hex, EqCol(pal.textFaint), 12.0f);

    if (pressed) ImGui::OpenPopup("##ethnir_color_pick");
    ImGui::SetNextWindowPos(ImVec2(row.Min.x, row.Max.y + 4.0f), ImGuiCond_Appearing);
    ImGui::SetNextWindowSize(ImVec2(portfolio::s(330.0f), 0.0f), ImGuiCond_Appearing);
    if (ImGui::BeginPopup("##ethnir_color_pick",
            ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings))
    {
        float c[4] = { shown.x, shown.y, shown.z, 1.0f };
        ImGui::SetNextItemWidth(portfolio::s(310.0f));
        if (custom::ColorEdit4("##ethnir_color_edit", c,
                ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoAlpha |
                ImGuiColorEditFlags_PickerHueWheel))
        {
            color[0] = c[0] * 255.0f;
            color[1] = c[1] * 255.0f;
            color[2] = c[2] * 255.0f;
            color[3] = 255.0f;
            EqMarkDirty();
        }
        ImGui::EndPopup();
    }
    return false;
}

inline void CopyLinkedEspColors(float *lineColor, float *boxColor, float *nameColor, float *healthColor, float *distanceColor, float *skeletonColor) {
    if (lineColor == nullptr) {
        return;
    }
    if (boxColor) std::memcpy(boxColor, lineColor, sizeof(float) * 4);
    if (nameColor) std::memcpy(nameColor, lineColor, sizeof(float) * 4);
    if (healthColor) std::memcpy(healthColor, lineColor, sizeof(float) * 4);
    if (distanceColor) std::memcpy(distanceColor, lineColor, sizeof(float) * 4);
    if (skeletonColor) std::memcpy(skeletonColor, lineColor, sizeof(float) * 4);
}

inline void RenderEspTab(float childWidth, float childHeight) {
    (void)childWidth;
    (void)childHeight;
    using namespace ethnir;
    EqBeginColumns();
    SectionLabel("PLAYERS");
    BeginGroupCard("eth_esp");
    RowToggle(ICON_FA_EYE, "ESP Line", &Config.ESPMenu.isPlayerLine);
    RowToggle(ICON_FA_EYE, "ESP Box", &Config.ESPMenu.Box);
    RowToggle(ICON_FA_EYE, "ESP Skeleton", &Config.ESPMenu.Skeleton);
    RowToggle(ICON_FA_EYE, "ESP Health", &Config.ESPMenu.Health);
    RowToggle(ICON_FA_EYE, "ESP Name", &Config.ESPMenu.Name);
    RowToggle(ICON_FA_EYE, "ESP Distance", &Config.ESPMenu.Distance);
    RowToggle(ICON_FA_EYE, "ESP Count", &Config.ESPMenu.Count);
    RowToggle(ICON_FA_EYE, "360 Alert", &Config.ESPMenu.Alert);
    RowToggle(ICON_FA_EYE, "Show AimLine", &Config.ESPMenu.Aimline);
    RowToggle(ICON_FA_EYE, "Yellow Wallhack", &Config.ExtraMenu.WallHack);
    RowToggle(ICON_FA_EYE, "Red Wallhack", &Config.ExtraMenu.RedWallhack);
    EndGroupCard();
    EqNextColumn();
    SectionLabel("ESP OPTIONS");
    BeginGroupCard("eth_esp_options");
    static const char *boxTypes[] = {"Fill", "Outline", "Corner", "3D"};
    static const char *linePositions[] = {"Top", "Mid", "Bottom"};
    static const char *healthPositions[] = {"Top", "Side"};
    static const char *espStyles[] = {"None", "3D Sphere", "Player Signal"};
    g_espBoxType = ImClamp((int)Config.ESPMenu.BoxType, 0, IM_ARRAYSIZE(boxTypes) - 1);
    g_espLinePosition = ImClamp((int)Config.ESPMenu.Target, 0, IM_ARRAYSIZE(linePositions) - 1);
    g_espHealthPosition = ImClamp((int)Config.ESPMenu.HealthPosition, 0, IM_ARRAYSIZE(healthPositions) - 1);
    g_espStyle = ImClamp((int)Config.ESPMenu.EspStyle, 0, IM_ARRAYSIZE(espStyles) - 1);
    ComboRow(ICON_FA_SLIDERS_H, "Box Type", &g_espBoxType, boxTypes, IM_ARRAYSIZE(boxTypes));
    ComboRow(ICON_FA_SLIDERS_H, "Line Position", &g_espLinePosition, linePositions, IM_ARRAYSIZE(linePositions));
    ComboRow(ICON_FA_SLIDERS_H, "Health Position", &g_espHealthPosition, healthPositions, IM_ARRAYSIZE(healthPositions));
    ComboRow(ICON_FA_SLIDERS_H, "ESP Style", &g_espStyle, espStyles, IM_ARRAYSIZE(espStyles));
    DrawRuntimeEspColorRow("Player ESP Color", Config.sColorsESPPLAYER.LinePLAYER);
    CopyLinkedEspColors(
        Config.sColorsESPPLAYER.LinePLAYER,
        Config.sColorsESPPLAYER.BoxPLAYER,
        Config.sColorsESPPLAYER.NamePLAYER,
        Config.sColorsESPPLAYER.HealthPLAYER,
        Config.sColorsESPPLAYER.DistancePLAYER,
        Config.sColorsESPPLAYER.SkeletonPLAYER
    );
    DrawRuntimeEspColorRow("POV ESP Color", Config.sColorsESPOTHERS.PovOTHERS);
    DrawRuntimeEspColorRow("Bot ESP Color", Config.sColorsESPBOT.LineBOT);
    CopyLinkedEspColors(
        Config.sColorsESPBOT.LineBOT,
        Config.sColorsESPBOT.BoxBOT,
        Config.sColorsESPBOT.NameBOT,
        Config.sColorsESPBOT.HealthBOT,
        Config.sColorsESPBOT.DistanceBOT,
        Config.sColorsESPBOT.SkeletonBOT
    );
    Config.ESPMenu.BoxType = (EspBoxType)g_espBoxType;
    Config.ESPMenu.Target = (LineTarget)g_espLinePosition;
    Config.ESPMenu.HealthPosition = (EspHealthPosition)g_espHealthPosition;
    Config.ESPMenu.EspStyle = (EspStyleTarget)g_espStyle;
    EndGroupCard();
    EqEndColumns();
}

inline void RenderAimTab(float childWidth, float childHeight) {
    (void)childWidth;
    (void)childHeight;
    using namespace ethnir;
    EqBeginColumns();
    SectionLabel("AIMBOT");
    BeginGroupCard("eth_aim");
    RowToggle(ICON_FA_CROSSHAIRS, "Aimbot 360", &Config.Aim.Aimbot360);
    RowToggle(ICON_FA_CROSSHAIRS, "Bullet Track", &Config.Aim.AimSilent);
    RowSlider(ICON_FA_CROSSHAIRS, "Aim Assist Size", &Config.Aim.AimAssistSize, 0.0f, 100.0f, "%.0f");
    EndGroupCard();
    EqNextColumn();
    SectionLabel("COMBAT");
    BeginGroupCard("eth_combat");
    static const char *targets[] = {"Head", "Chest", "Body"};
    static const char *triggers[] = {"None", "Shooting", "Scoping"};
    static const char *targetBy[] = {"Distance", "FOV"};
    int target = ImClamp((int)Config.Aim.Target, 0, IM_ARRAYSIZE(targets) - 1);
    int trigger = ImClamp((int)Config.Aim.Trigger, 0, IM_ARRAYSIZE(triggers) - 1);
    int aimBy = ImClamp((int)Config.Aim.By, 0, IM_ARRAYSIZE(targetBy) - 1);
    ComboRow(ICON_FA_CROSSHAIRS, "Location", &target, targets, IM_ARRAYSIZE(targets));
    ComboRow(ICON_FA_CROSSHAIRS, "Trigger", &trigger, triggers, IM_ARRAYSIZE(triggers));
    ComboRow(ICON_FA_CROSSHAIRS, "Target By", &aimBy, targetBy, IM_ARRAYSIZE(targetBy));
    Config.Aim.Target = (EAimTarget)target;
    Config.Aim.Trigger = (EAimTrigger)trigger;
    Config.Aim.By = (EAim)aimBy;
    RowSlider(ICON_FA_CROSSHAIRS, "FOV Size", &Config.Aim.Cross, 0.0f, 100.0f, "%.0f");
    EndGroupCard();
    EqEndColumns();
}

inline void RenderMemoryTab(float childWidth, float childHeight) {
    (void)childWidth;
    (void)childHeight;
    using namespace ethnir;
    EqBeginColumns();
    SectionLabel("COMBAT HACKS");
    BeginGroupCard("eth_combat_hacks");
    RowToggle(ICON_FA_BOLT, "Hitbox", &Config.ExtraMenu.Hit);
    RowToggle(ICON_FA_BOLT, "No Recoil", &Config.ExtraMenu.Recoil);
    RowToggle(ICON_FA_BOLT, "No Spread", &Config.ExtraMenu.Spread);
    RowToggle(ICON_FA_BOLT, "No Shake", &Config.ExtraMenu.Shake);
    RowToggle(ICON_FA_BOLT, "No Overheat", &Config.ExtraMenu.Rpd);
    RowToggle(ICON_FA_BOLT, "Firerate", &Config.ExtraMenu.Fire);
    RowToggle(ICON_FA_BOLT, "Weapon Kinetic", &Config.ExtraMenu.Kinetic);
    EndGroupCard();
    EqNextColumn();
    SectionLabel("UTILITY HACKS");
    BeginGroupCard("eth_utility_hacks");
    RowToggle(ICON_FA_BOLT, "Unlock Blueprint", &Config.ExtraMenu.UnlockBlueprint);
    RowToggle(ICON_FA_BOLT, "No Parachute", &Config.ExtraMenu.Parachute);
    RowToggle(ICON_FA_BOLT, "Anti Flashbang", &Config.ExtraMenu.Flash);
    RowToggle(ICON_FA_BOLT, "Fast Dive", &Config.ExtraMenu.Diving);
    RowToggle(ICON_FA_BOLT, "Fast Reload", &Config.ExtraMenu.Reload);
    RowToggle(ICON_FA_BOLT, "Fast Scope", &Config.ExtraMenu.Scope);
    RowToggle(ICON_FA_BOLT, "Quick Switch", &Config.ExtraMenu.Switch);
    EndGroupCard();

    SectionLabel("MOVEMENT");
    BeginGroupCard("eth_movement");
    RowSlider(ICON_FA_SLIDERS_H, "Snowboard Speed", &SnowBsize, 0.0f, 100.0f, "%.1f");
    RowSlider(ICON_FA_SLIDERS_H, "Slide Distance", &SlideRange, 0.0f, 30.0f, "%.1f");
    RowSlider(ICON_FA_SLIDERS_H, "SpeedHack", &speedHackMultiplier, 0.5f, 2.0f, "%.1fx");
    RowSlider(ICON_FA_SLIDERS_H, "High Jump", &jumpHeightMultiplier, 0.5f, 5.0f, "%.2fx");
    EndGroupCard();
    EqEndColumns();
}

inline void RenderMiscTab(float contentWidth, float contentHeight) {
    const misc_tab::LayoutMetrics layout = misc_tab::CalculateLayout(contentWidth, g_activeChangelogTab);
    const float columnGap = 16.0f;
    const float columnWidth = ImMax(0.0f, ImFloor((contentWidth - columnGap) * 0.5f));
    const float rightTopHeight = ImMax(0.0f, ImFloor((contentHeight - layout.rowGap) * 0.5f));
    const float rightBottomHeight = ImMax(0.0f, contentHeight - layout.rowGap - rightTopHeight);
    const float changelogHeight = ImMax(layout.changelogHeight, contentHeight - 20.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
                        ImVec2(ImGui::GetStyle().ItemSpacing.x, layout.rowGap));

    ethnir::EqBeginColumns();
    ethnir::SectionLabel("CHANGELOG");
    {
        const ChildFrame changelog = BeginContentChild(
            "CHANGELOG##MISC_CHANGELOG", ImVec2(columnWidth, changelogHeight),
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        const float changelogTabGap = 5.0f;
        const float changelogTabH = 40.0f;
        const float changelogSafeRight = ImGui::GetStyle().ScrollbarSize + 12.0f;
        const float changelogAvailW = ImMax(1.0f, ImGui::GetContentRegionAvail().x - changelogSafeRight);
        const float changelogTabW = ImMax(1.0f, ImFloor((changelogAvailW - changelogTabGap * 2.0f) / 3.0f));
        for (int i = 0; i < IM_ARRAYSIZE(misc_tab::kChangelogTabs); ++i) {
            char buttonId[48] = {};
            std::snprintf(buttonId, sizeof(buttonId), "##changelog_%d", i);
            if (i > 0) {
                ImGui::SameLine(0.0f, changelogTabGap);
            }
            if (misc_tab::DrawChangelogTab(
                    buttonId,
                    misc_tab::kChangelogIcons[i],
                    misc_tab::kChangelogTabs[i],
                    g_activeChangelogTab == i,
                    ImVec2(changelogTabW, changelogTabH))) {
                g_activeChangelogTab = i;
            }
        }
        misc_tab::ContentGap(12.0f);
        const misc_tab::ChangelogSelection activeSection =
            misc_tab::GetActiveChangelogSelection(g_activeChangelogTab);
        ImFont *iconFont = F107 ? F107 : ImGui::GetFont();
        ImGui::PushFont(iconFont);
        ImGui::TextColored(activeSection.color, "%s", activeSection.icon);
        ImGui::PopFont();
        ImGui::SameLine(0.0f, 8.0f);
        ImGui::TextColored(activeSection.color, "%s", activeSection.title);
        ImGui::SameLine(0.0f, 10.0f);
        ImGui::TextColored(c::text::text, "March 10, 2026");
        misc_tab::ContentGap(8.0f);
        misc_tab::DrawSectionItems(activeSection.items, activeSection.count);
        EndContentChild(changelog);
    }
    misc_tab::ContentGap(layout.rowGap);
    ethnir::EqNextColumn();
    ethnir::SectionLabel("INFO");
    {
        const ChildFrame info = BeginContentChild(
            "INFO##MISC_INFO", ImVec2(columnWidth, rightTopHeight),
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        misc_tab::DrawInfoSummary();
        EndContentChild(info);
    }
    misc_tab::ContentGap(layout.rowGap);
    {
        const ChildFrame price = BeginContentChild(
            "PRICELIST##MISC_PRICELIST", ImVec2(columnWidth, rightBottomHeight),
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        misc_tab::DrawPriceSummary("##runtime_misc_inline_prices");
        EndContentChild(price);
    }
    ethnir::EqEndColumns();
    ImGui::PopStyleVar();
}

inline void RenderSettingsTab(float contentWidth, float contentHeight) {
    const float rowGap = 6.0f;
    const float columnGap = 16.0f;
    const float childWidth = ImMax(0.0f, ImFloor((contentWidth - columnGap) * 0.5f));
    const float topChildHeight = ImMax(0.0f, ImFloor((contentHeight - rowGap) * 0.5f));
    const float bottomChildHeight = ImMax(0.0f, contentHeight - rowGap - topChildHeight);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,
                        ImVec2(ImGui::GetStyle().ItemSpacing.x, rowGap));

    ethnir::EqBeginColumns();
    ethnir::SectionLabel("ABOUT");
    {
        const ChildFrame licenseChild = BeginContentChild(
            "LICENSE INFO##RUNTIME_LICENSE_INFO", ImVec2(childWidth, topChildHeight));
        settings_tab::RenderLicenseCard();
        EndContentChild(licenseChild);
    }
    {
        const ChildFrame logoChild = BeginContentChild(
            "LOGO SETTINGS##RUNTIME_LOGO_SETTINGS", ImVec2(childWidth, bottomChildHeight));
        settings_tab::RenderLogoCard();
        EndContentChild(logoChild);
    }
    ethnir::EqNextColumn();
    ethnir::SectionLabel("CONFIGURATION");
    {
        const ChildFrame configChild = BeginContentChild(
            "CONFIG MANAGEMENT##RUNTIME_CONFIG_MANAGEMENT", ImVec2(childWidth, topChildHeight));
        settings_tab::RenderConfigCard();
        EndContentChild(configChild);
    }
    {
        const ChildFrame enhancementChild = BeginContentChild(
            "ENHANCEMENT##RUNTIME_ENHANCEMENT", ImVec2(childWidth, bottomChildHeight));
        settings_tab::RenderEnhancementCard();
        EndContentChild(enhancementChild);
    }
    ethnir::EqEndColumns();

    ImGui::PopStyleVar();
}

inline void CollapseMenu(StateRefs &state) {
    state.windowCollapsed = true;
    state.isMenuVisible = false;
    state.collapseBarLastActiveTime = ImGui::GetTime();
    state.collapseBarOpacityAnim = 1.0f;
    state.collapseBarPressAnim = 0.0f;
}

}
