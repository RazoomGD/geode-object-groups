#pragma once

#include "ObjectGroups.hpp"

class Group : public CCNode {
private:
    std::string m_groupName;
    short m_objectId;
    std::vector<std::vector<short>> m_matrix;
    bool m_isSingle; // single object or group
    bool m_isUpdateRequired;
    bool m_isInEditMode; // is menu setup for edit mode
    bool m_isUserCreated;

    CCMenu* m_menu; // there must be only buttons and nothing else
    CCScale9Sprite* m_bgSprite;
    CCLabelBMFont* m_textNode;
    CCMenu* m_topMenu;
    CCMenu* m_sideMenu;

    void setupControlMenus();
    
    void updateGroupView();
    
    bool exchangeItems(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);
    void moveItem(bool right, bool down, uint32_t itemX, uint32_t itemY);
    void addColumn(uint32_t index);
    void addRow(uint32_t index);

public:
    // create method variants
    static Group* createGroup(std::string name, short objId, std::vector<std::vector<short>>&& matrix);
    static Group* createSingle(short objId, bool isUserCreated);
    static Group* createDefault();
    static Group* create() = delete;

    // the most important methods here
    CreateMenuItem* getCmi();
    void updateMenu();

    // button handlers
    void onGroupBtnClick(CCObject*);
    void onOpenGroupMenu();
    void onCloseGroupMenu();

    void onInnerCreateButton(CCObject*);
    void onPlusButton(CCObject*);

    void onExtraButton(CCObject*);

    void clearAllCreateMenuItems();

    // debug
    Group() {
        log::debug("group constructed");
    }

    ~Group() {
        log::debug("group de-constructed");
    }

    // getters, setters
    std::string getName() const {return m_groupName;}
    short getObjId() const {return m_objectId;}
    bool isUserCreated() const {return m_isUserCreated;}
    bool isSingle() const {return m_isSingle;}
    const std::vector<std::vector<short>>& getMatrix() const {return m_matrix;}
    void setName(std::string name) {m_groupName = name;}
};
    