#include "ObjectGroups.hpp"


// colors: 1-green, 2-cyan, 3-pink, 4-gray, 5-darker gray, 6-red
CreateMenuItem* getCustomCreateBtn(int id, int bg, bool doRegister) {
    CreateMenuItem* btn;
    auto editor = Global::get().m_editorUI;
    if (bg <= 6) {
        btn = editor->getCreateBtn(id, bg);
    } else {
        btn = editor->getCreateBtn(id, 1);
        if (auto btnSpr = typeinfo_cast<ButtonSprite*>(btn->getChildren()->objectAtIndex(0)))
        switch (bg) {
            case 7: {btnSpr->updateBGImage("OG_button_07.png"_spr); break;}
            case 8: {btnSpr->updateBGImage("OG_button_08.png"_spr); break;}
            case 9: {btnSpr->updateBGImage("OG_button_09.png"_spr); break;}
            case 10: {btnSpr->updateBGImage("OG_button_10.png"_spr); break;}
            default: break;
        }
    }
    if (!doRegister && editor->m_createButtonArray->lastObject() == btn) {
        editor->m_createButtonArray->removeLastObject();
    }
    return btn;
}

void recursiveSetChildrenColor(CCSprite* parent, ccColor3B col) {
    if (auto children = parent->getChildren()) {
        for (unsigned i = 0; i < children->count(); i++) {
            auto child = typeinfo_cast<CCSprite*>(children->objectAtIndex(i));
            if (child) {
                child->setColor(col);
                recursiveSetChildrenColor(child, col);
            }
        }
    }
}

void setColorToCreateBtn(CreateMenuItem* cmi, ccColor3B col) {
    if (auto children = cmi->getChildren()) {
        for (int i = 0; i < children->count(); i++) {
            // recursively set color starting from ButtonSprite children
            if (auto btnSpr = typeinfo_cast<ButtonSprite*>(children->objectAtIndex(i))) {
                recursiveSetChildrenColor(btnSpr, col);
                break;
            }
        }
    }
}

void getBarSize(int* rows, int* cols) {
    *cols = GameManager::sharedState()->getIntGameVariable("0049");
    *rows = GameManager::sharedState()->getIntGameVariable("0050");
}

// lol these numbers are really hardcoded in RobTop's code
static const std::set<short> darkerButtonBgObjIds = {
    146, 147, 204, 206, 673, 674, 1340, 1340, 1341, 1342, 1343, 1344, 1345,
    144, 145, 205, 459, 
    498, 499, 500, 501, 277, 278, 719, 721, 990, 992, 1120, 1122, 1123, 1124, 1125, 1126, 1127, 1132, 1133,
    1134, 1135, 1136, 1137, 1138, 1139, 1241, 1242, 1243, 1244, 1245, 1246,
    1292, 1010, 1009, 1271, 1272, 1760, 1761, 1887, 1011, 1012, 1013, 1269, 1270, 1293, 1762, 1763, 1888,
    740, 741, 742
};

int getItemBtnColor(short objId) {
    return darkerButtonBgObjIds.contains(objId) ? 5 : 4;
}

int getGroupBtnColor() {
    return Global::get().m_settings.m_groupColor;
}