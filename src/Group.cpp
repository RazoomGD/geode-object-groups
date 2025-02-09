#include "ObjectGroups.hpp"

Group* Group::createGroup(std::string name, short objId, std::vector<std::vector<short>>&& matrix) {
    auto ret = new Group();
    if (!ret || !ret->init()) {
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
    ret->m_groupName = name;
    ret->m_objectId = objId;
    ret->m_matrix = matrix;

    ret->m_isSingle = false;
    ret->m_isUserCreated = true;

    ret->m_buttons = CCMenu::create();
    ret->addChild(ret->m_buttons);

    ret->autorelease();
    return ret;
}


Group* Group::createDefault() {
    static const short defaultObjectForTab[] = {83, 467, 1743, 8, 506, 36, 1327, 4065, 1587, 3910, 107, 1707, 899};
    short id = 3823; // :)
    
    // (feature) set thumbnail and the first object depending on the current tab
    if (auto bar = Global::get().m_editorUI->m_createButtonBar) {
        if (auto obj = static_cast<BarInfo*>(bar->getUserObject(BAR_USER_OBJ_ID))) {
            if (obj->m_tabIndx >= 0 && obj->m_tabIndx <= 12) {
                id = defaultObjectForTab[obj->m_tabIndx];
            }
        }
    }
    std::vector<std::vector<short>> matrix = {{id, 0}, {0, 0}};
    return Group::createGroup("new group", id, std::move(matrix));
}

Group* Group::createSingle(short objId, bool isUserCreated) {
    auto ret = new Group();
    if (!ret || !ret->init()) {
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
    ret->m_objectId = objId;
    ret->m_isSingle = true;
    ret->m_isUserCreated = isUserCreated;
    ret->m_buttons = nullptr;
    ret->autorelease();
    return ret;
}

CreateMenuItem* Group::getCmi() {
    if (m_isSingle) { // get classic cmi but with set userObject
        auto ret = getCustomCreateBtn(m_objectId, getItemBtnColor(m_objectId));
        ret->setUserObject(CMI_USER_OBJ_ID, this);
        return ret;
    } else { // get cmi with set userObject and custom selector
        auto ret = getCustomCreateBtn(m_objectId, getGroupBtnColor(), false);
        ret->setUserObject(CMI_USER_OBJ_ID, this);
        ret->m_pfnSelector = menu_selector(Group::onClick);
        return ret;
    }
}

void Group::onClick(CCObject*) {
    log::debug("group clicked bruh");
}

bool Group::exchangeItems(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
    if (y1 >= m_matrix.size() || y2 >= m_matrix.size()) return false;
    if (x1 >= m_matrix[0].size() || x2 >= m_matrix[0].size()) return false;
    short tmp = m_matrix[y1][x1];
    m_matrix[y1][x1] = m_matrix[y2][x2];
    m_matrix[y2][x2] = tmp;
    return true;
}

void Group::moveItem(bool right, bool down, uint8_t itemX, uint8_t itemY) {
    if (right) {
        exchangeItems(itemX, itemY, itemX+1, itemY);
    } else {
        if (itemX > 0) exchangeItems(itemX, itemY, itemX-1, itemY);
    }
    if (down) {
        exchangeItems(itemX, itemY, itemX, itemY+1);
    } else {
        if (itemY > 0) exchangeItems(itemX, itemY, itemX, itemY-1);
    }
}

void Group::addColumn(uint8_t index) {
    if (index >= m_matrix[0].size()) index = m_matrix[0].size() - 1;
    for (int i = 0; i < m_matrix.size(); i++) {
        m_matrix[i].insert(m_matrix[i].begin() + index, 0);
    }
}

void Group::addRow(uint8_t index) {
    if (index >= m_matrix.size()) index = m_matrix.size() - 1;
    int rowLen = m_matrix[0].size();
    std::vector<short> newRow(rowLen, 0);
    m_matrix.insert(m_matrix.begin() + index, newRow);
}

// utility for getting plus button
CreateMenuItem* getPlusButton() {
    auto btn = getCustomCreateBtn(1, 2, false);
    auto spr = static_cast<ButtonSprite*>(btn->getChildren()->objectAtIndex(0));
    spr->updateBGImage("OG_button_plus.png"_spr);
    spr->getChildByType<GameObject>(0)->removeFromParent();
    btn->m_objectID = 0;
    return btn;
}


void Group::updateMenu() {
    int szY = m_matrix.size();
    int szX = m_matrix[0].size();
    // todo: finish the code

}

