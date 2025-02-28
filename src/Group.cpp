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
    ret->m_isUpdateRequired = true; // lazy init

    // in case of empty matrix (it shouldn't be passed here though)
    if (ret->m_matrix.size() == 0) {
        ret->m_matrix.push_back({objId});
    }

    // fix issue when inner vectors have different sizes
    size_t maxSz = 1;
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
    auto bgCol = Global::get().m_settings.m_groupBgColor;
    ret->m_bgSprite->setColor(ccc3(bgCol.r, bgCol.g, bgCol.b));
    ret->m_bgSprite->setOpacity(bgCol.a);
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

    // ret->setupControlMenus(); <-- lazy setup

    ret->setID("RaZooM");
    ret->autorelease();
    return ret;
}

// (feature) set thumbnail object depending on the current tab
short getIdForOpenedTab() {
    static const short defaultObjectForTab[] = 
        {83, 467, 1743, 8, 506, 36, 1327, 4065, 1587, 3910, 107, 1707, 899};
    short id = 3823; // :)
    if (auto bar = Global::get().m_editorUI->m_createButtonBar)
    if (auto obj = static_cast<BarInfo*>(bar->getUserObject(BAR_USER_OBJ_ID)))
    if (obj->m_tabIndx >= 0 && obj->m_tabIndx <= 12) {
        id = defaultObjectForTab[obj->m_tabIndx];
    }
    return id;
}


Group* Group::createDefault() {
    short id = getIdForOpenedTab();
    std::vector<std::vector<short>> matrix = {{0, 0}, {0, 0}};
    return Group::createGroup("New Group", id, std::move(matrix));
}


Group* Group::createFromArray(std::string name, short objId, std::vector<short>&& array) {
    int total = array.size();
    if (total == 0) return Group::createDefault();
    
    int rowCount = (total < 5) ? total : ((total < 7 || total == 9) ? 3 : 4);
    int columnCount = ceil(total / (float)rowCount);
    
    std::vector<std::vector<short>> matrix;
    for (int objIter = 0; objIter < total;) {
        std::vector<short> newRow;
        for (int j = 0; j < columnCount; j++) {
            newRow.push_back(array[objIter++]);
            if (objIter == total) break;
        }
        matrix.push_back(newRow);
    }
    return Group::createGroup(name, objId, std::move(matrix));
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

    ret->setID("RaZooM");
    ret->autorelease();
    return ret;
}


Group* Group::createFromJsonValue(matjson::Value json, bool validateIds) {
    matjson::Value obj = json["obj"];
    if (!obj.isExactlyUInt()) return nullptr;

    int objectId = obj.asInt().unwrap();

    if (validateIds && !isObjIdExistsFast(objectId)) {
        objectId = getIdForOpenedTab();
    }

    std::string name;
    matjson::Value aName = json["name"];
    if (aName.isString()) {
        name = aName.asString().unwrap();
    }

    std::vector<std::vector<short>> matrix;
    matjson::Value aMatrix = json["group"];
    if (aMatrix.isArray()) {
        for (matjson::Value& row : aMatrix) {
            if (row.isArray()) {
                std::vector<short> vec;
                for (matjson::Value& el : row) {
                    if (el.isExactlyUInt()) {
                        vec.push_back(el.asInt().unwrap());
                    }
                }
                matrix.push_back(vec);
            }
        }
    }

    Group* group;
    if (matrix.empty()) {
        matjson::Value isUserObj = json["isUsr"];
        group = Group::createSingle(objectId, isUserObj.asBool().unwrapOr(false));
    } else {
        if (validateIds) {
            for (auto& row : matrix) {
                for (auto& val : row) {
                    if (!isObjIdExistsFast(val)) {
                        val = 0;
                    }
                }
            }
        }
        group = Group::createGroup(name, objectId, std::move(matrix));
    }
    return group;
}


matjson::Value Group::toJson() {
    matjson::Value jsonGroup;
    jsonGroup.set("obj", m_objectId);
    if (!m_isSingle) { // group
        jsonGroup.set("group", m_matrix);

        if (!m_groupName.empty()) {
            jsonGroup.set("name", m_groupName);
        }
    } else { // single object
        if (m_isUserCreated) {
            jsonGroup.set("isUsr", m_isUserCreated);
        }
    }
    return jsonGroup;
}


