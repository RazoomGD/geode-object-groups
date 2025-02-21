#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/EditButtonBar.hpp>
#include <Geode/modify/BoomScrollLayer.hpp>

#include <alphalaneous.editortab_api/include/EditorTabs.hpp>

#include <string>
#include <matjson.hpp>
#include <vector>
#include <list>
#include <set>
#include <map>

using namespace geode::prelude;



// for marking EditButtonBars
#define BAR_USER_OBJ_ID "OG-bar"
#define CMI_USER_OBJ_ID "OG-cmi"


struct MyEditorUI;


// Mod global state (singleton)
struct Global {
    static Global& get() {
        static Global instance;
        return instance;
    }

    MyEditorUI* m_editorUI;
    bool m_isEditMode;
    bool m_hasUnsavedOGChanges;
    // update notification related
    bool m_isFirstEditorEnter = true;
    bool m_isCurrentVersionSafe = true; // async

    // Group Config (index in array is a build tab index)
    std::array<Ref<CCArray>, 20> m_groups; 

    struct {
        uint8_t m_extraTabsCount;
        int m_groupColor;
        bool m_showNames;
        ccColor4B m_groupBgColor;
        void update() {
            m_extraTabsCount = Mod::get()->getSettingValue<int64_t>("extra-tabs-count");
            m_showNames = Mod::get()->getSettingValue<bool>("show-names");
            int col = std::atoi(Mod::get()->getSettingValue<std::string>("group-button-color-v2").c_str());
            m_groupColor = (col >= 1 && col <= 11) ? col : 1;
            m_groupBgColor = Mod::get()->getSettingValue<ccColor4B>("bg-color-v2");
        }
    } m_settings;
};



struct BarInfo : public CCObject {
    bool m_isLoaded;
    uint8_t m_tabIndx;
    BarInfo(uint8_t tabIndex, bool loaded) : m_tabIndx(tabIndex), m_isLoaded(loaded) {
        this->autorelease();
    }
};


// --------------------------- utils --------------------------- 

// shorter alert create
inline void alert(const char* text) {FLAlertLayer::create("Object Groups", text, "ok")->show();}
void shortAlert(const char* text, float timeSec=1.5);

// replacement for getCreateBtn
// this supports custom colors and can remove button from editor->m_createButtonArray
CreateMenuItem* getCustomCreateBtn(int id, int bg, bool doRegister=true);

// brighten or darken the CreateMenuItem (decompiled function)
void setColorToCreateBtnNew(CreateMenuItem* cmi, bool isBright);
void setColorToGameObjectNew(GameObject* gameObj, bool isBright);

// return number of rows and columns on editButtonBar
void getBarSize(int* rows, int* cols);

int getItemBtnColor(short objId);
int getGroupBtnColor();

std::string toValidString(const char* txt);

std::vector<std::vector<short>> divideGridAlignedObjects(CCArrayExt<GameObject*> objects);

// --------------------------- file --------------------------
int readConfigFromJson(std::string filename);
int writeConfigToJson(std::string filename);


