#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/EditButtonBar.hpp>
#include <Geode/modify/BoomScrollLayer.hpp>

#include <string>
#include <vector>

using namespace geode::prelude;



// colors: 1-green, 2-cyan, 3-pink, 4-gray, 5-darker gray, 6-red
#define ITEM_COLOR 4
#define DARKER_ITEM_COLOR 5
#define GROUP_ITEM_COLOR 4

#define MAX_CONTROLLED_BARS 20



// Mod global state (singleton)
struct Global {
    static Global& get() {
        static Global instance;
        return instance;
    }

    EditorUI* m_editorUI;
    std::array<EditButtonBar*, MAX_CONTROLLED_BARS> m_controlledBars;
    
    struct {
        void update() {

        }
    } m_settings;
};




enum class BtnType {Item, Group, GroupItem};

// each button in EditButtonBar must have this object
struct BtnInfo : CCObject {
	SEL_MenuHandler m_itemSelector;
	BtnType m_btnType;
	int* m_groupObj;

	BtnInfo(BtnType type, SEL_MenuHandler defaultSelector, int* groupObj=nullptr);

	void onClick(CCObject* sender);
};

class Group : public CCNode {
private:
    std::string m_groupName;
    short m_thumbnailObjId;
    std::vector<std::vector<short>> m_matrix;

public:
    // todo: как я понял matrix - это rvalue ссылка и ее надо передавать через std::move() 
    // (std move превращает lvalue в rvalue, при этом обнуляя данные объекта в месте, где его передавали)
    static Group* create(std::string name, short thumbnailObjId, std::vector<std::vector<short>>&& matrix);
    
};




// --------------------------- utils --------------------------- 

// replacement for getCreateBtn
// this supports custom colors and can remove button from editor->m_createButtonArray
CreateMenuItem* getCustomCreateBtn(int id, int bg, bool doRegister=true);

// set color to CreateMenuItem
void setColorToCreateBtn(CreateMenuItem* cmi, ccColor3B col);



// --------------------------- other --------------------------- 

// mathematically correct a % b
inline int mod(int a, int b) {return (a % b + b) % b;}