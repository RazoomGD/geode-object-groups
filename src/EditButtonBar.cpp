#include "Group.hpp"
#include "EditorUI.hpp"

#include <Geode/modify/EditButtonBar.hpp>

class $modify(MyEditButtonBar, EditButtonBar) {

    $override 
    void loadFromItems(CCArray* buttonArray, int p1, int p2, bool p3) {

        auto barInfo = static_cast<BarInfo*>(this->getUserObject(BAR_USER_OBJ_ID));
        if (!barInfo || barInfo->m_isLoaded) {
            // not my tab or just reload
            EditButtonBar::loadFromItems(buttonArray, p1, p2, p3);

        } else {
            // load my tab
            loadCustomBarForTab(buttonArray, barInfo->m_tabIndx, p1, p2, p3);
            barInfo->m_isLoaded = true;
        }

        // fix arrows overlapping
        if (m_scrollLayer) m_scrollLayer->setZOrder(2);
    }


    // create bar according to object groups config
    void loadCustomBarForTab(CCArray* oldButtons, int tab, int p1, int p2, bool p3) {
        if (tab >= Global::get().m_groups.size() || Global::get().m_groups[tab] == nullptr) {
            return EditButtonBar::loadFromItems(oldButtons, p1, p2, p3);
        }

        Global::editor()->m_createButtonArray->removeObjectsInArray(oldButtons);

        std::vector<short> allOldIdsOrdered;
        for (auto* btn : CCArrayExt<CreateMenuItem*>(oldButtons)) {
            allOldIdsOrdered.push_back(btn->m_objectID);
        }

        std::set<short> allOldIds(allOldIdsOrdered.begin(), allOldIdsOrdered.end());
        std::set<short> clearedIds;

        auto buttons = CCArray::create();
        auto config = Global::get().m_groups[tab];

        // fill group-cleared ids
        for (auto* group : CCArrayExt<Group*>(config)) {
            if (!group->isSingle()) {
                auto matrix = group->getMatrix();
                for (int i = 0; i < matrix.size(); i++) {
                    for (int j = 0; j < matrix[i].size(); j++) {
                        clearedIds.insert(matrix[i][j]);
                    }
                }
            } else if (group->isUserCreated()) {
                clearedIds.insert(group->getObjId());
            }
        }

        // create new array obj buttons
		for (auto* group : CCArrayExt<Group*>(config)) {
            if (!group->isSingle() || group->isUserCreated()) {
                buttons->addObject(group->getCmi());
            } else {
                short id = group->getObjId();
                if (allOldIds.contains(id) && !clearedIds.contains(id)) {
                    buttons->addObject(group->getCmi());
                    clearedIds.insert(id);
                }
            }
		}

        // add objects, that were not used in any of the groups
        for (auto objId : allOldIdsOrdered) {
            if (clearedIds.contains(objId)) continue;
            auto btn = getCustomCreateBtn(objId, getItemBtnColor(objId));
            buttons->addObject(btn);
        }

        EditButtonBar::loadFromItems(buttons, p1, p2, p3);

    }
};