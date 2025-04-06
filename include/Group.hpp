#pragma once

#include "ObjectGroups.hpp"

#include <matjson.hpp>
#include <matjson/std.hpp>

class GroupDragLayer;

class Group : public CCNode {
private:
    std::string m_groupName;
    std::array<short, 4> m_objectIds = {0};
    bool m_isUserCreated;
    std::vector<std::vector<short>> m_matrix;
    bool m_isSingle; // single object or group
    bool m_isUpdateRequired;
    bool m_isInEditMode; // is menu setup for edit mode

    CCMenu* m_menu = nullptr; // there must be only buttons and nothing else
    CCMenuItemToggler* m_pinBtn = nullptr;
    GroupDragLayer* m_dragLayer = nullptr;
    CCScale9Sprite* m_bgSprite = nullptr;
    CCLabelBMFont* m_textNode = nullptr;
    CCMenu* m_topMenu = nullptr;
    CCMenu* m_rightMenu = nullptr;
    CCMenu* m_leftMenu = nullptr;
    CreateMenuItem* m_cmi = nullptr;

    void setupControlMenus();
    
    void updateGroupView();

    bool exchangeItems(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);

    CreateMenuItem* getPlusButton();

    // button handlers
    void onGroupBtnClick(CCObject*);

    void onInnerCreateButton(CCObject*);
    void onInnerPlusButton(CCObject*);

    void onExtraButton(CCObject*);
    void onPinButton(CCObject*);
    void onDeleteObjButton(CCObject*);
    void onAddObjectButton(CCObject*);
    void onArrowButton(CCObject*);

public:
    // create method variants
    static Group* createGroup(std::string name, std::array<short,4> objIds, std::vector<std::vector<short>>&& matrix);
    static Group* createSingle(short objId, bool isUserCreated);
    static Group* createDefault();
    static Group* createFromArray(std::string name, std::array<short,4> objIds, std::vector<short>&& array);
    static Group* createFromJsonValue(matjson::Value json, bool validateIds=false); // always check for null!
    static Group* create() = delete;

    // the most important methods here
    CreateMenuItem* getCmi();
    void updateMenu(bool preserveSelectedCmi=true);
    bool getSelectedItemPosition(uint32_t* col, uint32_t* row);
    bool setSelectedCmiWithPosition(uint32_t col, uint32_t row);
    matjson::Value toJson();

    void clearAllCreateMenuItems(); // from editorUI button array

    void addColumn(uint32_t index);
    void addRow(uint32_t index);
    void addObjects(std::vector<short> ids);
    void deleteColumn(uint32_t index);
    void deleteRow(uint32_t index);
    void updateObjId(std::array<short,4> newObjIds);
    void updateName(std::string name, CreateMenuItem* cmi);
    bool tryDeleteButtonByValue(CreateMenuItem* cmi);

    // getters, setters
    std::string getName() const {return m_groupName;}
    bool isSingle() const {return m_isSingle;}
    bool isPinned() const {return m_pinBtn ? m_pinBtn->m_toggled : false;}
    bool isUserCreated() const {return m_isUserCreated;}
    void setUserCreated(bool val) {m_isUserCreated = val;}
    const std::array<short,4>& getObjIds() const {return m_objectIds;}
    const std::vector<std::vector<short>>& getMatrix() const {return m_matrix;}
    void setUpdateRequired(bool required) {m_isUpdateRequired = required;}
    CCMenuItemToggler* getPinBtn() const {return m_pinBtn;}
};
    