#pragma once

#include "Group.hpp"

#include <Geode/modify/EditorUI.hpp>

class $modify(MyEditorUI, EditorUI) {
	struct Fields {
		CCMenu* rowMenu = nullptr;
		CCMenu* buildTabRightMenu = nullptr;
		CCMenu* buildTabLeftMenu = nullptr;
		CCNode* pinnedGroups = nullptr; // there must be only groups and nothing else
		Ref<CCNode> buttonFrame = nullptr;
		std::map<std::string, std::string> myCustomObjects; // negative id as string : obj-string

		// Arrays of groups (index in array is a build tab index)
		std::array<Ref<CCArray>, 20> GROUPS; 

		struct {
			Ref<Group> group = nullptr;
			Ref<CreateMenuItem> cmi = nullptr;
		} openedGroup;
		
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
	CCMenu* setupRowMenu(float scale);
	CCMenu* setupRightMenu(float scale);
	CCMenu* setupLeftMenu(float scale);
	void setupExtraTabs(std::set<uint8_t> const &which);
	void setupVanillaTabs();
	void setupSearchTab();

	// hooks
	$override bool init(LevelEditorLayer* editorLayer);
	$override void toggleMode(CCObject* sender);
	$override void updateCreateMenu(bool p0);
	$override void onCreateButton(CCObject* sender);
	$override void showUI(bool show);
	$override void clickOnPosition(CCPoint p0);
	
	// handlers for my menus 
	void toggleEditGroupsMode(CCObject*);
	void onNewObjectButton(CCObject*);
	void onAddAsSingleCustomObjectButton(CCObject*);
	void onNewGroupButton(CCObject*);
	void onMoveForwardButton(CCObject*);
	void onMoveBackwardButton(CCObject*);
	void onSaveButton(CCObject*);
	void onDeleteItemButton(CCObject*);
	void onMoreOptionsButton(CCObject*);
	void onNewGroupFromLayoutButton(CCObject*);
	void onGotoObjectBtn(CCObject*);

	// various helper methods
	void moveSelectedButton(bool forward);
	void addButtonsAndReloadCurrentBar(CCArrayExt<CreateMenuItem*> buttons);
	void createIconForTheTabFromSelectedObjects();
	bool setSpiteToTabByIndexFromString(const std::string& objectString, CCMenuItemToggler* tab, uint8_t tabIdx);
	matjson::Value barToJsonValue(EditButtonBar* bar, std::set<short> &custom);
	void execForeachGroup(std::function<void(Group*, int tabIndex)> func, int whatTab=-1);
	bool addItemToActiveGroupByCmi(CreateMenuItem* cmi);
	short registerNewCustomObject(std::string oldStr);
	std::map<std::string, std::string> getCustomObjects(std::set<short> const &which);
	void updateGroupUIDs();
	void loadGroups(EditButtonBar* bar, CCArray* oldButtons, int tab, int p1, int p2, bool p3);
	EditButtonBar* getCurrentTabIfAllowed();

	void setNewFocusedCmi(CreateMenuItem* cmi);
	CreateMenuItem* getFocusedCmi();

	void setNewSelectedGroupCmi(CreateMenuItem* groupCmi);
	void setNewOpenedGroup(Group* newGroup, CreateMenuItem* cmi);
	Group* getOpenedGroup();

	void toggleSearch(bool forceToggleOff=false);
	void performSearchResult(const std::string& query);

	void updateGroupItem(int id);
	void goToObject(int id, bool openIfInGroup, bool playEffect=true);
};
