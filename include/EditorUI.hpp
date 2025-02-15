#pragma once

#include "Group.hpp"

#include "Group.hpp"


// mathematically correct a % b
// inline int mod(int a, int b) {return (a % b + b) % b;}


class $modify(MyEditorUI, EditorUI) {
	struct Fields {
		Ref<CCMenu> rowMenu = nullptr;
		Ref<CCMenu> toggleMenu = nullptr;
		Ref<CCNode> buttonFrame = nullptr;
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


	// support for BetterEdit scale factor
	// float getBetterEditInterfaceScale();

	CCMenu* setupRowMenu(float scale);

	CCMenu* setupToggleMenu(float scale);

	void setupExtraTabs(int count);

	$override
	bool init(LevelEditorLayer* editorLayer);

	$override
	void toggleMode(CCObject* sender);

	$override
	void updateCreateMenu(bool p0);

	// helper function that sets a frame to given cmi (cmi can be nullptr)
	void setSelectedCmi(CreateMenuItem* cmi);

	// helper function that returns a button on which the frame is set (or nullptr)
	CreateMenuItem* getSelectedCmi();

	$override
	void onCreateButton(CCObject* sender);

	// open/close the group
	void onGroupButton(CreateMenuItem* groupCmi);

	void closeOpenedGroupIfExists();

	// enable/disable the row menu
	void toggleEditGroupsMode(CCObject*);

	// helper function to find out whether my tab is opened now
	// inline bool isMyTab(EditButtonBar* tab);


	// ------------------------------- handlers of rowMenu ------------------------------- 
	void onNewObjectButton(CCObject*);

	void onDeleteItemButton(CCObject*);

	void onMoveForwardButton(CCObject*);

	void onMoveBackwardButton(CCObject*);

	// move selected button in create editButtonBar forward or backward
	void moveSelectedButton(bool forward);

	void onSaveButton(CCObject*);

	void onNewGroupButton(CCObject*);

	void onNewGroupFromLayoutButton(CCObject*);

	void addButtonsAndReloadCurrentBar(CCArrayExt<CreateMenuItem*> buttons);
};
