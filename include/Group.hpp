#pragma once

#include "ObjectGroups.hpp"

class Group : public CCNode {
private:
    std::string m_groupName;
    short m_objectId;
    std::vector<std::vector<short>> m_matrix;
    bool m_isSingle; // single object or group
    bool m_isInitialized; // used for lazy group load
    bool m_isInEditMode; // is menu setup for edit mode
    bool m_isUserCreated;

    CCMenu* m_menu; // there must be only buttons and nothing else
    CCScale9Sprite* m_bgSprite;
    CCLabelBMFont* m_textNode;
    CCMenu* m_topMenu;
    CCMenu* m_sideMenu;

    bool exchangeItems(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
    void updateGroupView();
    void setupControlMenus();

public:
    static Group* createGroup(std::string name, short objId, std::vector<std::vector<short>>&& matrix);
    static Group* createSingle(short objId, bool isUserCreated);
    static Group* createDefault();
    static Group* create() = delete;

    CreateMenuItem* getCmi();
    void updateMenu();
    void onClick(CCObject*);
    void onOpenGroupMenu();
    void onCloseGroupMenu();
    void onInnerCreateButton(CCObject*);
    void onPlusButton(CCObject*);
    void clearAllCreateMenuItems();

    void moveItem(bool right, bool down, uint8_t itemX, uint8_t itemY);
    void addColumn(uint8_t index);
    void addRow(uint8_t index);

    // debug
    Group() {
        log::debug("group constructed");
    }

    ~Group() {
        log::debug("group de-constructed");
    }

    // getters
    std::string getName() const {return m_groupName;}
    short getObjId() const {return m_objectId;}
    bool isUserCreated() const {return m_isUserCreated;}
    bool isSingle() const {return m_isSingle;}
    const std::vector<std::vector<short>>& getMatrix() const {return m_matrix;}
};
    