#include "Group.hpp"


class $modify(MyEditButtonBar, EditButtonBar) {

    $override 
    void loadFromItems(CCArray* buttonArray, int p1, int p2, bool p3) {
        if (buttonArray->count() == 0) {
            return EditButtonBar::loadFromItems(buttonArray, p1, p2, p3);
        }

        int tabIndex;
        if (auto obj = static_cast<BarInfo*>(this->getUserObject(BAR_USER_OBJ_ID))) {
            if (obj->m_isLoaded) {
                // tab already loaded
                return EditButtonBar::loadFromItems(buttonArray, p1, p2, p3);
            }
            obj->m_isLoaded = true; // will be loaded right now
            tabIndex = obj->m_tabIndx;
        } else {
            // user object is not set means this is either not my tab or tab is not loaded

            // check if this is my tab
            if (m_tabIndex < 0 || m_tabIndex >= 13) {
                return EditButtonBar::loadFromItems(buttonArray, p1, p2, p3);
            }

            // validate that this is the build tab and make a special check for the 1st tab
            auto cmi = typeinfo_cast<CreateMenuItem*>(buttonArray->objectAtIndex(0));
            if (!cmi || m_tabIndex == 0 && (cmi->m_objectID != 1 || buttonArray->count() < 400)) {
                return EditButtonBar::loadFromItems(buttonArray, p1, p2, p3);
            }

            // tab is mine and will be loaded right now
            this->setUserObject(BAR_USER_OBJ_ID, new BarInfo(m_tabIndex, true));
            tabIndex = m_tabIndex;
        }

        // load my tab
        loadCustomBarForTab(buttonArray, tabIndex, p1, p2, p3);
        log::debug("first load from items {}", tabIndex);

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
        

        EditButtonBar::loadFromItems(oldButtons, p1, p2, p3);
    }


};