std::string Group::toString(CCPoint bottomLeft, CCPoint* topRight) {
    if (isSingle()) {
        *topRight = bottomLeft;
        return fmt::format("1,{},2,{},3,{};", m_objectId, bottomLeft.x, bottomLeft.y);
    } else {
        std::string ret;
        *topRight = bottomLeft + ccp((m_matrix[0].size() - 1) * 60, (m_matrix.size() - 1) * 60);
        CCPoint pos = bottomLeft;
        for (int i = m_matrix.size() - 1; i >= 0; i--) {
            for (int j = 0; j < m_matrix[0].size(); j++) {
                if (m_matrix[i][j] > 0) 
                    ret.append(fmt::format("1,{},2,{},3,{};", m_matrix[i][j], pos.x, pos.y));
                pos.x += 60;
            }
            pos.y += 60;
            pos.x = bottomLeft.x;
        }
        return ret;
    }
}


void Group::clearAllCreateMenuItems() {
    if (m_menu != nullptr) {
        if (auto buttons = m_menu->getChildren()) {
            for (int i = 0; i < buttons->count(); i++) {
                auto cmi = static_cast<CreateMenuItem*>(buttons->objectAtIndex(i));
                if (cmi->m_objectID != 0) {
                    Global::get().m_editorUI->m_createButtonArray->fastRemoveObject(cmi);
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
        ret->m_pListener = this;
        ret->m_objectID = 0;
        ret->setTag(0); // compat with creative mode
        ret->setUserObject(new GroupInfo()); // compat with creative mode x2
    }
    ret->setUserObject(CMI_USER_OBJ_ID, this);
    setColorToCreateBtnNew(ret, true);
    m_cmi = ret;
    return ret;
}


void Group::setupControlMenus() {
    auto btn1 = CCMenuItemSpriteExtra::create(
        CCSprite::create("OG_button_move_up.png"_spr),
        this, menu_selector(Group::onArrowButton));
    auto btn2 = CCMenuItemSpriteExtra::create(
        CCSprite::create("OG_button_move_down.png"_spr),
        this, menu_selector(Group::onArrowButton));
    auto btn3 = CCMenuItemSpriteExtra::create(
        CCSprite::create("OG_button_move_left.png"_spr),
        this, menu_selector(Group::onArrowButton));
    auto btn4 = CCMenuItemSpriteExtra::create(
        CCSprite::create("OG_button_move_right.png"_spr),
        this, menu_selector(Group::onArrowButton));
    auto btn5 = CCMenuItemSpriteExtra::create(
        CCSprite::create("OG_button_newobject.png"_spr),
        this, menu_selector(Group::onAddObjectButton));
    auto btn6 = CCMenuItemSpriteExtra::create(
        CCSprite::create("OG_button_trashcan.png"_spr),
        this, menu_selector(Group::onDeleteObjButton));
    auto btn7 = CCMenuItemSpriteExtra::create(
        CCSprite::create("OG_button_small_plus.png"_spr),
        this, menu_selector(Group::onExtraButton));
    
    m_topMenu = CCMenu::create();
    this->addChild(m_topMenu);
    m_topMenu->setTouchPriority(-502);

    m_topMenu->setAnchorPoint({0.5, 0});
    m_topMenu->addChild(btn1);
    m_topMenu->addChild(btn2);
    m_topMenu->addChild(btn3);
    m_topMenu->addChild(btn4);
    m_topMenu->setLayout(RowLayout::create());

    btn1->setTag(1);
    btn2->setTag(2);
    btn3->setTag(3);
    btn4->setTag(4);
    
    m_rightMenu = CCMenu::create();
    this->addChild(m_rightMenu);
    m_rightMenu->setTouchPriority(-502);
    m_rightMenu->setScale(0.9);

    m_rightMenu->setAnchorPoint({0, 1});
    m_rightMenu->addChild(btn6);
    m_rightMenu->addChild(btn5);
    m_rightMenu->setLayout(ColumnLayout::create()->setAxisAlignment(AxisAlignment::End));

    m_leftMenu = CCMenu::create();
    this->addChild(m_leftMenu);
    m_leftMenu->setTouchPriority(-502);
    m_leftMenu->setScale(0.9);

    m_leftMenu->setAnchorPoint({1, 1});
    m_leftMenu->addChild(btn7);
    m_leftMenu->setLayout(ColumnLayout::create()->setAxisAlignment(AxisAlignment::End));
}


void Group::onArrowButton(CCObject* sender) {
    const int tag = sender->getTag(); // 1,2,3,4 - up, down, left, right
    if (tag > 4 || tag < 1) return;
    uint32_t col, row;
    if (!getSelectedItemPosition(&col, &row)) {
        // alert("No focused button within the group. Select the button first!");
        return;
    }
    if (auto cmi = Global::get().m_editorUI->getFocusedCmi()) {
        if (cmi->m_objectID == 0) return; // don't move plus buttons
    }

    uint32_t newCol = col, newRow = row;
    switch (tag) {
        case 1: {newRow -= 1; break;}
        case 2: {newRow += 1; break;}
        case 3: {newCol -= 1; break;}
        case 4: {newCol += 1; break;}
    }
    if (!exchangeItems(col, row, newCol, newRow)) return;
    updateMenu(false);
    Global::get().m_hasUnsavedOGChanges = true;
    setSelectedCmiWithPosition(newCol, newRow);
}


void Group::onDeleteObjButton(CCObject*) {
    uint32_t btnX, btnY;
    if (!getSelectedItemPosition(&btnX, &btnY)) {
        alert("No focused button within the group. <cj>Select the object you want to remove from group!</c>");
        return;
    }
    if (m_matrix[btnY][btnX] == 0) return;
    m_matrix[btnY][btnX] = 0;
    Global::get().m_editorUI->setNewFocusedCmi(nullptr);
    updateMenu(false);
    Global::get().m_hasUnsavedOGChanges = true;
}


void Group::onAddObjectButton(CCObject* sender) {
    auto selected = Global::get().m_editorUI->getSelectedObjects();

    if (selected->count() == 0) {
        alert("To add new object to the group you must select at least <cy>1</c> object in editor");
        return;
    }

    std::vector<short> ids = getUniqueIds(selected);

    uint32_t colSt, rowSt;
    if (!getSelectedItemPosition(&colSt, &rowSt)) {
        colSt = rowSt = 0;
    }

    auto iter = ids.begin();
    uint32_t col = colSt, row = rowSt;
    const uint32_t rowEnd = m_matrix.size();
    const uint32_t colEnd = m_matrix[0].size();
    do {
        if (m_matrix[row][col] == 0) {
            m_matrix[row][col] = *iter;
            if (++iter == ids.end()) break;
        }
        if (++col == colEnd) {
            col = 0;
            if (++row == rowEnd) row = 0;
        }
    } while (col != colSt || row != rowSt); // full cycle
    
    if (iter != ids.begin()) { // at least 1 added
        updateMenu();
        Global::get().m_hasUnsavedOGChanges = true;
    }

    if (iter != ids.end()) {
        alert("<cy>Group is full!</c>");
    }
}


// selector for group button (not for single object).
// this function will call one of onOpenGroupMenu or onCloseGroupMenu
void Group::onGroupBtnClick(CCObject* sender) {
    auto cmi = static_cast<CreateMenuItem*>(sender);
    auto editor = Global::get().m_editorUI;

    if (this == editor->getOpenedGroup()) {
        // close
        editor->setNewOpenedGroup(nullptr, nullptr);
    } else {
        // open
        editor->setNewOpenedGroup(this, cmi);

        if (m_isUpdateRequired || Global::get().m_isEditMode != m_isInEditMode) {
            updateMenu();
            m_isUpdateRequired = false;
            m_isInEditMode = Global::get().m_isEditMode;
        }
    }
    editor->setNewFocusedCmi(cmi);

}


void Group::onInnerCreateButton(CCObject* sender) {
    auto btn = static_cast<CreateMenuItem*>(sender);
    auto editor = Global::get().m_editorUI;

    EditorUI::get()->onCreateButton(btn); // avoid double call bug

    // if (!Global::get().m_isEditMode) {
    //     editor->setNewOpenedGroup(nullptr, nullptr);
    // }

    bool thisIsNowSelected = (btn->m_objectID == editor->m_selectedObjectIndex);
    if (thisIsNowSelected) {
        editor->setNewSelectedGroupCmi(m_cmi);

    } else {
        editor->setNewSelectedGroupCmi(nullptr);
    }
}


// selector for plus button
void Group::onInnerPlusButton(CCObject* sender) {
    auto cmi = static_cast<CreateMenuItem*>(sender);
    Global::get().m_editorUI->setNewFocusedCmi(cmi);
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


void Group::addColumn(uint32_t index) {
    if (index > m_matrix[0].size()) index = m_matrix[0].size();
    for (int i = 0; i < m_matrix.size(); i++) {
        m_matrix[i].insert(m_matrix[i].begin() + index, 0);
    }
}


void Group::addRow(uint32_t index) {
    if (index > m_matrix.size()) index = m_matrix.size();
    int rowLen = m_matrix[0].size();
    std::vector<short> newRow(rowLen, 0);
    m_matrix.insert(m_matrix.begin() + index, newRow);
}


void Group::deleteColumn(uint32_t index) {
    if (index >= m_matrix[0].size()) index = m_matrix[0].size() - 1;
    if (index == 0 && m_matrix[0].size() == 1) return;
    for (int i = 0; i < m_matrix.size(); i++) {
        m_matrix[i].erase(m_matrix[i].begin() + index);
    }
}


void Group::deleteRow(uint32_t index) {
    if (index >= m_matrix.size()) index = m_matrix.size() - 1;
    if (index == 0 && m_matrix.size() == 1) return;
    m_matrix.erase(m_matrix.begin() + index);
}


// utility for getting plus button
CreateMenuItem* Group::getPlusButton() {
    auto btnSpr = ButtonSprite::create(
        CCSprite::create(), 32, 0, 32, 1, true, "OG_button_plus.png"_spr, true);
    auto cmi = CreateMenuItem::create(btnSpr, nullptr, this, menu_selector(Group::onInnerPlusButton));
    cmi->m_scaleMultiplier = 1;
    cmi->m_objectID = 0;
    return cmi;
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


bool Group::getSelectedItemPosition(uint32_t* col, uint32_t* row) {
    auto cmi = Global::get().m_editorUI->getFocusedCmi();
    if (cmi == nullptr) return false;
    auto myButtons = m_menu->getChildren();
    if (myButtons == nullptr) return false;
    for (int i = 0; i < myButtons->count(); i++) {
        auto btn = myButtons->objectAtIndex(i);
        if (btn == cmi) {
            if (auto pos = static_cast<GroupCoords*>(cmi->getUserObject(COORDS_USER_OBJ_ID))) {
                *col = pos->m_col;
                *row = pos->m_row;
                return true;
            } else {
                return false;
            }
        }
    }
    return false;
}


bool Group::setSelectedCmiWithPosition(uint32_t col, uint32_t row) {
    auto buttons = m_menu->getChildren();
    if (buttons == nullptr) return false;
    for (int i = 0; i < buttons->count(); i++) {
        auto btn = static_cast<CreateMenuItem*>(buttons->objectAtIndex(i));
        if (auto uObj = static_cast<GroupCoords*>(btn->getUserObject(COORDS_USER_OBJ_ID))) {
            if (uObj->m_row == row && uObj->m_col == col) {
                Global::get().m_editorUI->setNewFocusedCmi(btn);
                return true;
            }
        }
    }
    return false;
}


void Group::updateMenu(bool preserveSelectedCmi) {
    const int szY = m_matrix.size();
    if (szY == 0) return;
    const int szX = m_matrix[0].size();
    const bool isEditMode = Global::get().m_isEditMode;

    if (isEditMode && (!m_topMenu || !m_rightMenu || !m_leftMenu)) {
        setupControlMenus(); // lazy setup
    }

    uint32_t focusedRow, focusedColumn; // try to preserve it
    bool hasSelectedCmi = preserveSelectedCmi ? 
                getSelectedItemPosition(&focusedColumn, &focusedRow) : false;

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
                    btn->setUserObject(COORDS_USER_OBJ_ID, new GroupCoords(j, i));
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
                    btn->m_pListener = this;
                    bool isSelected = (Global::get().m_editorUI->m_selectedObjectIndex == id);
                    setColorToCreateBtnNew(btn, !isSelected);
                }
                m_menu->addChild(btn);
                btn->setUserObject(COORDS_USER_OBJ_ID, new GroupCoords(j, i));
            }
        }
    }

    // get rid of the buttons that we don't need anymore
    auto registeredButtons = Global::get().m_editorUI->m_createButtonArray;
    for (int k = 0; k < oldButtons->count(); k++) {
        auto btn = static_cast<CreateMenuItem*>(oldButtons->objectAtIndex(k));
        if (btn->m_objectID != 0) {
            registeredButtons->fastRemoveObject(btn);
        }
    }

    updateGroupView();

    // preserve selected cmi
    if (hasSelectedCmi) {
        setSelectedCmiWithPosition(focusedColumn, focusedRow);
    }

}


// monster function (don't call it directly, it must be only called from updateMenu())
void Group::updateGroupView() {

    auto buttonArray = m_menu->getChildren();
    if (!buttonArray) {
        buttonArray = CCArray::create();
    }
    
    const float oneDistance = 45; // distance between two button centers
    const float shiftY = 57.5;
    const float border = 0.4f * oneDistance;

    const int rowCount = m_matrix.size();
    const int columnCount = m_matrix[0].size();
    const float centerShiftX = (columnCount - 1) * oneDistance * 0.5f;

    const float top = shiftY + (float)(rowCount - 1) * oneDistance;
    const float left = -centerShiftX;
    const float bottom = shiftY;
    const float right = (float)(columnCount - 1) * oneDistance - centerShiftX;

    for (int i = 0; i < buttonArray->count(); i++) {
        auto btn = static_cast<CreateMenuItem*>(buttonArray->objectAtIndex(i));
        auto uObj = static_cast<GroupCoords*>(btn->getUserObject(COORDS_USER_OBJ_ID));
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
        m_bgSprite->setContentSize({(right - left + oneDistance + border) * scaleFactor, 
            (top + spaceOnTop - bottom + oneDistance + border) * scaleFactor});
        m_bgSprite->setPosition({0, (top + spaceOnTop + bottom) / 2});
        m_bgSprite->setScale(1 / scaleFactor);
        
        auto bgCol = Global::get().m_settings.m_groupBgColor;
        m_bgSprite->setColor(ccc3(bgCol.r, bgCol.g, bgCol.b));
        m_bgSprite->setOpacity(bgCol.a);
    }

    // update control menus
    if (m_topMenu && m_rightMenu && m_leftMenu) {
        if (Global::get().m_isEditMode) {
            m_topMenu->setVisible(true);
            m_topMenu->setPosition({0, top + spaceOnTop + oneDistance * 0.8f});
            m_topMenu->setContentWidth(right - left + oneDistance * 2);
            m_topMenu->updateLayout();
            
            m_rightMenu->setVisible(true);
            m_rightMenu->setPosition({right + oneDistance * 0.8f, 
                top + spaceOnTop + oneDistance / 2});
            m_rightMenu->setContentHeight(top + spaceOnTop - bottom + oneDistance + border);
            m_rightMenu->updateLayout();

            m_leftMenu->setVisible(true);
            m_leftMenu->setPosition({left - oneDistance * 0.8f, 
                top + spaceOnTop + oneDistance / 2});
            m_leftMenu->setContentHeight(top + spaceOnTop - bottom + oneDistance * 2);
            m_leftMenu->updateLayout();
        } else {
            m_topMenu->setVisible(false);
            m_rightMenu->setVisible(false);
            m_leftMenu->setVisible(false);
        }
    }
}

