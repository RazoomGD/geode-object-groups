#pragma once

#include <Geode/Geode.hpp>

#include <alphalaneous.editortab_api/include/EditorTabs.hpp>

#include <matjson.hpp>

#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>


using namespace geode::prelude;


#define BAR_USER_OBJ_ID "OG-bar"
#define CMI_USER_OBJ_ID "OG-cmi"
#define COORDS_USER_OBJ_ID "OG-coord"


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

    // Arrays of groups (index in array is a build tab index)
    std::array<Ref<CCArray>, 20> m_groups; 

    struct OGSettings {
        uint8_t m_extraTabsCount;
        int m_groupBtnColor;
        bool m_showNames;
        bool m_autoClose;
        ccColor4B m_groupBgColor;
        void update() {
            m_extraTabsCount = Mod::get()->getSettingValue<int64_t>("extra-tabs-count");
            m_showNames = Mod::get()->getSettingValue<bool>("show-names");
            m_autoClose = Mod::get()->getSettingValue<bool>("auto-close");
            int col = std::atoi(Mod::get()->getSettingValue<std::string>("group-button-color-v2").c_str());
            m_groupBtnColor = (col >= 1 && col <= 11) ? col : 1;
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


struct {
    const char* barId;          // EditButtonBar id
    const char* togglerId;      // MenuToggler id
    const char* textureName;    // icon texture
    const float textureScale;   // icon texture scale
} vanillaTabsInfo[13] = {
    {"block-tab-bar",       "block-tab",        "square_01_001.png",            0.450f},
    {"outline-tab-bar",     "outline-tab",      "blockOutline_01_001.png",      0.450f},
    {"slope-tab-bar",       "slope-tab",        "triangle_a_02_001.png",        0.450f},
    {"hazard-tab-bar",      "hazard-tab",       "spike_01_001.png",             0.450f},
    {"3d-tab-bar",          "3d-tab",           "persp_outline_01_001.png",     0.800f},
    {"portal-tab-bar",      "portal-tab",       "ring_01_001.png",              0.450f},
    {"monster-tab-bar",     "monster-tab",      "GJBeast01_01_001.png",         0.346f},
    {"pixel-tab-bar",       "pixel-tab",        "pixelb_03_01_001.png",         1.227f},
    {"collectible-tab-bar", "collectible-tab",  "pixelitem_001_001.png",        0.844f},
    {"icon-tab-bar",        "icon-tab",         "particle_01_001.png",          0.844f},
    {"deco-tab-bar",        "deco-tab",         "d_spikes_01_001.png",          0.188f},
    {"sawblade-tab-bar",    "sawblade-tab",     "sawblade_02_001.png",          0.225f},
    {"trigger-tab-bar",     "trigger-tab",      "edit_eTintCol01Btn_001.png",   0.482f}
}; // pain


// --------------------------- utils --------------------------- 

// shorter alert create
inline void alert(const char* text) {FLAlertLayer::create("Object Groups", text, "ok")->show();}
void shortAlert(const char* text, float timeSec=1.5);

// replacement for getCreateBtn
CreateMenuItem* getCustomCreateBtn(int id, int bg, bool doRegister=true);

// brighten or darken the CreateMenuItem (decompiled function)
void setColorToCreateBtnNew(CreateMenuItem* cmi, bool isBright);
void setColorToGameObjectNew(GameObject* gameObj, bool isBright);

void getBarSize(int* rows, int* cols);
int getItemBtnColor(short objId);
int getGroupBtnColor();

std::string toValidString(const char* txt);
bool isObjIdExistsFast(short id);
std::vector<short> getUniqueIds(CCArrayExt<GameObject*> objects);

inline bool isDeveloperMode() {return Mod::get()->getSavedValue<uint64_t>("dev-pass", 0) == 291857115;} 

std::vector<std::vector<short>> divideGridAlignedObjects(CCArrayExt<GameObject*> objects);


// --------------------------- file --------------------------
int readConfigFromJson(std::string filename);
int writeConfigToJson(std::string filename);


