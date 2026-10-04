#pragma once
#include "../ImGui/imgui.h"
#include "Keyboard.h"
#include <string>
#include <algorithm>
#include <map>
#include <cmath>
#include "../ImGui/imgui_settings.h"
#include "../ImGui/custom_widgets.hpp"

#include "../Skin/Fields.h"
#include "../Skin/Thread.h"

extern float menu[4];
extern char searchQuery[256];
extern bool showKeyboard;
extern ImFont* F50;
extern ImFont* JAAT;
namespace font {
    extern ImFont* inter_semibold;
}

const int MIN_SEARCH_LENGTH = 3;
static int skinSubTab = 0;

static float cursorBlinkTimer = 0.0f;
static bool cursorVisible = true;
static std::string activeInputID = "";

struct checkbox_state
{
    ImVec4 background, circle, text, box;
    float slow_circle;
};

std::string ToLower(std::string str)
{
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

std::string GetBaseWeaponName(const std::string& fullName)
{
    size_t dashPos = fullName.find(" - ");
    if (dashPos == std::string::npos)
        return fullName;
    std::string base = fullName.substr(0, dashPos);
    size_t tagEnd = base.find("] ");
    if (tagEnd != std::string::npos && tagEnd + 2 < base.length())
        base = base.substr(tagEnd + 2);
    return base;
}

namespace ImGui {
    bool AstralInput(const char* label, char* buf, size_t buf_size, const ImVec2& size = ImVec2(0, 0)) {
        ImGuiWindow* window = GetCurrentWindow();
        if (window->SkipItems)
            return false;

        ImGuiContext& g = *GImGui;
        ImGuiIO& io = g.IO;
        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID(label);

        ImVec2 pos = window->DC.CursorPos;
        ImVec2 inputSize = size;
        if (inputSize.x <= 0.0f)
            inputSize.x = GetContentRegionAvail().x;
        if (inputSize.y <= 0.0f)
            inputSize.y = 50.0f;

        ImRect bb(pos, ImVec2(pos.x + inputSize.x, pos.y + inputSize.y));
        ItemSize(bb, style.FramePadding.y);
        if (!ItemAdd(bb, id))
            return false;

        bool hovered, held;
        ButtonBehavior(bb, id, &hovered, &held);
        bool clicked = hovered && IsMouseClicked(0);
        bool isActive = (g.ActiveId == id);

        if (clicked) {
            SetActiveID(id, window);
            SetFocusID(id, window);
            FocusWindow(window);
            activeInputID = label;
        }

        if (isActive) {
            cursorBlinkTimer += io.DeltaTime;
            if (cursorBlinkTimer >= 1.0f) {
                cursorBlinkTimer = 0.0f;
                cursorVisible = !cursorVisible;
            }
        } else {
            cursorBlinkTimer = 0.0f;
            cursorVisible = true;
        }

        ImU32 bgColor = GetColorU32(isActive ? c::elements::background_hovered : hovered ? c::button::background_hovered : c::widget::background);
        ImU32 borderColor = GetColorU32(isActive ? c::accent : c::button::outline);

        ImDrawList* draw_list = window->DrawList;

        draw_list->AddRectFilled(bb.Min, bb.Max, bgColor, 4.0f);
        draw_list->AddRect(bb.Min, bb.Max, borderColor, 4.0f, 0, 1.0f);

        std::string text = buf;
        ImVec2 textPos = ImVec2(bb.Min.x + 15.0f, bb.Min.y + (inputSize.y - GetFontSize()) * 0.5f);

        PushClipRect(ImVec2(bb.Min.x + 10.0f, bb.Min.y), ImVec2(bb.Max.x - 10.0f, bb.Max.y), true);

        if (text.empty() && !isActive) {
            ImVec4 hintColor = c::text::text_hov;
            hintColor.w *= 0.70f;
            draw_list->AddText(textPos, GetColorU32(hintColor), "Type here...");
        } else {
            draw_list->AddText(textPos, GetColorU32(c::text::text_active), text.c_str());

            if (isActive && cursorVisible) {
                ImVec2 textSize = CalcTextSize(text.c_str());
                float cursorX = textPos.x + textSize.x + 2.0f;
                float cursorY1 = textPos.y;
                float cursorY2 = textPos.y + GetFontSize();
                draw_list->AddLine(ImVec2(cursorX, cursorY1), ImVec2(cursorX, cursorY2),
                                   GetColorU32(c::text::text_active), 2.0f);
            }
        }

        PopClipRect();

        if (isActive) {
            float animProgress = fmod(cursorBlinkTimer, 1.0f);
            float lineWidth = bb.GetWidth() * 0.3f;
            float lineStart = bb.Min.x + (bb.GetWidth() - lineWidth) * 0.5f;
            float lineEnd = lineStart + lineWidth;

            float alpha = 1.0f - animProgress;
            if (animProgress > 0.5f) {
                alpha = (1.0f - animProgress) * 2.0f;
            } else {
                alpha = animProgress * 2.0f;
            }

            ImVec4 lineColorF = c::accent;
            lineColorF.w = alpha;
            ImU32 lineColor = GetColorU32(lineColorF);
            draw_list->AddLine(ImVec2(lineStart, bb.Max.y - 2.0f),
                               ImVec2(lineEnd, bb.Max.y - 2.0f),
                               lineColor, 3.0f);
        }

        return false;
    }

    bool CheckboxFullWidth(const char* label, bool* v)
    {
        return custom::Checkbox(label, v);
    }
}

static bool SkinCheckbox(const char* label, bool* value, std::function<void()> applySkin)
{
    if (custom::Checkbox(label, value))
    {
        applySkin();
        return true;
    }
    return false;
}

static bool WeaponSkinCheckbox(const char* label, bool* value, std::function<void()> applySkin)
{
    bool oldValue = *value;
    if (custom::Checkbox(label, value))
    {
        if (oldValue && !*value)
        {
            *value = true;
            applySkin();
        }
        else if (!oldValue && *value)
        {
            applySkin();
        }
        return true;
    }
    return false;
}

#define ID_DIAMOND_CAMO    0x1D37F758
#define ID_RED_SPRITE_CAMO 0x1D37F77E

inline void RenderSkinCategoryContent(int categoryIndex, bool drawSubTabs = false) {
    ImGui::BeginChild("SkinContent", ImVec2(0, 0));
    ImGui::Dummy(ImVec2(0, 8 * c::scale));
    int skin_tab = skinSubTab;

    if (drawSubTabs) {
        const float tabSpacing = 8.0f * c::scale;
        const float tabHeight = 34.0f * c::scale;
        const float tabWidth = ImMax(32.0f, (ImGui::GetContentRegionAvail().x - tabSpacing * 5.0f) / 6.0f);

        ImFont* subTabFont = font::inter_semibold;
        if (!subTabFont)
            subTabFont = F50;
        if (!subTabFont)
        {
            ImGuiIO& io = ImGui::GetIO();
            subTabFont = io.FontDefault;
        }
        if (subTabFont) ImGui::PushFont(subTabFont);
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(tabSpacing, tabSpacing));

        if (custom::Page(skinSubTab == 0, "Character", ImVec2(tabWidth, tabHeight), false, false)) skinSubTab = 0;
        ImGui::SameLine();
        if (custom::Page(skinSubTab == 1, "Watch", ImVec2(tabWidth, tabHeight), false, false)) skinSubTab = 1;
        ImGui::SameLine();
        if (custom::Page(skinSubTab == 2, "Deadbox", ImVec2(tabWidth, tabHeight), false, false)) skinSubTab = 2;
        ImGui::SameLine();
        if (custom::Page(skinSubTab == 3, "Plane", ImVec2(tabWidth, tabHeight), false, false)) skinSubTab = 3;
        ImGui::SameLine();
        if (custom::Page(skinSubTab == 4, "Weapon", ImVec2(tabWidth, tabHeight), false, false)) skinSubTab = 4;
        ImGui::SameLine();
        if (custom::Page(skinSubTab == 5, "Camo", ImVec2(tabWidth, tabHeight), false, false)) skinSubTab = 5;

        ImGui::PopStyleVar();
        if (subTabFont) ImGui::PopFont();
        skin_tab = skinSubTab;
    }

    if (skin_tab == 5) {
        ImGui::Dummy(ImVec2(0, 12 * c::scale));

        static bool camoOff = true;
        static bool camoDiamond = false;
        static bool camoRedSprite = false;

        ImGui::TextColored(c::text::text_active, "Camo Injector");
        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "Apply camo to Mythic / Legendary skins");
        ImGui::Dummy(ImVec2(0, 10 * c::scale));

        auto CamoCheckbox = [](const char* label, bool* v, ImVec4 activeColor) -> bool {
            ImGui::PushStyleColor(ImGuiCol_CheckMark, activeColor);
            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.06f, 0.0f, 0.1f, 0.9f));
            ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.15f, 0.08f, 0.25f, 0.95f));
            ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(0.25f, 0.15f, 0.4f, 1.0f));
            bool result = ImGui::Checkbox(label, v);
            ImGui::PopStyleColor(4);
            return result;
        };

        if (CamoCheckbox("Default / OFF", &camoOff, ImVec4(0.74f, 0.44f, 1.0f, 1.0f))) {
            if (camoOff) {
                camoDiamond = false;
                camoRedSprite = false;
                for (const auto& getitem : itemData) {
                    if (getitem.itemName.find("[M]") != std::string::npos ||
                        getitem.itemName.find("[L]") != std::string::npos) {
                        for (auto conf : weaponConfInstance) {
                            if (!conf) continue;
                            auto* fields = (WeaponConfFields*)((uintptr_t)conf + 0x20);
                            if (fields->ID == getitem.WeaponConf[2]) {
                                fields->DefWeaponSkinID = 0;
                            }
                        }
                    }
                }
            }
        }

        custom::Separator_line();
        ImGui::Dummy(ImVec2(0, 6 * c::scale));

        if (CamoCheckbox("Diamond Camo", &camoDiamond, ImVec4(0.72f, 0.95f, 1.0f, 1.0f))) {
            if (camoDiamond) {
                camoOff = false;
                camoRedSprite = false;
                for (const auto& getitem : itemData) {
                    if (getitem.itemName.find("[M]") != std::string::npos ||
                        getitem.itemName.find("[L]") != std::string::npos) {
                        for (auto conf : weaponConfInstance) {
                            if (!conf) continue;
                            auto* fields = (WeaponConfFields*)((uintptr_t)conf + 0x20);
                            if (fields->ID == getitem.WeaponConf[2]) {
                                fields->DefWeaponSkinID = ID_DIAMOND_CAMO;
                            }
                        }
                    }
                }
            }
        }

        ImGui::Dummy(ImVec2(0, 4 * c::scale));

        if (CamoCheckbox("Red Sprite Camo", &camoRedSprite, ImVec4(1.0f, 0.25f, 0.25f, 1.0f))) {
            if (camoRedSprite) {
                camoOff = false;
                camoDiamond = false;
                for (const auto& getitem : itemData) {
                    if (getitem.itemName.find("[M]") != std::string::npos ||
                        getitem.itemName.find("[L]") != std::string::npos) {
                        for (auto conf : weaponConfInstance) {
                            if (!conf) continue;
                            auto* fields = (WeaponConfFields*)((uintptr_t)conf + 0x20);
                            if (fields->ID == getitem.WeaponConf[2]) {
                                fields->DefWeaponSkinID = ID_RED_SPRITE_CAMO;
                            }
                        }
                    }
                }
            }
        }

        ImGui::Dummy(ImVec2(0, 20 * c::scale));
        const char* statusText = camoOff ? "STATUS: DEFAULT" :
                                 (camoDiamond ? "STATUS: DIAMOND ACTIVE" : "STATUS: RED SPRITE ACTIVE");
        ImU32 statusColor = camoOff ? IM_COL32(150, 150, 150, 255) :
                            (camoDiamond ? IM_COL32(185, 242, 255, 255) : IM_COL32(255, 80, 80, 255));

        ImFont* statusFont = font::inter_semibold ? font::inter_semibold : ImGui::GetFont();
        ImVec2 statusPos = ImGui::GetCursorScreenPos();
        ImGui::GetWindowDrawList()->AddText(statusFont, statusFont->FontSize, statusPos, statusColor, statusText);

        static int eligibleCount = 0;
        static bool counted = false;
        if (!counted && !itemData.empty()) {
            for (const auto& getitem : itemData) {
                if (getitem.itemName.find("[M]") != std::string::npos ||
                    getitem.itemName.find("[L]") != std::string::npos) {
                    eligibleCount++;
                }
            }
            counted = true;
        }

        ImGui::Dummy(ImVec2(0, 8 * c::scale));
        ImGui::TextColored(ImVec4(0.74f, 0.44f, 1.0f, 0.7f), "Eligible weapons: %d", eligibleCount);

        ImGui::EndChild();
        return;
    }

    static char charSearchQuery[256] = "";
    static bool skinSearchWasActive = false;
    static bool charSearchWasActive = false;
    bool charActive = false;
    bool charHovered = false;

    ImGui::Indent(10.0f);
    float totalAvail = ImGui::GetContentRegionAvail().x - 20.0f;
    float halfWidth = (totalAvail - 10.0f) * 0.5f;
    float searchWidth = (skin_tab == 0) ? halfWidth : totalAvail;

    ImGui::TextColored(c::text::text_active, "Search Skin:");
    if (skin_tab == 0) {
        ImGui::SameLine(halfWidth + 20.0f);
        ImGui::TextColored(c::text::text_active, "Search Custom Char:");
    }

    ImGui::AstralInput("##SearchSkin", searchQuery, IM_ARRAYSIZE(searchQuery), ImVec2(searchWidth, 50));
    bool skinClicked = ImGui::IsItemClicked();
    bool skinActive  = ImGui::IsItemActive();
    bool skinHovered = ImGui::IsItemHovered();
    if (skinClicked || (!skinSearchWasActive && skinActive)) {
        activeInputID = "##SearchSkin";
        showKeyboard = true;
    }
    skinSearchWasActive = skinActive;

    if (skin_tab == 0) {
        ImGui::SameLine(0.0f, 10.0f);
        ImGui::AstralInput("##SearchCustomChar", charSearchQuery, IM_ARRAYSIZE(charSearchQuery), ImVec2(halfWidth, 50));
        bool charClicked = ImGui::IsItemClicked();
        charActive = ImGui::IsItemActive();
        charHovered = ImGui::IsItemHovered();
        if (charClicked || (!charSearchWasActive && charActive)) {
            activeInputID = "##SearchCustomChar";
            showKeyboard = true;
        }
        charSearchWasActive = charActive;
    }

    if (showKeyboard && !skinActive && (skin_tab != 0 || (!charSearchWasActive && !charActive && !charHovered)) && ImGui::IsMouseClicked(0)) {
        ImGuiIO& io = ImGui::GetIO();
        float screenHeight = io.DisplaySize.y;
        float keyboardHeight = screenHeight * 0.60f;
        if (ImGui::GetMousePos().y < screenHeight - keyboardHeight) {
            showKeyboard = false;
        }
    }

    custom::Separator_line();
    ImGui::Unindent(30.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 4));
    ImGui::Indent(20.0f);
    std::string searchLower = ToLower(searchQuery);
    int itemCount = 0;
    const char *itemType = "";
    switch (skin_tab)
    {
    case 0:
        itemCount = charData.size();
        itemType = "Characters";
        break;
    case 1:
        itemCount = watch.size();
        itemType = "Watches";
        break;
    case 2:
        itemCount = deadboxF.size();
        itemType = "Deadboxes";
        break;
    case 3:
        itemCount = dropplane.size();
        itemType = "Planes";
        break;
    case 4:
        itemCount = itemData.size();
        itemType = "Weapons";
        break;
    }
    if (skin_tab == 0)
    {
        if (!g_targetCharacters.empty())
        {
            std::string charSearchLower = ToLower(charSearchQuery);

            std::vector<int> filteredIndices;
            std::vector<const char *> filteredNames;

            if (strlen(charSearchQuery) >= MIN_SEARCH_LENGTH)
            {
                for (size_t i = 0; i < g_targetCharacters.size(); i++)
                {
                    const auto &t = g_targetCharacters[i];
                    std::string nameLower = ToLower(t.name);
                    if (nameLower.find(charSearchLower) != std::string::npos)
                    {
                        filteredIndices.push_back(i);
                        filteredNames.push_back(t.name.c_str());
                    }
                }
            }
            else
            {
                for (size_t i = 0; i < g_targetCharacters.size(); i++)
                {
                    filteredIndices.push_back(i);
                    filteredNames.push_back(g_targetCharacters[i].name.c_str());
                }
            }

            if (!filteredNames.empty())
            {
                int currentInFiltered = -1;
                for (size_t i = 0; i < filteredIndices.size(); i++)
                {
                    if (filteredIndices[i] == g_selectedTargetCharIndex)
                    {
                        currentInFiltered = i;
                        break;
                    }
                }
                if (currentInFiltered == -1)
                    currentInFiltered = 0;

                custom::BeginGroup();
                {
                    if (custom::Combo("Custom character:", &currentInFiltered, filteredNames.data(), filteredNames.size()), -1)
                    {
                        g_selectedTargetCharIndex = filteredIndices[currentInFiltered];
                    }
                }
                custom::EndGroup();
            }
            else
            {
                ImGui::TextColored(c::text::text, "No matching characters");
            }
        }
        else
        {
            ImGui::TextColored(c::text::text, "Loading target characters...");
        }
    }

    ImGui::BeginChild("##SkinList", ImVec2(0, ImGui::GetContentRegionAvail().y - 10), true,
                      ImGuiWindowFlags_AlwaysVerticalScrollbar);
    switch (skin_tab)
    {
    case 0:
        if (!charData.empty())
        {
            for (const auto& getchar : charData)
            {
                if (strlen(searchQuery) >= MIN_SEARCH_LENGTH) {
                    std::string nameLower = ToLower(getchar.charName);
                    if (nameLower.find(searchLower) == std::string::npos)
                        continue;
                }

                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));
                SkinCheckbox(getchar.charName.c_str(), &sBool[getchar.charName],
                [&]() {
                    if (sBool[getchar.charName])
                    {
                        for (const auto& other : charData)
                        {
                            if (other.charName != getchar.charName)
                                sBool[other.charName] = false;
                        }
                    }
                    if (CharacterModelConfigInstance.size() > 0 && itemResourceConfigInstance.size() > 0 && RoleConfConfigInstance.size() > 0 && RolePackConfConfigInstance.size() > 0 && BRDeadboxSkinConfigInstance.size() > 0)
                    {
                        int selectedTraitor1P = g_targetCharacters[g_selectedTargetCharIndex].traitor1p;
                        if (g_targetCharacters[g_selectedTargetCharIndex].name == "Charly")
                        {
                            selectedTraitor1P = 710001101;
                        }
                        CharacterModelFields* targetCharacter1P = nullptr;
                        for (auto charModel : CharacterModelConfigInstance)
                        {
                            if (!charModel)
                                continue;
                            CharacterModelFields* characterfields =  (CharacterModelFields*)((uintptr_t)charModel + 0x10);
                            if (characterfields->Traitor1P == selectedTraitor1P)
                            {
                                targetCharacter1P = characterfields;
                                break;
                            }
                            if (characterfields->Traitor1P == 710001101)
                                targetCharacter1P = characterfields;
                        }
                        if (targetCharacter1P)
                        {
                            targetCharacter1P->BRBagModel         = getchar.charModel[0];
                            targetCharacter1P->BRHeadModel        = getchar.charModel[1];
                            targetCharacter1P->BRLobby            = getchar.charModel[2];
                            targetCharacter1P->BRModel            = getchar.charModel[3];
                            targetCharacter1P->BindEffect1P       = getchar.charModel[4];
                            targetCharacter1P->ChangeClipEffect1P = getchar.charModel[5];
                            targetCharacter1P->DefaultModelID     = getchar.charModel[6];
                            targetCharacter1P->Guarder1P          = getchar.charModel[7];
                            targetCharacter1P->Guarder3P          = getchar.charModel[8];
                            targetCharacter1P->GuarderBagModel    = getchar.charModel[9];
                            targetCharacter1P->GuarderHeadModel   = getchar.charModel[10];
                            targetCharacter1P->GuarderLobby       = getchar.charModel[11];
                        }
                        int selectedTraitor3P = g_targetCharacters[g_selectedTargetCharIndex].traitor3p;
                        if (g_targetCharacters[g_selectedTargetCharIndex].name == "Charly")
                        {
                            selectedTraitor3P = 710001102;
                        }
                        CharacterModelFields* targetCharacter3P = nullptr;
                        for (auto charModel : CharacterModelConfigInstance)
                        {
                            if (!charModel)
                                continue;
                            CharacterModelFields* characterfields = (CharacterModelFields*)((uintptr_t)charModel + 0x10);
                            if (characterfields->Traitor3P == selectedTraitor3P)
                            {
                                targetCharacter3P = characterfields;
                                break;
                            }
                            if (characterfields->Traitor1P == 710001102)
                                targetCharacter3P = characterfields;
                        }
                        if (targetCharacter3P)
                        {
                            targetCharacter3P->BRBagModel         = getchar.charModel[0];
                            targetCharacter3P->BRHeadModel        = getchar.charModel[1];
                            targetCharacter3P->BRLobby            = getchar.charModel[2];
                            targetCharacter3P->BRModel            = getchar.charModel[3];
                            targetCharacter3P->BindEffect1P       = getchar.charModel[4];
                            targetCharacter3P->ChangeClipEffect1P = getchar.charModel[5];
                            targetCharacter3P->DefaultModelID     = getchar.charModel[6];
                            targetCharacter3P->Guarder1P          = getchar.charModel[7];
                            targetCharacter3P->Guarder3P          = getchar.charModel[8];
                            targetCharacter3P->GuarderBagModel    = getchar.charModel[9];
                            targetCharacter3P->GuarderHeadModel   = getchar.charModel[10];
                            targetCharacter3P->GuarderLobby       = getchar.charModel[11];
                        }
                        int selectedItemFieldsID = g_targetCharacters[g_selectedTargetCharIndex].itemID;
                        if (g_targetCharacters[g_selectedTargetCharIndex].name == "Charly")
                        {
                            selectedItemFieldsID = 100301208;
                        }
                        ItemResourceFields* targetItem = nullptr;
                        for (auto itemRes : itemResourceConfigInstance)
                        {
                            if (!itemRes)
                                continue;
                            ItemResourceFields* itemFields = (ItemResourceFields*)((uintptr_t)itemRes + 0x10);
                            if (itemFields->ID == selectedItemFieldsID)
                            {
                                targetItem = itemFields;
                                break;
                            }
                            if (itemFields->ID == 100301208)
                                targetItem = itemFields;
                        }
                        if (targetItem)
                        {
                            targetItem->FxAssetID         = getchar.charRes[0];
                            targetItem->InventoryModelID  = getchar.charRes[1];
                            targetItem->ModelAssetIDRaw   = getchar.charRes[2];
                            targetItem->UISmallSpriteName = getchar.charRes2[0];
                            targetItem->UIMiniSpriteName  = getchar.charRes2[1];
                            targetItem->UISpriteName      = getchar.charRes2[2];
                            targetItem->UISquareSpriteName= getchar.charRes2[3];
                        }
                        int selectedRoleFID = g_targetCharacters[g_selectedTargetCharIndex].roleID;
                        if (g_targetCharacters[g_selectedTargetCharIndex].name == "Charly")
                        {
                            selectedRoleFID = 100301208;
                        }
                        RoleConfFields* targetRole = nullptr;
                        for (auto roles : RoleConfConfigInstance)
                        {
                            if (!roles)
                                continue;
                            RoleConfFields* roleF = (RoleConfFields*)((uintptr_t)roles + 0x10);
                            if (roleF->ID == selectedRoleFID)
                            {
                                targetRole = roleF;
                                break;
                            }
                            if (roleF->ID == 100301208)
                                targetRole = roleF;
                        }
                        if (targetRole)
                        {
                            targetRole->roleLeftArmID        = getchar.charRole[0];
                            targetRole->roleFinalSuitID      = getchar.charRole[1];
                            targetRole->roleBasicHologramID  = getchar.charRole[2];
                            targetRole->ColorID              = getchar.charRole[3];
                            targetRole->ColorSubID           = getchar.charRole[4];
                            targetRole->ShowRare             = getchar.charRole[5];
                            targetRole->RoleLvGroupID        = getchar.charRole[6];
                            targetRole->RolePackID           = getchar.charRole[7];
                            targetRole->LOCID_Name           = getchar.charRole2[0];
                        }
                        int selectedRolePackID = g_targetCharacters[g_selectedTargetCharIndex].rolepackID;
                        for (auto pack : RolePackConfConfigInstance)
                        {
                            if (!pack)
                                continue;
                            packfields = (RolePackFields *)((uintptr_t)pack + 0x10);
                            if (packfields->RolePackID == selectedRolePackID)
                            {
                                packfields->EntryAnimID = getchar.charPack[1];
                                packfields->GestureId = getchar.charPack[2];
                                packfields->HandEffectUI = getchar.charPack[3];
                                packfields->LoadingFrame = getchar.charPack[4];
                                packfields->KillStreakSkinID = getchar.charPack[5];
                            }
                        }
                    }
                });
                ImGui::PopStyleVar();
            }
        }
        else
        {
            ImGui::TextColored(ImVec4(1, 0.5f, 0.5f, 1), "No character skins loaded");
        }
        break;
    case 1:
        if (watch.size() > 0)
        {
            for (const auto& forwatch : watch)
            {
                if (strlen(searchQuery) >= MIN_SEARCH_LENGTH) {
                    std::string nameLower = ToLower(forwatch.watchname);
                    if (nameLower.find(searchLower) == std::string::npos)
                        continue;
                }
                if (forwatch.watchname.find("Watch") == std::string::npos ||
                        forwatch.watchname.find("Face") != std::string::npos) continue;

                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));
                SkinCheckbox(forwatch.watchname.c_str(), &sBool[forwatch.watchname],
                [&]() {
                    if (sBool[forwatch.watchname])
                    {
                        for (const auto& other : watch)
                        {
                            if (other.watchname != forwatch.watchname)
                                sBool[other.watchname] = false;
                        }
                    }
                    int selectedTraitor1P = g_targetCharacters[g_selectedTargetCharIndex].traitor1p;
                    if (g_targetCharacters[g_selectedTargetCharIndex].name == "Charly")
                    {
                        selectedTraitor1P = 710001101;
                    }
                    if (CharacterModelConfigInstance.size() > 0)
                    {
                        for (auto charModel : CharacterModelConfigInstance)
                        {
                            if (!charModel)
                                continue;
                            characterfields = (CharacterModelFields *)((uintptr_t)charModel + 0x10);
                            if (characterfields->Traitor1P == selectedTraitor1P)
                                characterfields->BindEffect1P = forwatch.watchvalue;
                        }
                    }
                });
                ImGui::PopStyleVar();
            }
        }
        else
        {
            ImGui::TextColored(ImVec4(1, 0.5f, 0.5f, 1), "No watch skins loaded");
        }
        break;
    case 2:
        if (deadboxF.size() > 0)
        {
            for (const auto& deadx : deadboxF)
            {
                if (strlen(searchQuery) >= MIN_SEARCH_LENGTH) {
                    std::string nameLower = ToLower(deadx.deadname);
                    if (nameLower.find(searchLower) == std::string::npos)
                        continue;
                }

                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));
                SkinCheckbox(deadx.deadname.c_str(), &sBool[deadx.deadname],
                [&]() {
                    if (sBool[deadx.deadname])
                    {
                        for (const auto& other : deadboxF)
                        {
                            if (other.deadname != deadx.deadname)
                                sBool[other.deadname] = false;
                        }
                    }
                    if (CharacterModelConfigInstance.size() > 0)
                    {
                        for (auto deadID : BRDeadboxSkinConfigInstance)
                        {
                            if (!deadID)
                                continue;
                            deadboxFields = (BRDeadboxSkinFields *)((uintptr_t)deadID + 0x10);
                            if (deadboxFields->ID == 180300004)
                            {
                                deadboxFields->ColorID = deadx.dead[0];
                                deadboxFields->DeadBoxEffectAsset = deadx.dead[1];
                                deadboxFields->Flag = deadx.dead[2];
                                deadboxFields->FlagAsset = deadx.dead[3];
                                deadboxFields->ModelAsset3P = deadx.dead[4];
                                deadboxFields->ModelAssetUI = deadx.dead[5];
                            }
                        }
                    }
                });
                ImGui::PopStyleVar();
            }
        }
        else
        {
            ImGui::TextColored(ImVec4(1, 0.5f, 0.5f, 1), "No deadbox skins loaded");
        }
        break;
    case 3:
        if (dropplane.size() > 0)
        {
            for (const auto& planex : dropplane)
            {
                if (strlen(searchQuery) >= MIN_SEARCH_LENGTH) {
                    std::string nameLower = ToLower(planex.planename);
                    if (nameLower.find(searchLower) == std::string::npos)
                        continue;
                }

                ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));
                SkinCheckbox(planex.planename.c_str(), &sBool[planex.planename],
                [&]() {
                    if (sBool[planex.planename])
                    {
                        for (const auto& other : dropplane)
                        {
                            if (other.planename != planex.planename)
                                sBool[other.planename] = false;
                        }
                    }
                    if (BRDropPlaneSkinConfigInstance.size() > 0)
                    {
                        for (auto planeID : BRDropPlaneSkinConfigInstance)
                        {
                            if (!planeID)
                                continue;
                            dropplaneFields = (BRDropPlaneSkinFields *)((uintptr_t)planeID + 0x10);
                            if (dropplaneFields->ID == 0)
                            {
                                dropplaneFields->ColorID = planex.plane[0];
                                dropplaneFields->ModelAsset1P = planex.plane[1];
                                dropplaneFields->ModelAsset3P = planex.plane[2];
                                dropplaneFields->ModelAssetCutScene = planex.plane[3];
                                dropplaneFields->ModelAssetUI = planex.plane[4];
                                dropplaneFields->Priority = planex.plane[5];
                            }
                        }
                    }
                });
                ImGui::PopStyleVar();
            }
        }
        else
        {
            ImGui::TextColored(ImVec4(1, 0.5f, 0.5f, 1), "No plane skins loaded");
        }
        break;
    case 4:
        if (!itemData.empty())
        {
            auto applyWeaponSkin = [&](const auto& getitem) {
                int baseID = getitem.WeaponConf[0];
                int extraID = getitem.WeaponExtra[0];
                int skinID = getitem.WeaponConf[2];
                int itemID = getitem.Item2Inventory[0];
                int lootID = getitem.Item2Inventory[3];
                for (auto item : itemInventoryInstance)
                {
                    if (!item)
                        continue;
                    item2Fields = (Item2InventoryFields *)((uintptr_t)item + 0x20);
                    if (item2Fields->ItemID == getitem.Item2Inventory[0] || item2Fields->ItemID == getitem.Item2Inventory[3])
                    {
                        item2Fields->WeaponAssetGroupID = getitem.Item2Inventory[1];
                        item2Fields->WeaponIconID = getitem.Item2Inventory[2];
                    }
                }
                for (auto conf : weaponConfInstance)
                {
                    if (!conf)
                        continue;
                    weaponconfFields = (WeaponConfFields *)((uintptr_t)conf + 0x20);
                    if (baseID == weaponconfFields->ID ||
                            extraID == weaponconfFields->ID ||
                            skinID == weaponconfFields->ID ||
                            itemID == weaponconfFields->ID ||
                            lootID == weaponconfFields->ID)
                    {
                        weaponconfFields->DefWeaponSkinID = getitem.WeaponConf[2];
                        weaponconfFields->DefaultKillBrocast = getitem.WeaponConf[3];
                        weaponconfFields->ExternalUnVisible = true;

                        for (auto skinConf : weaponConfInstance)
                        {
                            if (!skinConf)
                                continue;
                            WeaponConfFields *skinFields = (WeaponConfFields *)((uintptr_t)skinConf + 0x20);
                            if (skinFields->ID == getitem.WeaponConf[2])
                            {
                                weaponconfFields->LOCID_Name = skinFields->LOCID_Name;
                                break;
                            }
                        }
                        if (baseID == weaponconfFields->ID)
                        {
                            weaponconfFields->ColorID = getitem.WeaponConf[1];
                        }
                    }
                }
                if (getitem.WeaponExtra[4] > 0)
                {
                    activeKillEffects[baseID] = getitem.WeaponExtra[4];
                    activeKillEffects[extraID] = getitem.WeaponExtra[4];
                    activeKillEffects[skinID] = getitem.WeaponExtra[4];
                    activeKillEffects[itemID] = getitem.WeaponExtra[4];
                    activeKillEffects[lootID] = getitem.WeaponExtra[4];
                }
                for (auto extra : weaponExtraInstance)
                {
                    if (!extra)
                        continue;
                    weaponextraFields = (WeaponConfExtraFields *)((uintptr_t)extra + 0x10);
                    if (weaponextraFields->ID == baseID ||
                            weaponextraFields->ID == extraID ||
                            weaponextraFields->ID == skinID ||
                            weaponextraFields->ID == itemID ||
                            weaponextraFields->ID == lootID)
                    {
                        if (getitem.itemName.find("[MYTHIC]") != std::string::npos)
                        {
                            weaponextraFields->DefaultMythicArmor = getitem.WeaponExtra[1];
                            if (getitem.itemName.find("[MYTHIC] AK117 - Memento Mori") != std::string::npos)
                            {
                                weaponextraFields->DefaultMythicSig = getitem.WeaponExtra[2];
                            }
                        }
                        weaponextraFields->DefaultDeadReplayEffectId = getitem.WeaponExtra[3];
                        weaponextraFields->DefaultKillEffectId = getitem.WeaponExtra[4];
                    }
                }
                for (auto asset : weaponAssetGroupInstance)
                {
                    if (!asset)
                        continue;
                    weaponAssetFields = (WeaponAssetGroupFields *)((uintptr_t)asset + 0x40);
                    if (weaponAssetFields->Id == getitem.WeaponAsset[0])
                    {
                        if (getitem.WeaponAsset[1] / 10 == getitem.WeaponAsset[2] / 10 && getitem.WeaponAsset[1] > 0)
                        {
                            weaponAssetFields->FireEffectGroupID = getitem.WeaponAsset[1];
                        }
                    }
                }
                for (auto itemResource : itemResourceConfigInstance)
                {
                    if (!itemResource)
                        continue;
                    itemFields = (ItemResourceFields *)((uintptr_t)itemResource + 0x10);
                    if (itemFields->ID == skinID)
                    {
                        itemFields->FxAssetID = getitem.WeaponConf[0];
                        itemFields->InventoryModelID = getitem.ItemResInt[1];
                        itemFields->ModelAssetIDRaw = getitem.ItemResInt[2];
                        itemFields->UIMiniSpriteName = getitem.ItemRes[0];
                        itemFields->UISmallSpriteName = getitem.ItemRes[1];
                        itemFields->UISpriteName = getitem.ItemRes[2];
                        itemFields->UISquareSpriteName = getitem.ItemRes[3];
                    }
                    if (itemFields->ID == skinID)
                    {
                        itemFields->FxAssetID = getitem.ItemResInt[0];
                        itemFields->InventoryModelID = getitem.ItemResInt[1];
                        itemFields->ModelAssetIDRaw = getitem.ItemResInt[2];
                        itemFields->UIMiniSpriteName = getitem.ItemRes[0];
                        itemFields->UISmallSpriteName = getitem.ItemRes[1];
                        itemFields->UISquareSpriteName = getitem.ItemRes[3];
                    }
                    if (itemFields->ID == itemID)
                    {
                        itemFields->FxAssetID = getitem.ItemResInt[0];
                        itemFields->InventoryModelID = getitem.ItemResInt[1];
                        itemFields->ModelAssetIDRaw = getitem.ItemResInt[2];
                        itemFields->UIMiniSpriteName = getitem.ItemRes[0];
                        itemFields->UISmallSpriteName = getitem.ItemRes[1];
                        itemFields->UISpriteName = getitem.ItemRes[2];
                    }
                    if (itemFields->ID == lootID)
                    {
                        itemFields->FxAssetID = getitem.ItemResInt[0];
                        itemFields->InventoryModelID = getitem.ItemResInt[1];
                        itemFields->ModelAssetIDRaw = getitem.ItemResInt[2];
                        itemFields->UIMiniSpriteName = getitem.ItemRes[0];
                        itemFields->UISmallSpriteName = getitem.ItemRes[1];
                        itemFields->UISpriteName = getitem.ItemRes[2];
                        itemFields->UISquareSpriteName = getitem.ItemRes[3];
                    }
                    if (itemFields->ID == extraID)
                    {
                        itemFields->FxAssetID = getitem.ItemResInt[0];
                        itemFields->InventoryModelID = getitem.ItemResInt[1];
                        itemFields->ModelAssetIDRaw = getitem.ItemResInt[2];
                        itemFields->UIMiniSpriteName = getitem.ItemRes[0];
                        itemFields->UISmallSpriteName = getitem.ItemRes[1];
                        itemFields->UISpriteName = getitem.ItemRes[2];
                    }
                    if (itemFields->ID == baseID)
                    {
                        itemFields->FxAssetID = getitem.ItemResInt[0];
                        itemFields->InventoryModelID = getitem.ItemResInt[1];
                        itemFields->ModelAssetIDRaw = getitem.ItemResInt[2];
                        itemFields->UIMiniSpriteName = getitem.ItemRes[0];
                        itemFields->UISmallSpriteName = getitem.ItemRes[1];
                        itemFields->UISpriteName = getitem.ItemRes[2];
                    }
                }

                int fireEffectID = 0;
                for (auto asset : weaponAssetGroupInstance)
                {
                    if (!asset)
                        continue;
                    weaponAssetFields = (WeaponAssetGroupFields *)((uintptr_t)asset + 0x40);
                    if (weaponAssetFields && weaponAssetFields->Id == getitem.WeaponAsset[0])
                    {
                        fireEffectID = weaponAssetFields->FireEffectGroupID;
                        break;
                    }
                }

                if (fireEffectID > 0)
                {
                    for (auto fireConf : weaponFireEffectInstance)
                    {
                        if (!fireConf)
                            continue;
                        WeaponFireEffectFields *wfFields = (WeaponFireEffectFields *)((uintptr_t)fireConf + 0x10);
                        if (!Tools::IsPtrValid(wfFields))
                            continue;

                        if (wfFields->Id == fireEffectID)
                        {
                            wfFields->LevelEffectDelayTimeUI = 0.1f;
                            wfFields->LevelEffectDelayTimeUI_King = 0.1f;
                            auto *killCountArray = *(Array<int> **)((uintptr_t)fireConf + 0x118);
                            if (killCountArray && Tools::IsPtrValid(killCountArray))
                            {
                                for (int i = 0; i < killCountArray->getLength(); ++i)
                                {
                                    killCountArray->m_Items[i] = 0;
                                }
                            }
                            auto *killCountArrayKing = *(Array<int> **)((uintptr_t)fireConf + 0x120);
                            if (killCountArrayKing && Tools::IsPtrValid(killCountArrayKing))
                            {
                                for (int i = 0; i < killCountArrayKing->getLength(); ++i)
                                {
                                    killCountArrayKing->m_Items[i] = 0;
                                }
                            }
                            break;
                        }
                    }
                }
            };

            for (const auto& getitem : itemData)
            {
                if (strlen(searchQuery) >= MIN_SEARCH_LENGTH) {
                    std::string nameLower = ToLower(getitem.itemName);
                    if (nameLower.find(searchLower) == std::string::npos)
                        continue;
                }
                if (sBool[getitem.itemName])
                {
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));
                    WeaponSkinCheckbox(getitem.itemName.c_str(), &sBool[getitem.itemName],
                                       [&, getitem]()
                    {
                        std::string currentBase = GetBaseWeaponName(getitem.itemName);
                        for (const auto& other : itemData)
                        {
                            if (other.itemName != getitem.itemName &&
                                    GetBaseWeaponName(other.itemName) == currentBase)
                            {
                                sBool[other.itemName] = false;
                            }
                        }
                        applyWeaponSkin(getitem);
                    });
                    ImGui::PopStyleVar();
                }
            }

            for (const auto& getitem : itemData)
            {
                if (strlen(searchQuery) >= MIN_SEARCH_LENGTH) {
                    std::string nameLower = ToLower(getitem.itemName);
                    if (nameLower.find(searchLower) == std::string::npos)
                        continue;
                }
                if (!sBool[getitem.itemName])
                {
                    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(8, 8));
                    WeaponSkinCheckbox(getitem.itemName.c_str(), &sBool[getitem.itemName],
                                       [&, getitem]()
                    {
                        std::string currentBase = GetBaseWeaponName(getitem.itemName);
                        for (const auto& other : itemData)
                        {
                            if (other.itemName != getitem.itemName &&
                                    GetBaseWeaponName(other.itemName) == currentBase)
                            {
                                sBool[other.itemName] = false;
                            }
                        }
                        applyWeaponSkin(getitem);
                    });
                    ImGui::PopStyleVar();
                }
            }
        }
        else
        {
            ImGui::TextColored(ImVec4(1, 0.5f, 0.5f, 1), "No weapon skins loaded");
        }
        break;
    }
    ImGui::EndChild();
    if (showKeyboard) {
        if (activeInputID == "##SearchCustomChar") {
            RenderVirtualKeyboard("##VirtualKeyboard", charSearchQuery, IM_ARRAYSIZE(charSearchQuery), &showKeyboard);
        } else {
            RenderVirtualKeyboard("##VirtualKeyboard", searchQuery, IM_ARRAYSIZE(searchQuery), &showKeyboard);
        }
    }
}

inline void RenderTab4Content() {
    RenderSkinCategoryContent(skinSubTab, true);
}
