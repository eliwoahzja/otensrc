#ifndef GUEST_ACCOUNT_MANAGER_H
#define GUEST_ACCOUNT_MANAGER_H

#include "../ImGui/imgui.h"
#include "../ImGui/imgui_internal.h"
#include <string>
#include <vector>
#include <unistd.h>
#include <cstdlib>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <chrono>
#include <thread>

extern float menu[4];

#ifndef OBFUSCATE
#define OBFUSCATE(str) str
#endif

static const char* guestAccountPaths[] = {
    "/data/data/com.mobile.survive/virtual/data/user/0/com.garena.game.codm/shared_prefs/lastUserId.xml",
    "/data/data/com.mobile.survive/virtual/data/user/0/com.garena.game.codm/shared_prefs/gsdk_prefs.xml",
    "/data/data/com.mobile.survive/virtual/data/user/0/com.garena.game.codm/shared_prefs/MFILE.xml",
    "/data/data/com.mobile.survive/virtual/data/user/0/com.garena.game.codm/shared_prefs/apm_cfg.xml",
    "/data/data/com.mobile.survive/virtual/data/user/0/com.garena.game.codm/shared_prefs/appsflyer-data.xml",
    "/data/data/com.mobile.survive/virtual/data/user/0/com.garena.game.codm/shared_prefs/buglySdkInfos.xml",
    "/data/data/com.mobile.survive/virtual/data/user/0/com.garena.game.codm/shared_prefs/CentauriHTTPSP.xml",
    "/data/data/com.mobile.survive/virtual/data/user/0/com.garena.game.codm/shared_prefs/CentauriOverseaIP.xml",
    "/data/data/com.mobile.survive/virtual/data/user/0/com.garena.game.codm/shared_prefs/com.garena.game.codm.PayCachePreference_crypto.xml",
    "/data/data/com.mobile.survive/virtual/data/user/0/com.garena.game.codm/shared_prefs/com.garena.game.codm_preferences.xml",
    "/data/data/com.mobile.survive/virtual/data/user/0/com.garena.game.codm/shared_prefs/com.garena.game.codm.v2.playerprefs.xml",
    "/data/data/com.mobile.survive/virtual/data/user/0/com.garena.game.codm/shared_prefs/com.garena.msdk.persist.fallback.xml",
    "/data/data/com.samsung.mobile/virtual/data/user/0/com.garena.game.codm/shared_prefs/lastUserId.xml",
    "/data/data/com.samsung.mobile/virtual/data/user/0/com.garena.game.codm/shared_prefs/gsdk_prefs.xml",
    "/data/data/com.samsung.mobile/virtual/data/user/0/com.garena.game.codm/shared_prefs/MFILE.xml",
    "/data/data/com.samsung.mobile/virtual/data/user/0/com.garena.game.codm/shared_prefs/apm_cfg.xml",
    "/data/data/com.samsung.mobile/virtual/data/user/0/com.garena.game.codm/shared_prefs/appsflyer-data.xml",
    "/data/data/com.samsung.mobile/virtual/data/user/0/com.garena.game.codm/shared_prefs/buglySdkInfos.xml",
    "/data/data/com.samsung.mobile/virtual/data/user/0/com.garena.game.codm/shared_prefs/CentauriHTTPSP.xml",
    "/data/data/com.samsung.mobile/virtual/data/user/0/com.garena.game.codm/shared_prefs/CentauriOverseaIP.xml",
    "/data/data/com.samsung.mobile/virtual/data/user/0/com.garena.game.codm/shared_prefs/com.garena.game.codm.PayCachePreference_crypto.xml",
    "/data/data/com.samsung.mobile/virtual/data/user/0/com.garena.game.codm/shared_prefs/com.garena.game.codm_preferences.xml",
    "/data/data/com.samsung.mobile/virtual/data/user/0/com.garena.game.codm/shared_prefs/com.garena.game.codm.v2.playerprefs.xml",
    "/data/data/com.samsung.mobile/virtual/data/user/0/com.garena.game.codm/shared_prefs/com.garena.msdk.persist.fallback.xml",
    "/data/data/com.pengyou.cloneapp/chaos/data/user/0/com.garena.game.codm/shared_prefs/lastUserId.xml",
    "/data/data/com.pengyou.cloneapp/chaos/data/user/0/com.garena.game.codm/shared_prefs/gsdk_prefs.xml",
    "/data/data/com.pengyou.cloneapp/chaos/data/user/0/com.garena.game.codm/shared_prefs/MFILE.xml",
    "/data/data/com.pengyou.cloneapp/chaos/data/user/0/com.garena.game.codm/shared_prefs/apm_cfg.xml",
    "/data/data/com.pengyou.cloneapp/chaos/data/user/0/com.garena.game.codm/shared_prefs/appsflyer-data.xml",
    "/data/data/com.pengyou.cloneapp/chaos/data/user/0/com.garena.game.codm/shared_prefs/buglySdkInfos.xml",
    "/data/data/com.pengyou.cloneapp/chaos/data/user/0/com.garena.game.codm/shared_prefs/CentauriHTTPSP.xml",
    "/data/data/com.pengyou.cloneapp/chaos/data/user/0/com.garena.game.codm/shared_prefs/CentauriOverseaIP.xml",
    "/data/data/com.pengyou.cloneapp/chaos/data/user/0/com.garena.game.codm/shared_prefs/com.garena.game.codm.PayCachePreference_crypto.xml",
    "/data/data/com.pengyou.cloneapp/chaos/data/user/0/com.garena.game.codm/shared_prefs/com.garena.game.codm_preferences.xml",
    "/data/data/com.pengyou.cloneapp/chaos/data/user/0/com.garena.game.codm/shared_prefs/com.garena.game.codm.v2.playerprefs.xml",
    "/data/data/com.pengyou.cloneapp/chaos/data/user/0/com.garena.game.codm/shared_prefs/com.garena.msdk.persist.fallback.xml",
    "/data/user/0/com.garena.game.codm/shared_prefs/lastUserId.xml",
    "/data/user/0/com.garena.game.codm/shared_prefs/gsdk_prefs.xml",
    "/data/user/0/com.garena.game.codm/shared_prefs/MFILE.xml",
    "/data/user/0/com.garena.game.codm/shared_prefs/apm_cfg.xml",
    "/data/user/0/com.garena.game.codm/shared_prefs/appsflyer-data.xml",
    "/data/user/0/com.garena.game.codm/shared_prefs/buglySdkInfos.xml",
    "/data/user/0/com.garena.game.codm/shared_prefs/CentauriHTTPSP.xml",
    "/data/user/0/com.garena.game.codm/shared_prefs/CentauriOverseaIP.xml",
    "/data/user/0/com.garena.game.codm/shared_prefs/com.garena.game.codm.PayCachePreference_crypto.xml",
    "/data/user/0/com.garena.game.codm/shared_prefs/com.garena.game.codm_preferences.xml",
    "/data/user/0/com.garena.game.codm/shared_prefs/com.garena.game.codm.v2.playerprefs.xml",
    "/data/user/0/com.garena.game.codm/shared_prefs/com.garena.msdk.persist.fallback.xml",
    "/data/data/com.garena.game.codm/shared_prefs/lastUserId.xml",
    "/data/data/com.garena.game.codm/shared_prefs/gsdk_prefs.xml",
    "/data/data/com.garena.game.codm/shared_prefs/MFILE.xml",
    "/data/data/com.garena.game.codm/shared_prefs/apm_cfg.xml",
    "/data/data/com.garena.game.codm/shared_prefs/appsflyer-data.xml",
    "/data/data/com.garena.game.codm/shared_prefs/buglySdkInfos.xml",
    "/data/data/com.garena.game.codm/shared_prefs/CentauriHTTPSP.xml",
    "/data/data/com.garena.game.codm/shared_prefs/CentauriOverseaIP.xml",
    "/data/data/com.garena.game.codm/shared_prefs/com.garena.game.codm.PayCachePreference_crypto.xml",
    "/data/data/com.garena.game.codm/shared_prefs/com.garena.game.codm_preferences.xml",
    "/data/data/com.garena.game.codm/shared_prefs/com.garena.game.codm.v2.playerprefs.xml",
    "/data/data/com.garena.game.codm/shared_prefs/com.garena.msdk.persist.fallback.xml"
};

