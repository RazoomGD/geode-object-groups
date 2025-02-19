#pragma once

#include "Group.hpp"

class $modify(MyEditorUI, EditorUI) {
	struct Fields {
		Ref<CCMenu> rowMenu = nullptr;
		Ref<CCMenu> toggleMenu = nullptr;
		Ref<CCNode> buttonFrame = nullptr;
		struct {
			bool enable = false;
			CreateMenuItem* fillerObj = nullptr;
		} optimizeGetCreateBtn;
		struct {
			Ref<Group> group = nullptr;
			Ref<CreateMenuItem> cmi = nullptr;
		} openedGroup;

		Fields() {
			// init global config and load data from json
			// for (int i = 0; i < Global::get().m_groups.size(); i++) {
			// 	Global::get().m_groups[i] = CCArray::create();
			// }
			// auto file = Mod::get()->getResourcesDir().append("OGv2_config.json");
			// readConfigFromJson(file.string()); // todo: move the file to save directory
		}
		~Fields() {
			// for (int i = 0; i < Global::get().m_groups.size(); i++) {
			// 	Global::get().m_groups[i] = nullptr;
			// }
		}
	};


	// setup methods
	CCMenu* setupRowMenu(float scale);
	CCMenu* setupToggleMenu(float scale);
	void setupExtraTabs(int count);
	void setupVanillaTabs();

	// hooks
	$override bool init(LevelEditorLayer* editorLayer);
	$override void toggleMode(CCObject* sender);
	$override void updateCreateMenu(bool p0);
	$override void onCreateButton(CCObject* sender);
	$override void showUI(bool show);
	// $override CreateMenuItem* getCreateBtn(int id, int bg); // todo: tmp for test
	
	// handlers for my menus 
	void toggleEditGroupsMode(CCObject*);
	void onNewObjectButton(CCObject*);
	void onDeleteItemButton(CCObject*);
	void onMoveForwardButton(CCObject*);
	void onMoveBackwardButton(CCObject*);
	void onSaveButton(CCObject*);
	void onNewGroupButton(CCObject*);
	void onNewGroupFromLayoutButton(CCObject*);

	// various helper methods
	void moveSelectedButton(bool forward);
	void addButtonsAndReloadCurrentBar(CCArrayExt<CreateMenuItem*> buttons);

	void setSelectedCmi(CreateMenuItem* cmi);
	CreateMenuItem* getSelectedCmi();

	void onGroupButton(CreateMenuItem* groupCmi);
	void closeOpenedGroupIfExists();
};
