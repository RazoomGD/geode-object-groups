#pragma once

#include "ObjectGroups.hpp"

#include <matjson.hpp>
#include <matjson/std.hpp>

class GroupDragLayer;

// static int groupCountAAA = 0;

enum class GroupState {CLOSED = 0, OPENED, HOVERED, PINNED};

class Group : public CCNode {
private:
    std::string m_groupName;
    std::array<int, 4> m_objectIds = {0};
    bool m_isUserCreated; // false - means that it was added as missing object (append deleted option)
    std::vector<std::vector<int>> m_matrix;
    bool m_isSingle; // single object or group
    bool m_isUpdateRequired;
    bool m_isInEditMode; // is menu setup for edit mode
    uint16_t m_groupUID; // id that is kept between editor re-entries
    GroupState m_groupState = GroupState::CLOSED;

    CCMenu* m_menu = nullptr; // there must be only buttons and nothing else
    CCMenuItemToggler* m_pinBtn = nullptr;
    GroupDragLayer* m_dragLayer = nullptr;
    CCScale9Sprite* m_bgSprite = nullptr;
    CCLabelBMFont* m_textNode = nullptr;
    CCMenu* m_topMenu = nullptr;
    CCMenu* m_pinMenu = nullptr;
    CCMenu* m_rightMenu = nullptr;
    CCMenu* m_leftMenu = nullptr;
    CreateMenuItem* m_cmi = nullptr;

    // todo: delete this
    // Group() {log::debug("group+ {}", ++groupCountAAA);}
    // ~Group() {log::debug("group- {}", --groupCountAAA);}

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
    static Group* createGroup(std::string name, std::array<int,4> objIds, std::vector<std::vector<int>>&& matrix);
    static Group* createSingle(int objId, bool isUserCreated);
    static Group* createDefault();
    static Group* createFromArray(std::string name, std::array<int,4> objIds, std::vector<int>&& array);
    static Group* createFromJsonValue(matjson::Value json, bool validateIds=false); // always check for null!
    static Group* create() = delete;

    static Group* get(CreateMenuItem* cmi);

    // the most important methods here
    CreateMenuItem* getCmi();
    void updateMenu(bool preserveSelectedCmi=true);
    bool containsButton(CreateMenuItem* cmi);
    bool getSelectedItemPosition(uint32_t* col, uint32_t* row);
    bool setSelectedCmiWithPosition(uint32_t col, uint32_t row);
    CreateMenuItem* getCmiByPosition(uint32_t col, uint32_t row);
    matjson::Value toJson(std::set<int> &custom);

    void remapCustomObjects(std::map<int, std::string> const &customObjects);

    void clearAllCreateMenuItems(); // from editorUI button array

    void changeGroupState(GroupState state);

    void addColumn(uint32_t index);
    void addRow(uint32_t index);
    void addObjects(std::vector<int> ids, bool setFocused=false);
    void deleteColumn(uint32_t index);
    void deleteRow(uint32_t index);
    void updateObjId(std::array<int,4> newObjIds);
    void updateName(std::string name, CreateMenuItem* cmi);

    // getters, setters
    std::string getName() const {return m_groupName;}
    bool isSingle() const {return m_isSingle;}
    GroupState getState() const {return m_groupState;}
    bool isUserCreated() const {return m_isUserCreated;}
    void setUserCreated(bool val) {m_isUserCreated = val;}
    const std::array<int,4>& getObjIds() const {return m_objectIds;}
    const std::vector<std::vector<int>>& getMatrix() const {return m_matrix;}
    void setUpdateRequired(bool required) {m_isUpdateRequired = required;}
    uint16_t getGroupUID() const {return m_groupUID;}
    void setGroupUID(uint16_t groupUID) {m_groupUID = groupUID;}

};
    