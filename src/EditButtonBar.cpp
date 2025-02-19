#include "Group.hpp"


class $modify(MyEditButtonBar, EditButtonBar) {

    $override 
    void loadFromItems(CCArray* buttonArray, int p1, int p2, bool p3) {
        if (!buttonArray || buttonArray->count() == 0) {
            return EditButtonBar::loadFromItems(buttonArray, p1, p2, p3);
        }

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
        

        EditButtonBar::loadFromItems(oldButtons, p1, p2, p3);
    }


};