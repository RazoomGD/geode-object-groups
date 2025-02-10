#include "ObjectGroups.hpp"

#include <matjson.hpp>
#include <matjson/std.hpp>

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


bool writeConfigToJson(std::string filename) {
    Value config;
    for (int i = 0; i < Global::get().m_groups.size(); i++) { // foreach tab
        auto tabArray = Global::get().m_groups[i].data();
        if (tabArray == nullptr || tabArray->count() == 0) {
            continue;
        }

        Value jsonArray(std::vector<int>{});
        for (int i = 0; i < tabArray->count(); i++) { // foreach item
            auto group = static_cast<Group*>(tabArray->objectAtIndex(i));
            Value jsonGroup;
            jsonGroup.set("obj", group->getObjId());
            if (!group->isSingle()) { // group
                jsonGroup.set("group", group->getMatrix());

                auto name = group->getName();
                if (!name.empty()) {
                    jsonGroup.set("name", name);
                }
            } else { // single object
                if (group->isUserCreated()) {
                    jsonGroup.set("isUsr", true);
                }
            }
            jsonArray.push(jsonGroup);
        }

        auto tabIdx = std::to_string(i);
        config[tabIdx] = 1;
    }
    Value jsonObj = makeObject({
        {"comment", "DANGER! Don't edit this manually!"},
        {"config", config}
    });

    std::ofstream jsonFile(filename, std::ios::out | std::ios::trunc);
    if (!jsonFile) {
        return false;
    }
    jsonFile << jsonObj.dump() << std::endl;
    return true;
}

bool readConfigFromJson(std::string filename) {
    std::ifstream jsonFile(filename);
    if (!jsonFile) {
        return false;
    }
    auto res = matjson::parse(jsonFile);
    if (res.isErr()) {
        return false;
    }

    Value jsonObj = res.unwrap();
    Value config = jsonObj["config"];
    if (!config.isObject()) {
        return false;
    }

    for (int i = 0; i < Global::get().m_groups.size(); i++) { // foreach tab
        auto tabArray = Global::get().m_groups[i].data();
        if (tabArray == nullptr) {
            return false;
        }
        if (tabArray->count()) {
            tabArray->removeAllObjects();
        }

        auto key = std::to_string(i);
        Value tab = config[key];
        if (!tab.isArray()) {
            continue;
        }

        for (Value& groupObj : tab) { // foreach group/obj entry
            Value obj = groupObj["obj"];
            if (!obj.isExactlyUInt()) {
                continue;
            }

            std::string name;
            Value aName = groupObj["name"];
            if (aName.isString()) {
                name = aName.asString().unwrap();
            }

            std::vector<std::vector<short>> matrix;
            Value aMatrix = groupObj["group"];
            if (aMatrix.isArray()) {
                for (Value& row : aMatrix) {
                    if (row.isArray()) {
                        std::vector<short> vec;
                        for (Value& el : row) {
                            if (el.isExactlyUInt()) {
                                vec.push_back(el.asInt().unwrap());
                            }
                        }
                        matrix.push_back(vec);
                    }
                }
            }
            Group* group;
            if (matrix.empty()) {
                Value isUserObj = groupObj["isUsr"];
                group = Group::createSingle(obj.asInt().unwrap(), isUserObj.asBool().unwrapOr(false));
            } else {
                group = Group::createGroup(name, obj.asInt().unwrap(), std::move(matrix));
            }
            Global::get().m_groups[i].data()->addObject(group);
        }
    }
    return true;
}