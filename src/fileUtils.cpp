#include "config.hpp"

#include <json.hpp>

bool writeConfigToJson(std::string filename) {
    auto jsonConfig = json::Array();
    for (auto& category : *getCONFIG()) {
        auto jsonCategory = json::Object();
        jsonCategory["build_tab_index"] = category.first;

        auto jsonGroups = json::Array();
        for (auto& group : category.second) {
            auto jsonGroup = json::Object();
            jsonGroup["group_name"] = group.m_groupName;
            jsonGroup["thumbnail_object_id"] = group.m_thumbnailObjectId;
            if (group.m_properties & GROUP_HIDE) {
                jsonGroup["hide_group"] = true;
            }
            if (group.m_properties & GROUP_UNGROUP) {
                jsonGroup["ungroup"] = true;
            }
            
            auto objectIds = json::Array();
            for (auto id : group.m_objectIds) {
                objectIds.append(id);
            }
            jsonGroup["object_ids"] = objectIds;
            jsonGroups.append(jsonGroup);
        }
        jsonCategory["object_groups"] = jsonGroups;
        jsonConfig.append(jsonCategory);
    }

    auto jsonObj = json::Object();
    jsonObj["config"] = jsonConfig;

    std::ofstream jsonFile(filename, std::ios::out | std::ios::trunc);
    if (jsonFile.is_open()) {
        jsonFile << jsonObj << std::endl;
        jsonFile.close();
        return true;
    } else {
        return false;
    }
}

bool readConfigFromJson(std::string filename, std::vector<std::string> * errList) {
    auto oldErrListSize = errList->size();

    getCONFIG()->clear();

    std::ifstream jsonFile(filename);
    if (!jsonFile.is_open()) {
        errList->push_back(fmt::format("Error: Unable to open '{}' file", filename));
        return false;
    }

    std::string tmpString;
    std::string jsonContent = "";
    while (std::getline(jsonFile, tmpString)) {
        jsonContent += tmpString;
    }
    jsonFile.close();

    auto jsonObj = json::JSON::Load(jsonContent, errList);
    if (oldErrListSize != errList->size()) {
        errList->push_back("Error: Unable to parse json file!");
        return false;
    }
    
    auto jsonConfig = jsonObj["config"];
    if (jsonConfig.IsNull()) { errList->push_back("The required 'config' key was not found"); return false;}

    for (auto& tab : jsonConfig.ArrayRange()) {
        if (tab.IsNull()) { 
            errList->push_back("Error loading one of the build tabs"); 
            continue;
        }
        auto tabIndex = tab["build_tab_index"];
        if (tabIndex.IsNull()) { 
            errList->push_back("The required 'build_tab_index' key was not found"); 
            continue;
        }
        int tabIndexInt = tabIndex.ToInt();
        const int tabIndexMin = 1, tabIndexMax = 13;
        if (tabIndexInt > tabIndexMax || tabIndexInt < tabIndexMin) { 
            errList->push_back(fmt::format("Tab index {} is out of bounds ({} <= index <= {})", tabIndexInt, tabIndexMin, tabIndexMax)); 
            continue;
        }
        auto groups = tab["object_groups"];
        if (groups.IsNull()) { 
            errList->push_back(fmt::format("The required 'object_groups' key was not found in #{} build tab", tabIndexInt)); 
            continue;
        }
        int iter = 0;
        std::vector<Group> vecGroups;
        for (auto& group : groups.ArrayRange()) {
            iter++;
            if (group.IsNull()) { 
                errList->push_back(fmt::format("Error loading the #{} group in the #{} build tab", iter, tabIndexInt));
                continue;
            }
            auto groupName = group["group_name"];
            if (groupName.IsNull()) { 
                errList->push_back(fmt::format("The required 'group_name' key was not found for the #{} group in the #{} build tab", iter, tabIndexInt));
                continue;
            }
            auto groupNameStr = groupName.ToString();
            auto objectIds = group["object_ids"];
            if (objectIds.IsNull()) { 
                errList->push_back(fmt::format("The required 'object_ids' key was not found for the '{}' group in the #{} build tab", groupNameStr, tabIndexInt));
                continue;
            }
            auto thumbnailObjId = group["thumbnail_object_id"];
            if (thumbnailObjId.IsNull()) { 
                errList->push_back(fmt::format("The required 'thumbnail_object_id' key was not found for the '{}' group in the #{} build tab", groupNameStr, tabIndexInt));
                continue;
            }
            auto thumbnailObjIdInt = thumbnailObjId.ToInt();
            if (thumbnailObjIdInt <= 0) { 
                errList->push_back(fmt::format("Illegal 'thumbnail_object_id' for the '{}' group in the #{} build tab", groupNameStr, tabIndexInt));
                continue;
            }
            short groupProps = 0;
            auto hideGroup = group["hide_group"];
            if (!hideGroup.IsNull() && hideGroup.ToBool()) {
                groupProps |= GROUP_HIDE;
            }
            auto ungroupGroup = group["ungroup"];
            if (!ungroupGroup.IsNull() && ungroupGroup.ToBool()) {
                groupProps |= GROUP_UNGROUP;
            }
            bool allObjectsLoaded = true;
            std::vector<short> vecObjectIds;
            for (auto& objId : objectIds.ArrayRange()) {
                if (objId.IsNull() || objId.ToInt() <= 0) { 
                    allObjectsLoaded = false;
                    continue;
                }
                auto objIdInt = objId.ToInt();
                vecObjectIds.push_back((short) objIdInt);
            }
            if (!allObjectsLoaded) {
                errList->push_back(fmt::format("Some objects appear to have illegal ids in the '{}' group in the #{} build tab", groupNameStr, tabIndexInt));
            }
            Group aGroup = {groupNameStr, (short) thumbnailObjIdInt, vecObjectIds, groupProps};
            vecGroups.push_back(aGroup);
        }
        getCONFIG()->insert({ (short) tabIndexInt, vecGroups});
    }
    return errList->size() == oldErrListSize;
}
