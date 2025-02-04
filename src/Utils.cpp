#include "ObjectGroups.hpp"


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
