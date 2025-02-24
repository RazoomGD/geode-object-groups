#include "Group.hpp"
#include "EditorUI.hpp"

#include <Geode/modify/EditButtonBar.hpp>

class $modify(MyEditButtonBar, EditButtonBar) {

    $override 
    void loadFromItems(CCArray* buttonArray, int p1, int p2, bool p3) {

        auto barInfo = static_cast<BarInfo*>(this->getUserObject(BAR_USER_OBJ_ID));
        if (!barInfo || barInfo->m_isLoaded) {
            // not my tab or just reload
            return EditButtonBar::loadFromItems(buttonArray, p1, p2, p3);
        }

        // load my tab
        loadCustomBarForTab(buttonArray, barInfo->m_tabIndx, p1, p2, p3);
        log::debug("first load from items {}", barInfo->m_tabIndx);

        barInfo->m_isLoaded = true;

        // fix overlapping with arrows
        if (auto myChildren = this->getChildren())
        for (int i = 0; i < myChildren->count(); i++) {
            if (auto bsl = typeinfo_cast<BoomScrollLayer*>(myChildren->objectAtIndex(i))) {
                bsl->setZOrder(2);
                break;
            }
        }
    }


    // create bar according to object groups config
    void loadCustomBarForTab(CCArray* oldButtons, int tab, int p1, int p2, bool p3) {
        if (tab >= 20 || Global::get().m_groups[tab] == nullptr) {
            return EditButtonBar::loadFromItems(oldButtons, p1, p2, p3);
        }

        Global::get().m_editorUI->m_createButtonArray->removeObjectsInArray(oldButtons);

        std::set<short> allOldIds;
        std::vector<short> allOldIdsOrdered;
        std::set<short> clearedIds;
        for (unsigned i = 0; i < oldButtons->count(); i++) {
            auto btn = static_cast<CreateMenuItem*>(oldButtons->objectAtIndex(i));
            allOldIdsOrdered.push_back(btn->m_objectID);
            allOldIds.insert(btn->m_objectID);
        }

        auto buttons = CCArray::create();
        auto config = Global::get().m_groups[tab];

		for (auto* group : CCArrayExt<Group*>(config)) {
            if (group->isSingle() && !group->isUserCreated()) {
                if (allOldIds.contains(group->getObjId())) {
                    buttons->addObject(group->getCmi());
                    clearedIds.insert(group->getObjId());
                }
            } else {
                buttons->addObject(group->getCmi());
                if (group->isSingle()) {
                    clearedIds.insert(group->getObjId());
                } else {
                    auto matrix = group->getMatrix();
                    for (int i = 0; i < matrix.size(); i++) {
                        for (int j = 0; j < matrix[i].size(); j++) {
                            clearedIds.insert(matrix[i][j]);
                        }
                    }
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