#pragma once

#include <iomanip>
#include <sstream>
#include <ctime>
#include <cstdio>

#include <fstream>
#include <sys/stat.h>

#include "../../foxcheats/include/json.hpp"

std::string androidFilesDir = "/storage/emulated/0/Android/data/com.garena.game.codm/files/";

constexpr int kConfigVersion = 2;

namespace ethcfg
{

    inline bool GetBool(const nlohmann::json& j, const char* key, bool dflt)
    {
        const auto it = j.find(key);
        return (it != j.end() && it->is_boolean()) ? it->get<bool>() : dflt;
    }
    inline float GetFloat(const nlohmann::json& j, const char* key, float dflt)
    {
        const auto it = j.find(key);
        if (it == j.end()) return dflt;
        if (it->is_number_float()) return it->get<float>();
        if (it->is_number_integer()) return (float)it->get<int>();
        return dflt;
    }
    inline int GetInt(const nlohmann::json& j, const char* key, int dflt)
    {
        const auto it = j.find(key);
        return (it != j.end() && it->is_number_integer()) ? it->get<int>() : dflt;
    }
}

void SaveConfiguration(const std::string& filename) {
    nlohmann::json config;

    config["version"] = kConfigVersion;

    config["ESPMenu"]["Box"] = Config.ESPMenu.Box;
    config["ESPMenu"]["Name"] = Config.ESPMenu.Name;
    config["ESPMenu"]["Health"] = Config.ESPMenu.Health;
    config["ESPMenu"]["Distance"] = Config.ESPMenu.Distance;
    config["ESPMenu"]["Target"] = static_cast<int>(Config.ESPMenu.Target);
    config["ESPMenu"]["BoxType"] = static_cast<int>(Config.ESPMenu.BoxType);
    config["ESPMenu"]["HealthPosition"] = static_cast<int>(Config.ESPMenu.HealthPosition);
    config["ESPMenu"]["CrosshairType"] = static_cast<int>(Config.ESPMenu.CrosshairType);
    config["ESPMenu"]["EspStyle"] = static_cast<int>(Config.ESPMenu.EspStyle);

    config["Aim"]["Aimbot360"] = Config.Aim.Aimbot360;
    config["Aim"]["AimSilent"] = Config.Aim.AimSilent;
    config["Aim"]["AimAssistSize"] = Config.Aim.AimAssistSize;
    config["Aim"]["Target"] = static_cast<int>(Config.Aim.Target);
    config["Aim"]["Trigger"] = static_cast<int>(Config.Aim.Trigger);
    config["Aim"]["By"] = static_cast<int>(Config.Aim.By);
    config["Aim"]["size"] = Config.Aim.size;

    config["ExtraMenu"]["Flash"] = Config.ExtraMenu.Flash;
    config["ExtraMenu"]["Diving"] = Config.ExtraMenu.Diving;
    config["ExtraMenu"]["Fire"] = Config.ExtraMenu.Fire;
    config["ExtraMenu"]["Hit"] = Config.ExtraMenu.Hit;
    config["ExtraMenu"]["Rpd"] = Config.ExtraMenu.Rpd;
    config["ExtraMenu"]["Parachute"] = Config.ExtraMenu.Parachute;
    config["ExtraMenu"]["Recoil"] = Config.ExtraMenu.Recoil;
    config["ExtraMenu"]["Shake"] = Config.ExtraMenu.Shake;
    config["ExtraMenu"]["Spread"] = Config.ExtraMenu.Spread;
    config["ExtraMenu"]["Reload"] = Config.ExtraMenu.Reload;
    config["ExtraMenu"]["Scope"] = Config.ExtraMenu.Scope;
    config["ExtraMenu"]["Switch"] = Config.ExtraMenu.Switch;
    config["ExtraMenu"]["KineticArmor"] = Config.ExtraMenu.Kinetic;

    config["isJumpAdjustmentEnabled"] = isJumpAdjustmentEnabled;
    config["jumpHeightMultiplier"] = jumpHeightMultiplier;
    config["SnowB"] = SnowB;
    config["SnowBsize"] = SnowBsize;
    config["isSpeedHackEnabled"] = isSpeedHackEnabled;
    config["speedHackMultiplier"] = speedHackMultiplier;

    config["menu"][0] = menu[0];
    config["menu"][1] = menu[1];
    config["menu"][2] = menu[2];

    std::string configDir = androidFilesDir + "configs";
    mkdir(configDir.c_str(), 0777);

    const std::string filePath = configDir + "/" + filename + ".json";
    const std::string tmpPath = filePath + ".tmp";
    {
        std::ofstream file(tmpPath, std::ios::trunc);
        if (!file.is_open()) {
            return;
        }
        file << std::setw(4) << config;
        file.flush();
        if (!file.good()) {
            file.close();
            std::remove(tmpPath.c_str());
            return;
        }
        file.close();
    }
    std::remove(filePath.c_str());
    if (std::rename(tmpPath.c_str(), filePath.c_str()) != 0) {
        std::remove(tmpPath.c_str());
    }
}