static const int TOTAL_FILES = sizeof(guestAccountPaths) / sizeof(guestAccountPaths[0]);
static const int TOTAL_CYCLES = 2;
static const int TOTAL_RESET_OPERATIONS = TOTAL_FILES * TOTAL_CYCLES;
static const int TOTAL_SAVE_OPERATIONS = TOTAL_FILES;

static std::string GetPackageNameFromPath(const std::string& path) {

    if (path.find("/com.mobile.survive/") != std::string::npos) {
        return "com.mobile.survive";
    } else if (path.find("/com.samsung.mobile/") != std::string::npos) {
        return "com.samsung.mobile";
    } else if (path.find("/com.pengyou.cloneapp/") != std::string::npos) {
        return "com.pengyou.cloneapp";
    } else if (path.find("/data/user/0/") != std::string::npos) {
        return "user_data";
    } else if (path.find("/data/data/com.garena.game.codm/") != std::string::npos) {
        return "original_codm";
    }
    return "unknown_package";
}

static std::string GetFileNameFromPath(const std::string& path) {
    size_t lastSlash = path.find_last_of('/');
    if (lastSlash != std::string::npos) {
        return path.substr(lastSlash + 1);
    }
    return path;
}

static bool CreateDirectoryIfNotExists(const std::string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {

        if (mkdir(path.c_str(), 0755) != 0) {
            return false;
        }
    } else if (!(info.st_mode & S_IFDIR)) {

        return false;
    }
    return true;
}

