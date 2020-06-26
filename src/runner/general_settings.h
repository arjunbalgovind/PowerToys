#pragma once

#include <common/json.h>

struct GeneralSettings
{
    bool isPackaged;
    bool isStartupEnabled;
    std::wstring startupDisabledReason;
    std::map<std::wstring, bool> isModulesEnabledMap;
    bool isElevated;
    bool isRunElevated;
    bool isAdmin;
    bool downloadUpdatesAutomatically;
    std::wstring theme;
    std::wstring systemTheme;
    std::wstring powerToysVersion;

    json::JsonObject to_json();
};

json::JsonObject load_general_settings();
GeneralSettings get_general_settings();
void apply_general_settings(const json::JsonObject& general_configs, bool save = true);
void start_initial_powertoys();
