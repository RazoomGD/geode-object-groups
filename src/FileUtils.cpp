#include "Group.hpp"
#include "EditorUI.hpp"

#include <iostream>
#include <fstream>

using namespace matjson;

$on_mod(Loaded) {
    // make sure config file exists
    auto path = Mod::get()->getConfigDir(true).append("OGv2_config.json");
    if (!std::filesystem::exists(path)) {
        auto pathDefault = Mod::get()->getResourcesDir().append("OGv2_config_default.json");
        log::info("User config file ({}) not found!", path.string());
        if (!std::filesystem::exists(pathDefault)) {
            // no way to get config
            log::error("DEFAULT CONFIG FILE NOT FOUND! YOU SHOULD NOT HAVE DELETED IT!");
            log::error("PATH: {}", pathDefault.string());
        } else {
            std::ifstream src(pathDefault, std::ios::binary);
            std::ofstream dst(path, std::ios::binary);
            dst << src.rdbuf();
            log::info("Config file updated from the default config");
        }
    }
}

// write config to json file. Return 0 on success, -1 on file error
int writeConfigToJson(std::string filename) {
    // open file
    std::ofstream jsonFile(filename, std::ios::out | std::ios::trunc);

    if (!jsonFile) return -1;

    Value config;
    // foreach bar with my user object
    for (auto* bar : CCArrayExt<EditButtonBar*>(EditorUI::get()->m_createButtonBars)) {
    	if (auto barInfo = static_cast<BarInfo*>(bar->getUserObject(BAR_USER_OBJ_ID))) {
            auto jsonArray = Global::get().m_editorUI->barToJsonValue(bar);
            auto tabIdx = fmt::format("tab_{}", barInfo->m_tabIndx);
            config[tabIdx] = jsonArray;
        }
    }
    
    Value jsonObj = makeObject({
        {"comment", "DANGER! Don't edit this manually!"},
        {"config", config}
    });

    // write file
    jsonFile << jsonObj.dump() << std::endl;
    jsonFile.close();
    
    return 0;
}


// load config from json file to global buffers (Global->m_groups)
// return 0 on success, -1 on file error, -2 on json error
int readConfigFromJson(std::string filename) {
    std::ifstream jsonFile(filename);
    if (!jsonFile) {
        return -1;
    }
    auto res = matjson::parse(jsonFile);
    if (!res.isOk()) {
        return -2;
    }

    Value jsonObj = res.unwrap();
    Value config = jsonObj["config"];
    if (!config.isObject()) {
        return -2;
    }

    for (int i = 0; i < Global::get().m_groups.size(); i++) { // foreach tab
        Value tab = config[fmt::format("tab_{}", i)];
        if (tab.isArray()) {
            auto tabArray = CCArray::create();
            for (Value& groupObj : tab) { // foreach group/obj entry
                if (auto group = Group::createFromJsonValue(groupObj)) {
                    tabArray->addObject(group);
                }
            }
            Global::get().m_groups[i] = tabArray;
        } else {
            Global::get().m_groups[i] = nullptr;
        }
    }

    return 0;
}