static void ResetBannedGuestAccountFiles(float& progress) {
    progress = 0.0f;
    int completedOperations = 0;

    for (int cycle = 1; cycle <= 2; ++cycle) {
        for (const char* path : guestAccountPaths) {
            std::string command = "rm -f \"" + std::string(path) + "\"";
            system(command.c_str());

            completedOperations++;
            progress = static_cast<float>(completedOperations) / TOTAL_RESET_OPERATIONS * 100.0f;

            usleep(50 * 1000);
        }

        usleep(200 * 1000);
    }

    progress = 100.0f;
}

static void SaveGuestAccountFiles(float& progress) {
    progress = 0.0f;
    int completedOperations = 0;

    std::string baseSaveDir = "/storage/emulated/0/GuestAccountBackup/";

    if (!CreateDirectoryIfNotExists(baseSaveDir)) {

        baseSaveDir = "/data/user/0/guest_account_backups/";
        CreateDirectoryIfNotExists(baseSaveDir);
    }

    for (const char* path : guestAccountPaths) {
        std::string sourcePath = std::string(path);

        std::string packageName = GetPackageNameFromPath(sourcePath);
        std::string fileName = GetFileNameFromPath(sourcePath);

        std::string packageDir = baseSaveDir + packageName + "/";
        CreateDirectoryIfNotExists(packageDir);

        std::string command = "cp \"" + sourcePath + "\" \"" + packageDir + fileName + "\"";
        int result = system(command.c_str());

        completedOperations++;
        progress = static_cast<float>(completedOperations) / TOTAL_SAVE_OPERATIONS * 100.0f;

        usleep(50 * 1000);
    }

    progress = 100.0f;
}

