#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/EditButtonBar.hpp>
#include <Geode/modify/BoomScrollLayer.hpp>

#include <alphalaneous.editortab_api/include/EditorTabs.hpp>

#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>

using namespace geode::prelude;



// for marking EditButtonBars
#define BAR_USER_OBJ_ID "OG-bar"
#define CMI_USER_OBJ_ID "OG-cmi"



// Mod global state (singleton)
struct Global {
    static Global& get() {
        static Global instance;
        return instance;
    }

    EditorUI* m_editorUI;

    // Group Config (index in array is a build tab index)
    std::array<Ref<CCArray>, 20> m_groups; 

    struct {
        uint8_t m_extraTabsCount;
        int m_groupColor;
        void update() {
            m_extraTabsCount = Mod::get()->getSettingValue<int64_t>("extra-tabs-count");
            int col = std::atoi(Mod::get()->getSettingValue<std::string>("group-button-color-v2").c_str());
            m_groupColor = (col >= 1 && col <= 10) ? col : 1;
        }
    } m_settings;
};



class Group : public CCNode {
private:
    std::string m_groupName;
    short m_objectId;
    std::vector<std::vector<short>> m_matrix;
    bool m_isSingle;
    bool m_isUserCreated;

    CCMenu* m_menu;
    CCScale9Sprite* m_bgSprite;

    bool exchangeItems(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
    void updateButtonPositionsAndBackground();

public:
    static Group* createGroup(std::string name, short objId, std::vector<std::vector<short>>&& matrix);
    static Group* createSingle(short objId, bool isUserCreated);
    static Group* createDefault();
    static Group* create() = delete;

    CreateMenuItem* getCmi();
    void updateMenu();
    void onOpen(CCObject*);
    void onPlusButton(CCObject*);

    void moveItem(bool right, bool down, uint8_t itemX, uint8_t itemY);
    void addColumn(uint8_t index);
    void addRow(uint8_t index);

    // getters
    std::string getName() const {return m_groupName;}
    short getObjId() const {return m_objectId;}
    bool isUserCreated() const {return m_isUserCreated;}
    bool isSingle() const {return m_isSingle;}
    const std::vector<std::vector<short>>& getMatrix() const {return m_matrix;}
};

// // types of the buttons in the EditButtonBar
// enum class BtnType {
//     UserItem,       // added by user
//     DefaultItem,    // added by RobTop (by default)
//     Group           // group
// };

// // each button in EditButtonBar added by user must have this object
// struct BtnInfo : CCObject {
// 	SEL_MenuHandler m_itemSelector;
// 	BtnType m_btnType;
// 	Ref<Group> m_groupObj;

// 	BtnInfo(BtnType type, SEL_MenuHandler defaultSelector, Group* groupObj=nullptr);

// 	void onClickDefault(CCObject* sender);
// };


struct BarInfo : public CCObject {
    bool m_isLoaded;
    uint8_t m_tabIndx;
    BarInfo(uint8_t tabIndex, bool loaded) : m_tabIndx(tabIndex), m_isLoaded(loaded) {
        this->autorelease();
    }
};


// --------------------------- utils --------------------------- 

// replacement for getCreateBtn
// this supports custom colors and can remove button from editor->m_createButtonArray
CreateMenuItem* getCustomCreateBtn(int id, int bg, bool doRegister=true);

// brighten or darken the CreateMenuItem (decompiled function)
void setColorToCreateBtnNew(CreateMenuItem* cmi, bool isBright);

// return number of rows and columns on editButtonBar
void getBarSize(int* rows, int* cols);

int getItemBtnColor(short objId);
int getGroupBtnColor();

std::vector<std::vector<short>> divideGridAlignedObjects(CCArrayExt<GameObject*> objects);

// --------------------------- file --------------------------
bool readConfigFromJson(std::string filename);
bool writeConfigToJson(std::string filename);


