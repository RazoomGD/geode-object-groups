#include "EditorUI.hpp"
#include <Geode/modify/GameManager.hpp>

class $modify(GameManager) {
    gd::string stringForCustomObject(int customObjectID) {
        if (customObjectID <= CUSTOM_OBJECT_ID_OFFSET) {
            if (auto editor = Global::editor()) {
                auto &cuObjs = editor->m_fields->myCustomObjects;
                auto iter = cuObjs.find(std::to_string(customObjectID));
                if (iter != cuObjs.end()) {
                    auto str = iter->second;
                    if (!str.empty()) return str;
                }
            }
        }
        return GameManager::stringForCustomObject(customObjectID);
    }
};
