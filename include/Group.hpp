#pragma once

#include "ObjectGroups.hpp"

#include <matjson.hpp>
#include <matjson/std.hpp>

class Group : public CCNode {
private:
    std::string m_groupName;
    short m_objectId;
    std::vector<std::vector<short>> m_matrix;
    bool m_isSingle; // single object or group
    bool m_isUpdateRequired;
    bool m_isInEditMode; // is menu setup for edit mode
    bool m_isUserCreated;

    CCMenu* m_menu = nullptr; // there must be only buttons and nothing else
    CCScale9Sprite* m_bgSprite = nullptr;
    CCLabelBMFont* m_textNode = nullptr;
    CCMenu* m_topMenu = nullptr;
    CCMenu* m_sideMenu = nullptr;
    CreateMenuItem* m_cmi = nullptr;

    void setupControlMenus();
    
    void updateGroupView();

    bool exchangeItems(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2);

public:
    // create method variants
    static Group* createGroup(std::string name, short objId, std::vector<std::vector<short>>&& matrix);
    static Group* createSingle(short objId, bool isUserCreated);
    static Group* createDefault();
    static Group* createFromJsonValue(matjson::Value json); // always check for null!
    static Group* create() = delete;

    // the most important methods here
    CreateMenuItem* getCmi();
    void updateMenu(bool preserveSelectedCmi=true);
    bool getSelectedItemPosition(uint32_t* col, uint32_t* row);
    bool setSelectedCmiWithPosition(uint32_t col, uint32_t row);
    matjson::Value toJson();

    // button handlers
    void onGroupBtnClick(CCObject*);
    void onOpenGroupMenu();
    void onCloseGroupMenu();

    void onInnerCreateButton(CCObject*);
    void onPlusButton(CCObject*);

    void onExtraButton(CCObject*);
    void onDeleteObjButton(CCObject*);
    void onArrowButton(CCObject*);

    void clearAllCreateMenuItems();

    void updateObjId(short newObjId);
    void addColumn(uint32_t index);
    void addRow(uint32_t index);
    void deleteColumn(uint32_t index);
    void deleteRow(uint32_t index);

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
    