static void ShowGuestAccountManager(ImDrawList* draw) {
    static enum State {
        IDLE,
        SHOW_OPTIONS,
        SHOW_CONFIRM_RESET,
        SHOW_CONFIRM_SAVE,
        RESETTING,
        SAVING,
        RESET_COMPLETED,
        SAVE_COMPLETED
    } currentState = IDLE;

    static float progress = 0.0f;
    static bool operationInProgress = false;
    static std::chrono::time_point<std::chrono::steady_clock> operationStartTime;
    static std::string currentOperationName;

    if (currentState == IDLE) {

        ImVec2 buttonSize = ImVec2(280, 40);
        ImGui::SetCursorPosX((315 - buttonSize.x) / 2);

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(menu[0], menu[1], menu[2], 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(menu[0], menu[1], menu[2], 0.9f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(menu[0], menu[1], menu[2], 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImColor(255, 255, 255, 180).Value);
        ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255).Value);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);

        if (ImGui::Button(OBFUSCATE("MANAGE GUEST ACCOUNT"), buttonSize)) {
            currentState = SHOW_OPTIONS;
        }

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(5);

        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Reset or save guest account files");
        }
    }

    if (currentState == SHOW_OPTIONS) {
        ImGui::OpenPopup("GuestAccountOptions");
    }

    ImGui::SetNextWindowPos(
        ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f),
        ImGuiCond_Always,
        ImVec2(0.5f, 0.5f)
    );

    if (ImGui::BeginPopupModal("GuestAccountOptions", nullptr,
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse))
    {
        ImGui::Text("Guest Account Management");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(menu[0], menu[1], menu[2], 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(menu[0], menu[1], menu[2], 0.9f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(menu[0], menu[1], menu[2], 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImColor(255, 255, 255, 180).Value);
        ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255).Value);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);

        ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f), "Reset Guest Account:");
        ImGui::Text("Delete all guest account files and restart app.");
        if (ImGui::Button("RESET GUEST", ImVec2(-1, 40))) {
            currentState = SHOW_CONFIRM_RESET;
            ImGui::CloseCurrentPopup();
        }

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(5);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(menu[0], menu[1], menu[2], 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(menu[0], menu[1], menu[2], 0.9f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(menu[0], menu[1], menu[2], 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImColor(255, 255, 255, 180).Value);
        ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255).Value);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);

        ImGui::TextColored(ImVec4(0.5f, 1.0f, 0.5f, 1.0f), "Save Guest Account:");
        ImGui::Text("Create backup of guest account files.");
        if (ImGui::Button("SAVE GUEST", ImVec2(-1, 40))) {
            currentState = SHOW_CONFIRM_SAVE;
            ImGui::CloseCurrentPopup();
        }

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(5);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.6f, 0.6f, 0.9f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImColor(255, 255, 255, 180).Value);
        ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255).Value);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);

        if (ImGui::Button("CANCEL", ImVec2(-1, 30))) {
            currentState = IDLE;
            ImGui::CloseCurrentPopup();
        }

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(5);

        ImGui::EndPopup();
    }

    if (currentState == SHOW_CONFIRM_RESET) {
        ImGui::OpenPopup("ConfirmResetGuest");
    }

    ImGui::SetNextWindowPos(
        ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f),
        ImGuiCond_Always,
        ImVec2(0.5f, 0.5f)
    );

    if (ImGui::BeginPopupModal("ConfirmResetGuest", nullptr,
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse))
    {
        ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "WARNING: This will delete game data!");
        ImGui::Spacing();
        ImGui::Text("Reset Banned Guest Account?");
        ImGui::Text("This will delete all guest account files and restart the app.");
        ImGui::Spacing();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(menu[0], menu[1], menu[2], 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(menu[0], menu[1], menu[2], 0.9f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(menu[0], menu[1], menu[2], 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImColor(255, 255, 255, 180).Value);
        ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255).Value);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);

        if (ImGui::Button("RESET", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 0))) {
            currentState = RESETTING;
            progress = 0.0f;
            operationInProgress = true;
            operationStartTime = std::chrono::steady_clock::now();
            currentOperationName = "Resetting";
            ImGui::CloseCurrentPopup();
        }

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(5);

        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.6f, 0.6f, 0.9f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImColor(255, 255, 255, 180).Value);
        ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255).Value);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);

        if (ImGui::Button("CANCEL", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
            currentState = IDLE;
            ImGui::CloseCurrentPopup();
        }

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(5);

        ImGui::EndPopup();
    }

    if (currentState == SHOW_CONFIRM_SAVE) {
        ImGui::OpenPopup("ConfirmSaveGuest");
    }

    ImGui::SetNextWindowPos(
        ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f),
        ImGuiCond_Always,
        ImVec2(0.5f, 0.5f)
    );

    if (ImGui::BeginPopupModal("ConfirmSaveGuest", nullptr,
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse))
    {
        ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "Create Backup of Guest Account");
        ImGui::Spacing();
        ImGui::Text("Save Guest Account Files?");
        ImGui::Text("This will create a backup of all guest account files.");
        ImGui::Text("Files will be saved in: /storage/emulated/0/GuestAccountBackup/");
        ImGui::Spacing();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(menu[0], menu[1], menu[2], 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(menu[0], menu[1], menu[2], 0.9f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(menu[0], menu[1], menu[2], 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImColor(255, 255, 255, 180).Value);
        ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255).Value);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);

        if (ImGui::Button("SAVE", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 0))) {
            currentState = SAVING;
            progress = 0.0f;
            operationInProgress = true;
            operationStartTime = std::chrono::steady_clock::now();
            currentOperationName = "Saving";
            ImGui::CloseCurrentPopup();
        }

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(5);

        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.6f, 0.6f, 0.6f, 0.9f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImColor(255, 255, 255, 180).Value);
        ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255).Value);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);

        if (ImGui::Button("CANCEL", ImVec2(ImGui::GetContentRegionAvail().x, 0))) {
            currentState = IDLE;
            ImGui::CloseCurrentPopup();
        }

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(5);

        ImGui::EndPopup();
    }

    if (currentState == RESETTING || currentState == SAVING) {
        ImGui::OpenPopup("OperationProgress");
    }

    ImGui::SetNextWindowPos(
        ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f),
        ImGuiCond_Always,
        ImVec2(0.5f, 0.5f)
    );

    if (ImGui::BeginPopupModal("OperationProgress", nullptr,
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse))
    {

        auto currentTime = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - operationStartTime).count();

        if (progress < 100.0f) {

            if (currentState == RESETTING) {
                ImGui::Text("Resetting guest account...");
            } else {
                ImGui::Text("Saving guest account files...");
            }

            ImGui::Spacing();

            ImGui::ProgressBar(progress / 100.0f, ImVec2(-1, 20));

            ImGui::Text("%.1f%% complete", progress);

            if (currentState == RESETTING) {
                ImGui::Text("Deleting files...");
            } else {
                ImGui::Text("Copying files...");
            }

            if (progress > 0) {
                float remainingPercent = 100.0f - progress;
                float totalEstimatedTime = (elapsed / progress) * 100.0f;
                int remainingSeconds = static_cast<int>((totalEstimatedTime - elapsed) / 1000.0f);
                ImGui::Text("Estimated time remaining: %d seconds", std::max(0, remainingSeconds));
            }

            static bool operationStarted = false;
            if (!operationStarted) {
                operationStarted = true;
            } else {

                if (operationInProgress) {
                    if (currentState == RESETTING) {
                        ResetBannedGuestAccountFiles(progress);
                    } else {
                        SaveGuestAccountFiles(progress);
                    }
                    operationInProgress = false;
                }

                if (progress >= 100.0f) {
                    operationStarted = false;
                    if (currentState == RESETTING) {
                        currentState = RESET_COMPLETED;
                    } else {
                        currentState = SAVE_COMPLETED;
                    }
                }
            }
        }

        ImGui::EndPopup();
    }

    if (currentState == RESET_COMPLETED) {
        ImGui::OpenPopup("ResetCompleted");
    }

    ImGui::SetNextWindowPos(
        ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f),
        ImGuiCond_Always,
        ImVec2(0.5f, 0.5f)
    );

    if (ImGui::BeginPopupModal("ResetCompleted", nullptr,
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse))
    {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "✓ Reset Completed!");
        ImGui::Spacing();
        ImGui::Text("All guest account files have been deleted.");
        ImGui::Text("Click OK to restart the application.");
        ImGui::Spacing();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(menu[0], menu[1], menu[2], 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(menu[0], menu[1], menu[2], 0.9f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(menu[0], menu[1], menu[2], 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImColor(255, 255, 255, 180).Value);
        ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255).Value);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);

        if (ImGui::Button("OK - Restart Now", ImVec2(-1, 40))) {

            kill(getpid(), SIGKILL);
            ImGui::CloseCurrentPopup();
        }

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(5);

        ImGui::EndPopup();
    }

    if (currentState == SAVE_COMPLETED) {
        ImGui::OpenPopup("SaveCompleted");
    }

    ImGui::SetNextWindowPos(
        ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f),
        ImGuiCond_Always,
        ImVec2(0.5f, 0.5f)
    );

    if (ImGui::BeginPopupModal("SaveCompleted", nullptr,
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse))
    {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "✓ Save Completed!");
        ImGui::Spacing();
        ImGui::Text("All guest account files have been saved.");
        ImGui::Text("Location: /storage/emulated/0/GuestAccountBackup/");
        ImGui::Spacing();
        ImGui::Text("Each package's files are in separate folders:");
        ImGui::BulletText("com.mobile.survive/");
        ImGui::BulletText("com.samsung.mobile/");
        ImGui::BulletText("com.pengyou.cloneapp/");
        ImGui::BulletText("user_data/");
        ImGui::BulletText("original_codm/");
        ImGui::Spacing();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(menu[0], menu[1], menu[2], 0.8f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(menu[0], menu[1], menu[2], 0.9f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(menu[0], menu[1], menu[2], 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImColor(255, 255, 255, 180).Value);
        ImGui::PushStyleColor(ImGuiCol_Text, ImColor(255, 255, 255, 255).Value);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.0f);

        if (ImGui::Button("OK", ImVec2(-1, 40))) {
            currentState = IDLE;
            ImGui::CloseCurrentPopup();
        }

        ImGui::PopStyleVar(2);
        ImGui::PopStyleColor(5);

        ImGui::EndPopup();
    }
}

static void RenderGuestAccountManagerButton(ImDrawList* draw) {
    ShowGuestAccountManager(draw);
}

#endif
