#include "Group.hpp"
#include "EditorUI.hpp"

#include <iostream>
#include <fstream>

#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>

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

    Value config;
    std::set<uint8_t> savedTabs;
    std::set<short> custom;
    
    // foreach bar with my user object
    auto allTabs = alpha::editor_tabs::getAllTabs();
    for (auto node : *allTabs) {
    	if (auto barInfo = tryGetBarInfo(node)) {
            auto jsonArray = Global::editor()->barToJsonValue(static_cast<EditButtonBar*>(node), custom);
            auto tabIdx = fmt::format("tab_{}", barInfo->m_tabIndx);
            config[tabIdx] = jsonArray;
            savedTabs.insert(barInfo->m_tabIndx);
        }
    }

    // foreach tab that might be not loaded
    auto &allGroups = Global::editor()->m_fields->GROUPS;
    for (int i = 0; i < allGroups.size(); i++) {
        if (savedTabs.contains(i)) continue;
        if (auto array = allGroups[i]) {
            std::vector<Value> groupsVec;
            for (auto* group : CCArrayExt<Group*>(array)) {
                groupsVec.push_back(group->toJson(custom));
            }
            config[fmt::format("tab_{}", i)] = Value(groupsVec);
        }
    }
    
    Value jsonObj = makeObject({
        {"comment", "DANGER! Don't edit this manually unless you know what you're doing!"},
        {"config", config},
        {"custom", Global::editor()->getCustomObjects(custom)}
    });

    // write file
    std::ofstream jsonFile(filename, std::ios::out | std::ios::trunc);
    if (!jsonFile) return -1;
    
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

    Value customObj = jsonObj["custom"];
    if (customObj.isObject()) {
        auto &customObjConfig = Global::editor()->m_fields->myCustomObjects;
        for (auto& [cuObjId, cuObjStr] : customObj) {
            if (cuObjStr.isString()) {
                customObjConfig.insert({cuObjId, cuObjStr.asString().unwrap()});
            }
        }
    }

    Value config = jsonObj["config"];
    if (!config.isObject()) {
        return -2;
    }

    auto &allGroups = Global::editor()->m_fields->GROUPS;
    for (int i = 0; i < allGroups.size(); i++) { // foreach tab
        Value tab = config[fmt::format("tab_{}", i)];
        if (tab.isArray()) {
            auto tabArray = CCArray::create();
            for (Value& groupObj : tab) { // foreach group/obj entry
                if (auto group = Group::createFromJsonValue(groupObj, true)) {
                    tabArray->addObject(group);
                }
            }
            allGroups[i] = tabArray;
        } else {
            allGroups[i] = nullptr;
        }
    }

    return 0;
}

static std::string writeFormatted(matjson::Value const &groupsArray, std::set<short> const &customIds) {
    auto custom = Global::editor()->getCustomObjects(customIds);
    if (custom.empty()) {
        return matjson::makeObject({
            {"config", groupsArray}
        }).dump();
    } else {
        return matjson::makeObject({
            {"config", groupsArray},
            {"custom", custom}
        }).dump();
    }
}

std::string copyGroupAsJson(CreateMenuItem* cmi) {
    std::set<short> custom;
    if (auto group = Group::get(cmi)) {
        if (!group->isSingle()) {
            auto json = matjson::Value(std::vector<matjson::Value>{group->toJson(custom)});
            return writeFormatted(json, custom);
        }
    }
    if (cmi->m_objectID < 0) custom.insert(cmi->m_objectID);
    auto json = matjson::Value(std::vector<matjson::Value>{matjson::makeObject({{"obj", cmi->m_objectID}})});
    return writeFormatted(json, custom);
}

std::string copyTabAsJson(EditButtonBar* bar) {
    auto editor = Global::editor();
    std::set<short> custom;
    auto json = editor->barToJsonValue(bar, custom);
    return writeFormatted(json, custom);
}

void pasteGroupsFromJsonToCurrentTab(std::string data) {
    auto parsed = matjson::parse(data);

    if (parsed.isErr()) {
        alert("Couldn't paste!\n<cr>INCORRECT JSON FORMAT</c>");
        return;
    }

    if (!Global::editor()->getCurrentTabIfAllowed()) {
        alert("You can't create a button in this tab");
        return;
    }

    auto json = *parsed;

    std::map<int, std::string> customAll;
    auto custom = json["custom"];
    if (custom.isObject()) {
        for (auto& [key, value] : custom) {
            customAll.insert({std::atoi(key.c_str()), value.asString().unwrapOr("")});
        }
    }

    auto newGroups = CCArray::create();
    int groupCount = 0;

    auto fgroups = json["config"];
    if (fgroups.isArray()) {
        for (auto& val : fgroups) {
            if (auto group = Group::createFromJsonValue(val, true)) {
                newGroups->addObject(group);
                group->setUserCreated(true);
                if (!group->isSingle()) groupCount++;
            }
        }
    }

    int total = newGroups->count();
    if (total == 0) {
        alert("<cj>Nothing was found :(</c>\nCheck that there are no mistakes in json. "
            "(You can copy other groups as json to see the expected json format)");
        return;
    }

    createQuickPopup("Object Groups", 
        fmt::format("Are you sure you want to paste <cy>{}</c> buttons\n"
                    "(<cy>{}</c> groups and <cy>{}</c> objects)?", 
                    total, groupCount, total - groupCount),
        "No", "Yes", 
        [newGroups = Ref(newGroups), customAll] (auto, bool isBtn2) {
            if (isBtn2) {
                auto buttons = CCArray::create();
                for (auto *group : CCArrayExt<Group*>(newGroups)) {
                    group->remapCustomObjects(customAll);
                    buttons->addObject(group->getCmi());
                }
                if (!buttons->count()) return;
                Global::editor()->addButtonsAndReloadButtonBar(buttons);
                Global::editor()->goToPageWithCmi(static_cast<CreateMenuItem*>(buttons->lastObject()), true);
                Global::get().m_hasUnsavedOGChanges = true;
                shortAlert(fmt::format("Pasted {} buttons!", buttons->count()).c_str());
            } else {
                shortAlert("Nothing happened!");
            }
        }
    );  
}