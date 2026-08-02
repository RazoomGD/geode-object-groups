#pragma once

#include <Geode/Geode.hpp>

#include <matjson.hpp>

#include <utility>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>


using namespace geode::prelude;


#define BAR_USER_OBJ_ID "OG-bar"_spr
#define CMI_USER_OBJ_ID "OG-cmi"_spr
#define INNER_CMI_USER_OBJ_ID "OG-coord"_spr
#define CMI_GROUP_NAME_USER_OBJ_ID "OG-name"_spr
#define CUSTOM_OBJECT_ID_OFFSET -15000 // my objects are -15001, -15002, -15003, ...


struct MyEditorUI;
class Group;


// Mod global state (singleton)
struct Global {
    static Global& get() {
        static Global instance;
        return instance;
    }

    static MyEditorUI* editor() {
        return Global::get().m_editorUI;
    }

    MyEditorUI* m_editorUI;
    bool m_isEditMode;
    bool m_hasUnsavedOGChanges;
    std::unordered_map<uint16_t, CCPoint> m_pinnedGroupsStates;

    struct OGSettings {
        std::string m_extraTabs;
        int m_groupBtnColor;
        int m_font;
        bool m_showNames;
        bool m_autoClose;
        ccColor4B m_groupBgColor;
        ccColor4B m_groupBgHoverColor;
        // bool m_enableSearchTab;
        bool m_enableGoToObject;
        bool m_pinButton;
        bool m_pinGestures;
        bool m_shiftAdd;
        bool m_keepPinned;
        bool m_appendDeleted;
        int m_hoverMode;
        bool m_ignoreHoveredGroups;
        bool m_coloredCustomObjects;
        float m_groupCustomScale;

        void update() {
            auto mod = Mod::get();
            if (auto sett = typeinfo_pointer_cast<SettingBaseValueV3<std::string>>(mod->getSetting("extra-tabs"))) {
                m_extraTabs = sett->getValue();
            }
            if (auto sett = typeinfo_pointer_cast<SettingBaseValueV3<std::string>>(mod->getSetting("enable-pinning"))) {
                m_pinButton = (sett->getValue().find('0') != std::string::npos);
                m_pinGestures = (sett->getValue().find('1') != std::string::npos);
            }
            m_font = mod->getSettingValue<int64_t>("used-font");
            m_showNames = mod->getSettingValue<bool>("show-names");
            m_autoClose = mod->getSettingValue<bool>("auto-close");
            int col = std::atoi(mod->getSettingValue<std::string>("group-button-color-v2").c_str());
            m_groupBtnColor = (col >= 1 && col <= 11) ? col : 1;
            m_groupBgColor = mod->getSettingValue<ccColor4B>("bg-color-v2");
            m_groupBgHoverColor = mod->getSettingValue<ccColor4B>("hover-color-v2");
            // m_enableSearchTab = mod->getSettingValue<bool>("enable-search");
            m_enableGoToObject = mod->getSettingValue<bool>("enable-goto-object");
            m_shiftAdd = mod->getSettingValue<bool>("shift-add");
            m_keepPinned = mod->getSettingValue<bool>("keep-pinned");
            m_appendDeleted = mod->getSettingValue<bool>("append-deleted");
            int hover = std::atoi(mod->getSettingValue<std::string>("hover-behavior").c_str());
            m_hoverMode = (hover >= 1 && hover <= 4) ? hover : 2;
            m_ignoreHoveredGroups = mod->getSettingValue<bool>("ignore-hovered");
            m_coloredCustomObjects = mod->getSettingValue<bool>("colored-custom-objects");
            m_groupCustomScale = mod->getSettingValue<double>("group-custom-scale");

            // future me: don't forget to modify it in MoreOptionsPopup.hpp
        }
    } m_settings;
};


struct BarInfo : public CCObject {
    uint8_t m_tabIndx;
    BarInfo(uint8_t tabIndex) : m_tabIndx(tabIndex) {
        this->autorelease();
    }
};

struct EditorScale {float commonScale; float tabScale;};


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
void alert(const char* text);
void shortAlert(const char* text, float timeSec=0.5);
void callAfterTransition(std::function<void()> func);

// replacement for getCreateBtn
CreateMenuItem* getCustomCreateBtn(int id, int bg, bool doRegister=true, float fixScale=1.f);
CreateMenuItem* getCustomCreateBtn(std::array<int, 4> const &ids, int bg, bool doRegister=true);

// brighten or darken the CreateMenuItem (decompiled function)
void setColorToCreateBtnNew(CreateMenuItem* cmi, bool isBright);
void setColorToGameObjectNew(GameObject* gameObj, bool isBright);

// void getBarSize(int* rows, int* cols);
int getItemBtnColor(int objId);
int getGroupBtnColor();
std::string getFontFileById(int id);
BarInfo* tryGetBarInfo(CCNode* editButtonBar);
EditorScale getEditorScale();
float getTabScale();
void updatePinnedGroupsState();
bool isMyCustomObject(int id);

std::string toValidString(const char* txt);
bool isObjIdExistsFast(int id);
std::vector<int> getUniqueIds(CCArrayExt<GameObject*> objects);
CreateMenuItem* cloneGroupCmi(CreateMenuItem* cmi, Group* group);
void playCircleEffectOnCmi(CreateMenuItem* cmi);

std::vector<std::vector<int>> divideGridAlignedObjects(CCArrayExt<GameObject*> objects);

float computeMatchRatio(const std::string& target, const std::string& query);

// --------------------------- file --------------------------
int readConfigFromJson(std::string filename);
int writeConfigToJson(std::string filename);
std::string copyGroupAsJson(CreateMenuItem* cmi);
std::string copyTabAsJson(EditButtonBar* bar);
void pasteGroupsFromJsonToCurrentTab(std::string data);
