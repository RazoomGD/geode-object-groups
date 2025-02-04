#include "ObjectGroups.hpp"




class $modify(MyEditButtonBar, EditButtonBar) {


    $override 
    void loadFromItems(CCArray* buttonArray, int p1, int p2, bool p3) {
        if (buttonArray->count() == 0) {
            return EditButtonBar::loadFromItems(buttonArray, p1, p2, p3);
        }

        int tabIndex;
        if (auto obj = this->getUserObject(BAR_USER_OBJ_ID)) {
            tabIndex = static_cast<CCInteger*>(obj)->getValue();
        } else {
            tabIndex = this->m_tabIndex;
            if (tabIndex < 0 || tabIndex >= 13) {
                return EditButtonBar::loadFromItems(buttonArray, p1, p2, p3);
            }
            // validate that this is the build tab and make a special check for the 1st tab
            auto cmi = typeinfo_cast<CreateMenuItem*>(buttonArray->objectAtIndex(0));
            if (!cmi || tabIndex == 0 && cmi->m_objectID != 1) {
                return EditButtonBar::loadFromItems(buttonArray, p1, p2, p3);
            }
            this->setUserObject(BAR_USER_OBJ_ID, CCInteger::create(tabIndex));
        }

        createCustomBarForCategory(buttonArray, tabIndex + 1, p1, p2, p3);
        log::debug("load from items {}", tabIndex);

        // fix overlapping with arrows
        if (auto myChildren = this->getChildren())
        for (int i = 0; i < myChildren->count(); i++) {
            if (auto bsl = typeinfo_cast<BoomScrollLayer*>(myChildren->objectAtIndex(i))) {
                bsl->setZOrder(2);
                break;
            }
        }
    }


    // create bar according to object groups config (category = tabIndex + 1)
    void createCustomBarForCategory(CCArray* oldButtons, short category, int p1, int p2, bool p3) {
        

        EditButtonBar::loadFromItems(oldButtons, p1, p2, p3);
    }


};