bool LoadConfiguration(const std::string& filename) {
    try {
        const std::string filePath = androidFilesDir + "configs/" + filename + ".json";
        std::ifstream file(filePath);
        if (!file.is_open()) {
            return false;
        }

        nlohmann::json config;
        file >> config;

        if (config.contains("ESPMenu")) {
            const nlohmann::json& e = config["ESPMenu"];
            Config.ESPMenu.Box = ethcfg::GetBool(e, "Box", Config.ESPMenu.Box);
            Config.ESPMenu.Name = ethcfg::GetBool(e, "Name", Config.ESPMenu.Name);
            Config.ESPMenu.Health = ethcfg::GetBool(e, "Health", Config.ESPMenu.Health);
            Config.ESPMenu.Distance = ethcfg::GetBool(e, "Distance", Config.ESPMenu.Distance);
            Config.ESPMenu.Target = static_cast<LineTarget>(ethcfg::GetInt(e, "Target", (int)Config.ESPMenu.Target));
            Config.ESPMenu.BoxType = static_cast<EspBoxType>(ethcfg::GetInt(e, "BoxType", (int)Config.ESPMenu.BoxType));
            Config.ESPMenu.HealthPosition = static_cast<EspHealthPosition>(ethcfg::GetInt(e, "HealthPosition", (int)Config.ESPMenu.HealthPosition));
            Config.ESPMenu.CrosshairType = static_cast<CrosshairTarget>(ethcfg::GetInt(e, "CrosshairType", (int)Config.ESPMenu.CrosshairType));
            Config.ESPMenu.EspStyle = static_cast<EspStyleTarget>(ethcfg::GetInt(e, "EspStyle", (int)Config.ESPMenu.EspStyle));
        }

        if (config.contains("Aim")) {
            const nlohmann::json& a = config["Aim"];
            Config.Aim.Aimbot360 = ethcfg::GetBool(a, "Aimbot360", Config.Aim.Aimbot360);
            Config.Aim.AimSilent = ethcfg::GetBool(a, "AimSilent", Config.Aim.AimSilent);
            Config.Aim.AimAssistSize = ethcfg::GetFloat(a, "AimAssistSize", Config.Aim.AimAssistSize);
            Config.Aim.Target = static_cast<EAimTarget>(ethcfg::GetInt(a, "Target", (int)Config.Aim.Target));
            Config.Aim.Trigger = static_cast<EAimTrigger>(ethcfg::GetInt(a, "Trigger", (int)Config.Aim.Trigger));
            Config.Aim.By = static_cast<EAim>(ethcfg::GetInt(a, "By", (int)Config.Aim.By));
            Config.Aim.size = ethcfg::GetFloat(a, "size", Config.Aim.size);
        }

        if (config.contains("ExtraMenu")) {
            const nlohmann::json& x = config["ExtraMenu"];
            Config.ExtraMenu.Flash = ethcfg::GetBool(x, "Flash", Config.ExtraMenu.Flash);
            Config.ExtraMenu.Diving = ethcfg::GetBool(x, "Diving", Config.ExtraMenu.Diving);
            Config.ExtraMenu.Fire = ethcfg::GetBool(x, "Fire", Config.ExtraMenu.Fire);
            Config.ExtraMenu.Hit = ethcfg::GetBool(x, "Hit", Config.ExtraMenu.Hit);
            Config.ExtraMenu.Rpd = ethcfg::GetBool(x, "Rpd", Config.ExtraMenu.Rpd);
            Config.ExtraMenu.Parachute = ethcfg::GetBool(x, "Parachute", Config.ExtraMenu.Parachute);
            Config.ExtraMenu.Recoil = ethcfg::GetBool(x, "Recoil", Config.ExtraMenu.Recoil);
            Config.ExtraMenu.Shake = ethcfg::GetBool(x, "Shake", Config.ExtraMenu.Shake);
            Config.ExtraMenu.Spread = ethcfg::GetBool(x, "Spread", Config.ExtraMenu.Spread);
            Config.ExtraMenu.Reload = ethcfg::GetBool(x, "Reload", Config.ExtraMenu.Reload);
            Config.ExtraMenu.Scope = ethcfg::GetBool(x, "Scope", Config.ExtraMenu.Scope);
            Config.ExtraMenu.Switch = ethcfg::GetBool(x, "Switch", Config.ExtraMenu.Switch);
            Config.ExtraMenu.Kinetic = ethcfg::GetBool(x, "KineticArmor", Config.ExtraMenu.Kinetic);
        }

        isJumpAdjustmentEnabled = ethcfg::GetBool(config, "isJumpAdjustmentEnabled", isJumpAdjustmentEnabled);
        jumpHeightMultiplier = ethcfg::GetFloat(config, "jumpHeightMultiplier", jumpHeightMultiplier);
        SnowB = ethcfg::GetBool(config, "SnowB", SnowB);
        SnowBsize = ethcfg::GetFloat(config, "SnowBsize", SnowBsize);
        isSpeedHackEnabled = ethcfg::GetBool(config, "isSpeedHackEnabled", isSpeedHackEnabled);
        speedHackMultiplier = ethcfg::GetFloat(config, "speedHackMultiplier", speedHackMultiplier);

        if (config.contains("menu") && config["menu"].is_array() && config["menu"].size() >= 3) {
            const nlohmann::json& m = config["menu"];
            if (m[0].is_number()) menu[0] = m[0].get<float>();
            if (m[1].is_number()) menu[1] = m[1].get<float>();
            if (m[2].is_number()) menu[2] = m[2].get<float>();
        }

        return true;
    }
    catch (const std::exception&) {
        return false;
    }
}
