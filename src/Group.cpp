#include "Group.hpp"
#include "EditorUI.hpp"
#include "ExtraPopup.hpp"


struct GroupCoords : public CCObject {
    uint32_t m_col;
    uint32_t m_row;
    GroupCoords(uint32_t col, uint32_t row) : m_col(col), m_row(row) {
        this->autorelease();
    }
};


Group* Group::createGroup(std::string name, short objId, std::vector<std::vector<short>>&& matrix) {
    auto ret = new Group();
    if (!ret || !ret->init()) {
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
    // properties
    ret->m_groupName = name;
    ret->m_objectId = objId;
    ret->m_matrix = matrix;
    ret->m_isSingle = false;
    ret->m_isInEditMode = false;
    ret->m_isUserCreated = true;
    ret->m_isUpdateRequired = false; // lazy init
    ret->m_cmi = nullptr;

    // in case of empty matrix (it shouldn't be passed here though)
    if (ret->m_matrix.size() == 0) {
        ret->m_matrix.push_back({objId});
    }

    // fix issue when inner vectors have different sizes
    size_t maxSz = 0;
    for (int i = 0; i < ret->m_matrix.size(); i++) {
        auto sz = ret->m_matrix[i].size();
        if (sz > maxSz) maxSz = sz;
    }
    for (int i = 0; i < ret->m_matrix.size(); i++) {
        ret->m_matrix[i].resize(maxSz, 0);
    }

    // bg sprite
    ret->m_bgSprite = CCScale9Sprite::create("square02b_001.png", {0, 0, 80, 80});
    ret->addChild(ret->m_bgSprite);
    ret->m_bgSprite->setColor(ccc3(0,0,0));
    ret->m_bgSprite->setOpacity(170);
    ret->m_bgSprite->setZOrder(-10);

    // text node
    ret->m_textNode = CCLabelBMFont::create("", "goldFont.fnt");
    ret->addChild(ret->m_textNode);
    ret->m_textNode->setZOrder(-9);
    ret->m_textNode->setAnchorPoint({0.5, 0});

    // menu for buttons
    ret->m_menu = CCMenu::create();
    ret->addChild(ret->m_menu);
    ret->m_menu->setPosition({0,0});
    ret->m_menu->setContentSize({0,0});
    ret->m_menu->setTouchPriority(-502);

    // top menu
    ret->m_topMenu = CCMenu::create();
    ret->addChild(ret->m_topMenu);
    ret->m_topMenu->setTouchPriority(-502);

    // side menu
    ret->m_sideMenu = CCMenu::create();
    ret->addChild(ret->m_sideMenu);
    ret->m_sideMenu->setTouchPriority(-502);

    ret->setupControlMenus();

    ret->setID("RaZooM");
    ret->autorelease();
    return ret;
}


Group* Group::createDefault() {
    static const short defaultObjectForTab[] = {83, 467, 1743, 8, 506, 36, 1327, 4065, 1587, 3910, 107, 1707, 899};
    short id = 3823; // :)
    
    // (feature) set thumbnail and the first object depending on the current tab
    if (auto bar = Global::get().m_editorUI->m_createButtonBar)
    if (auto obj = static_cast<BarInfo*>(bar->getUserObject(BAR_USER_OBJ_ID)))
    if (obj->m_tabIndx >= 0 && obj->m_tabIndx <= 12) {
        id = defaultObjectForTab[obj->m_tabIndx];
    }

    std::vector<std::vector<short>> matrix = {{0, 0}, {0, 0}};
    return Group::createGroup("New Group", id, std::move(matrix));
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
    ret->m_menu = nullptr;
    ret->m_bgSprite = nullptr;
    ret->m_textNode = nullptr;
    ret->m_topMenu = nullptr;
    ret->m_sideMenu = nullptr;
    ret->m_cmi = nullptr;

    ret->setID("RaZooM");
    ret->autorelease();
    return ret;
}


void Group::clearAllCreateMenuItems() {
    if (m_menu != nullptr) {
        if (auto buttons = m_menu->getChildren()) {
            for (int i = 0; i < buttons->count(); i++) {
                if (auto cmi = typeinfo_cast<CreateMenuItem*>(buttons->objectAtIndex(i))) {
                    if (cmi->m_objectID != 0) {
                        Global::get().m_editorUI->m_createButtonArray->removeObject(cmi);
                    }
                }
            }
        }
    }
}


CreateMenuItem* Group::getCmi() {
    CreateMenuItem* ret;
    if (m_isSingle) { // get classic cmi but with set userObject
        ret = getCustomCreateBtn(m_objectId, getItemBtnColor(m_objectId));
    } else { // get cmi with set userObject and custom selector
        ret = getCustomCreateBtn(m_objectId, getGroupBtnColor(), false);
        ret->m_pfnSelector = menu_selector(Group::onGroupBtnClick);
    }
    ret->setUserObject(CMI_USER_OBJ_ID, this);
    setColorToCreateBtnNew(ret, true);
    m_cmi = ret;
    return ret;
}

void Group::setupControlMenus() {
    auto btn1 = CCMenuItemSpriteExtra::create(
        CCSprite::create("OG_button_move_up.png"_spr),
        this, nullptr);
    auto btn2 = CCMenuItemSpriteExtra::create(
        CCSprite::create("OG_button_move_down.png"_spr),
        this, nullptr);
    auto btn3 = CCMenuItemSpriteExtra::create(
        CCSprite::create("OG_button_move_left.png"_spr),
        this, nullptr);
    auto btn4 = CCMenuItemSpriteExtra::create(
        CCSprite::create("OG_button_move_right.png"_spr),
        this, nullptr);
    auto btn5 = CCMenuItemSpriteExtra::create(
        CCSprite::create("OG_button_small_plus.png"_spr),
        this, menu_selector(Group::onExtraButton));
    auto btn6 = CCMenuItemSpriteExtra::create(
        CCSprite::create("OG_button_trashcan.png"_spr),
        this, nullptr);
            
    m_topMenu->setAnchorPoint({0.5, 0});
    m_topMenu->addChild(btn1);
    m_topMenu->addChild(btn2);
    m_topMenu->addChild(btn3);
    m_topMenu->addChild(btn4);
    m_topMenu->setLayout(RowLayout::create());
    
    m_sideMenu->setAnchorPoint({0, 1});
    m_sideMenu->addChild(btn5);
    m_sideMenu->addChild(btn6);
    m_sideMenu->setLayout(ColumnLayout::create()->setAxisAlignment(AxisAlignment::End));
}


// selector for group button (not for single object).
// this function will call one of onOpenGroupMenu or onCloseGroupMenu
void Group::onGroupBtnClick(CCObject* sender) {
    auto cmi = static_cast<CreateMenuItem*>(sender);
    Global::get().m_editorUI->onGroupButton(cmi);
}

void Group::onOpenGroupMenu() {
    if (!m_isUpdateRequired /* lazy init */ || 
                Global::get().m_isEditMode != m_isInEditMode) {
        updateMenu();
        m_isUpdateRequired = true;
        m_isInEditMode = Global::get().m_isEditMode;
    }
}

void Group::onCloseGroupMenu() {
    // ¯\_(ツ)_/¯
}

void Group::onInnerCreateButton(CCObject* sender) {
    Global::get().m_editorUI->onCreateButton(sender);
    if (!Global::get().m_isEditMode) {
        Global::get().m_editorUI->closeOpenedGroupIfExists();
    }
}


// selector for plus button (adding new object)
void Group::onPlusButton(CCObject* sender) {
    // auto cmi = static_cast<CreateMenuItem*>(sender);
    // auto group = static_cast<Group*>(cmi->getUserObject(CMI_USER_OBJ_ID));
    // if (!group) return;

    log::debug("plus pressed");
}

void Group::onExtraButton(CCObject*) {
    ExtraOptionsPopup::create(this)->show();
}


bool Group::exchangeItems(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2) {
    if (y1 >= m_matrix.size() || y2 >= m_matrix.size()) return false;
    if (x1 >= m_matrix[0].size() || x2 >= m_matrix[0].size()) return false;
    short tmp = m_matrix[y1][x1];
    m_matrix[y1][x1] = m_matrix[y2][x2];
    m_matrix[y2][x2] = tmp;
    return true;
}


void Group::moveItem(bool right, bool down, uint32_t itemX, uint32_t itemY) {
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


void Group::addColumn(uint32_t index) {
    if (index >= m_matrix[0].size()) index = m_matrix[0].size() - 1;
    for (int i = 0; i < m_matrix.size(); i++) {
        m_matrix[i].insert(m_matrix[i].begin() + index, 0);
    }
}


void Group::addRow(uint32_t index) {
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
    btn->m_pfnSelector = menu_selector(Group::onPlusButton);
    // btn->setEnabled(false); <--- set it for filler buttons
    return btn;
}


void Group::updateObjId(short newObjId) {
    if (!m_cmi) return;
    m_objectId = newObjId;
    auto otherCmi = getCustomCreateBtn(newObjId, getGroupBtnColor(), false);
    bool selected = false; // preserve selected
    if (auto spr = m_cmi->getChildByType<ButtonSprite>(0)) {
        if (spr->m_subBGSprite) {
            auto col = spr->m_subBGSprite->getColor(); // button bg sprite
            selected = (col == ccc3(127, 127, 127));
        }
    }
    m_cmi->setNormalImage(otherCmi->getNormalImage());
    m_cmi->updateSprite();
    if (selected) {
        setColorToCreateBtnNew(m_cmi, false);
    }
    // otherCmi will be autoreleased
}


bool Group::getSelectedItemPos(uint32_t* x, uint32_t* y) {
    auto cmi = Global::get().m_editorUI->getSelectedCmi();
    if (cmi == nullptr) {
        return false;
    }
    auto myButtons = m_menu->getChildren();
    for (int i = 0; i < myButtons->count(); i++) {
        auto btn = myButtons->objectAtIndex(i);
        if (btn == cmi) {
            if (auto pos = static_cast<GroupCoords*>(cmi->getUserObject())) {
                *x = pos->m_col;
                *y = pos->m_row;
                return true;
            } else {
                return false;
            }
        }
    }
    return false;
}


void Group::updateMenu() {
    int szY = m_matrix.size();
    if (szY == 0) return;
    int szX = m_matrix[0].size();

    auto oldButtons = m_menu->getChildren();
    if (oldButtons == nullptr) {
        oldButtons = CCArray::create();
    } else {
        // copy the array so that it won't be cleared in removeAllChildren()
        auto tmp = CCArray::create();
        tmp->addObjectsFromArray(oldButtons);
        oldButtons = tmp;
    }

    m_menu->removeAllChildren(); // remove old buttons
        
    // prepare new buttons

    const bool isEditMode = Global::get().m_isEditMode;

    for (uint32_t i = 0; i < szY; i++) {
        for (uint32_t j = 0; j < szX; j++) {
            const short id = m_matrix[i][j];

            if (id == 0 && !isEditMode) {
                continue; // we need plus buttons only in edit mode
            }

            // reuse old buttons when possible
            bool found = false;
            for (int k = 0; k < oldButtons->count(); k++) {
                auto btn = static_cast<CreateMenuItem*>(oldButtons->objectAtIndex(k));
                if (btn->m_objectID == id) {
                    m_menu->addChild(btn);
                    btn->setUserObject(new GroupCoords(i, j));
                    oldButtons->fastRemoveObjectAtIndex(k); // important to break immediately
                    found = true;
                    break;
                }
            }

            if (!found) {
                CreateMenuItem *btn;
                if (id == 0) {
                    btn = getPlusButton();
                } else {
                    btn = getCustomCreateBtn(id, getItemBtnColor(id));
                    btn->m_pfnSelector = menu_selector(Group::onInnerCreateButton);
                    // setColorToCreateBtnNew(btn, true);
                }
                btn->setUserObject(new GroupCoords(i, j));
                m_menu->addChild(btn);
            }
        }
    }

    // get rid of the buttons that we don't need anymore
    auto registeredButtons = Global::get().m_editorUI->m_createButtonArray;
    for (int k = 0; k < oldButtons->count(); k++) {
        auto btn = static_cast<CreateMenuItem*>(oldButtons->objectAtIndex(k));
        if (btn->m_objectID != 0) {
            registeredButtons->removeObject(btn);
        }
    }

    updateGroupView();

}


// monster function
void Group::updateGroupView() {

    auto buttonArray = m_menu->getChildren();
    if (!buttonArray) {
        buttonArray = CCArray::create();
    }
    
    const float oneDistance = 45; // distance between two button centers
    const float shiftY = 57.5;

    const int rowCount = m_matrix.size();
    const int columnCount = m_matrix[0].size();
    const float centerShiftX = (columnCount - 1) * oneDistance * 0.5f;

    const float top = shiftY + (float)(rowCount - 1) * oneDistance;
    const float left = -centerShiftX;
    const float bottom = shiftY;
    const float right = (float)(columnCount - 1) * oneDistance - centerShiftX;

    for (int i = 0; i < buttonArray->count(); i++) {
        auto btn = static_cast<CreateMenuItem*>(buttonArray->objectAtIndex(i));
        auto uObj = static_cast<GroupCoords*>(btn->getUserObject());
        float x = left + (float)uObj->m_col * oneDistance;
        float y = top - (float)uObj->m_row * oneDistance;
        btn->setPosition(ccp(x, y));
    }

    // update name text
    float spaceOnTop = 0;
    if (Global::get().m_settings.m_showNames && !m_groupName.empty() && m_textNode) {
        m_textNode->setString(m_groupName.c_str());
        float availableSpace = right - left + oneDistance;
        float takenSpace = m_textNode->getContentWidth();
        m_textNode->setScale(std::min(1.1f, availableSpace / takenSpace));
        spaceOnTop = m_textNode->getScaledContentHeight();
        m_textNode->setPosition({0, top + oneDistance * 0.63f});
    } else if (m_textNode) {
        m_textNode->setString("");
    }
    
    // update bg
    if (m_bgSprite) {
        const float scaleFactor = 2; // for CCScale9Sprite not to be destroyed
        const float border = 1.4f * oneDistance;
        m_bgSprite->setContentSize({(right - left + border) * scaleFactor, 
            (top + spaceOnTop - bottom + border) * scaleFactor});
        m_bgSprite->setPosition({0, (top + spaceOnTop + bottom) / 2});
        m_bgSprite->setScale(1 / scaleFactor);
    }

    // update control menus
    if (m_topMenu && m_sideMenu) {
        if (Global::get().m_isEditMode) {
            m_topMenu->setVisible(true);
            m_topMenu->setPosition({0, top + spaceOnTop + oneDistance * 0.8f});
            m_topMenu->setContentWidth(right - left + oneDistance * 2);
            m_topMenu->updateLayout();
            
            m_sideMenu->setVisible(true);
            m_sideMenu->setPosition({right + oneDistance * 0.8f, 
                top + spaceOnTop + oneDistance / 2});
            m_sideMenu->setContentHeight(top + spaceOnTop - bottom + oneDistance);
            m_sideMenu->setScale(1.2);
            m_sideMenu->updateLayout();
    
        } else {
            m_topMenu->setVisible(false);
            m_sideMenu->setVisible(false);
        }
    }
}

