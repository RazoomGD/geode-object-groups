#pragma once

#include "Group.hpp"

#include <Geode/modify/EditorUI.hpp>

class $modify(MyEditorUI, EditorUI) {
	struct Fields {
		CCMenu* rowMenu = nullptr;
		CCMenu* rowMenu2 = nullptr;
		CCMenu* buildTabRightMenu = nullptr;
		CCMenu* buildTabLeftMenu = nullptr;
		CCNode* pinnedGroupsNode = nullptr; // there must be only groups and nothing else
		Ref<CCNode> buttonFrame = nullptr;
		std::map<std::string, std::string> myCustomObjects; // negative id as string : obj-string

		// Arrays of groups (index in array is a build tab index)
		std::array<Ref<CCArray>, 25> GROUPS; 

		// Ref<CreateMenuItem> hoveredButtonOverride = nullptr;
		
		Ref<CreateMenuItem> selectedGroupCmi = nullptr;

		Ref<EditButtonBar> searchTabBar = nullptr;

		~Fields() { // on editor exit
			for (int i = 0; i < GROUPS.size(); i++) {
				GROUPS[i] = nullptr;
			}
			Global::get().m_editorUI = nullptr;
		}
	};


	static void onModify(auto& self) {
        (void) self.setHookPriorityAfterPost("EditorUI::init", "nwo5.better_object_tab_icons");
        (void) self.setHookPriorityAfterPost("EditorUI::init", "hjfod.betteredit");
        // (void) self.setHookPriorityAfterPost("EditorUI::toggleMode", "alphalaneous.editortab_api");
        // (void) self.setHookPriorityBeforePost("EditorUI::init", "viper.object_pinning");
		(void) self.setHookPriorityPre("EditorUI::onCreateButton", Priority::EarlyPre);
    }


	// setup methods
	void setupRowMenu();
	void setupRightMenu();
	void setupLeftMenu();
	void setupExtraTabs(std::set<uint8_t> const &which);
	void setupVanillaTabs();
	void setupSearchTab();
	void updateGroupUIDs();
	void updateUiScale();
	
	// hooks
	bool init(LevelEditorLayer* editorLayer);
	void toggleMode(CCObject* sender);
	void updateCreateMenu(bool p0);
	void onCreateButton(CCObject* sender);
	void showUI(bool show);
	void clickOnPosition(CCPoint p0);
	bool ccTouchBegan(CCTouch* touch, CCEvent* event);
	void onPause(CCObject* sender);

	// handlers for my menus 
	void onToggleEditGroupsMode(CCObject*);
	void onNewObjectButton(CCObject*);
	void onAddAsSingleCustomObjectButton(CCObject*);
	void onNewGroupButton(CCObject*);
	void onSaveButton(CCObject*);
	void onDeleteItemButton(CCObject*);
	void onMoreOptionsButton(CCObject*);
	void onNewGroupFromLayoutButton(CCObject*);
	void onGotoObjectBtn(CCObject*);
	void onMoveButton(CCObject*);
	void onToggleSecondRowMenu(CCObject*);
	// void onMoveTabButton(CCObject*);

	// various helper methods
	void addButtonsAndReloadButtonBar(CCArrayExt<CreateMenuItem*> buttons, EditButtonBar* forceTab=nullptr);
	void deleteButtonFromTab(EditButtonBar* bar, int idx, bool ungroup);
	void createIconForTheTabFromSelectedObjects();
	bool setSpiteToTabByIndexFromString(const std::string& objectString, CCMenuItemToggler* tab, uint8_t tabIdx);
	matjson::Value barToJsonValue(EditButtonBar* bar, std::set<int> &custom);
	void execForeachGroup(std::function<void(Group*, int tabIndex)> func, int whatTab=-1);
	bool addItemToActiveGroupByCmi(CreateMenuItem* cmi);
	int registerNewCustomObject(std::string oldStr);
	std::map<std::string, std::string> getCustomObjects(std::set<int> const &which);
	void loadGroups(EditButtonBar* bar, CCArray* oldButtons, int tab, int p1, int p2, bool p3);
	EditButtonBar* getCurrentTabIfAllowed();
	void updateHover(float);

	void setNewFocusedCmi(CreateMenuItem* cmi);
	CreateMenuItem* getFocusedCmi();

	void setNewSelectedGroupCmi(CreateMenuItem* groupCmi);
	Group* getOpenedOrHoveredGroupV2();

	void toggleSearch(bool forceToggleOff=false);
	void performSearchResult(const std::string& query);

	// void updateGroupItem(int id);
	void goToPageWithCmi(CreateMenuItem* target, bool focus=false);
	void goToObjectV2(int id, bool playEffect=true);
};
