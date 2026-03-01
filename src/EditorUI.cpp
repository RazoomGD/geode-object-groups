#include "EditorUI.hpp"
#include "MoreOptionsPopup.hpp"
#include "SearchPopup.hpp"
#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>

// mathematically correct a % b
inline int mod(int a, int b) {return (a % b + b) % b;}

static void setTabIcons(CCMenuItemToggler* toggler, CCNode* on, CCNode* off){
	auto offBtnSprite = toggler->m_offButton->getNormalImage();
	auto onBtnSprite = toggler->m_onButton->getNormalImage();
	offBtnSprite->removeAllChildren();
	onBtnSprite->removeAllChildren();
	onBtnSprite->addChildAtPosition(on, Anchor::Center, ccp(0, -1));
	if (auto rgba = typeinfo_cast<CCRGBAProtocol*>(off))
		rgba->setOpacity(150);
	offBtnSprite->addChildAtPosition(off, Anchor::Center, ccp(0, -1));
}


static std::string convertToEditorTabApiId(const std::string &normalId) {
	return utils::string::replace(normalId, "-tab-bar", "");
}


CCMenu* MyEditorUI::setupRowMenu(float scale) {
	const auto rowMenu = CCMenu::create();
	this->addChild(rowMenu);
	rowMenu->setAnchorPoint({0.5, 0});
	rowMenu->setLayout(RowLayout::create()->setGap(30));
	rowMenu->setPosition(ccp(CCDirector::get()->getWinSize().width / 2, 111 * scale));
	rowMenu->setID("row_menu"_spr);

	auto newObjectBtn = CCMenuItemSpriteExtra::create(
		CCSprite::create("OG_rowBtn_newObject.png"_spr), this, 
		menu_selector(MyEditorUI::onNewObjectButton)
	);
	auto newGroupBtn = CCMenuItemSpriteExtra::create(
		CCSprite::create("OG_rowBtn_newGroup.png"_spr), this, 
		menu_selector(MyEditorUI::onNewGroupButton)
	);
	auto moveForwardBtn = CCMenuItemSpriteExtra::create(
		CCSprite::create("OG_rowBtn_moveRight.png"_spr), this, 
		menu_selector(MyEditorUI::onMoveForwardButton)
	);
	auto moveBackwardBtn = CCMenuItemSpriteExtra::create(
		CCSprite::create("OG_rowBtn_moveLeft.png"_spr), this, 
		menu_selector(MyEditorUI::onMoveBackwardButton)
	);
	auto saveMeBtn = CCMenuItemSpriteExtra::create(
		CCSprite::create("OG_rowBtn_save.png"_spr), this, 
		menu_selector(MyEditorUI::onSaveButton)
	);
	auto deleteItemButton = CCMenuItemSpriteExtra::create(
		CCSprite::create("OG_rowBtn_deleteButton.png"_spr), this, 
		menu_selector(MyEditorUI::onDeleteItemButton)
	);
	auto moreOptionsButton = CCMenuItemSpriteExtra::create(
		CCSprite::create("OG_rowBtn_options.png"_spr), this, 
		menu_selector(MyEditorUI::onMoreOptionsButton)
	); 

	rowMenu->addChild(newObjectBtn);
	rowMenu->addChild(newGroupBtn);
	rowMenu->addChild(moveBackwardBtn);
	rowMenu->addChild(moveForwardBtn);
	rowMenu->addChild(saveMeBtn);
	rowMenu->addChild(deleteItemButton);
	rowMenu->addChild(moreOptionsButton);

	rowMenu->setContentWidth(570);
	
	rowMenu->updateLayout();

	float maxWidth = std::min(320.f, CCDirector::get()->getWinSize().width - 96 * 2 + 10);
	rowMenu->setScale(maxWidth / rowMenu->getContentWidth());

	return rowMenu;
}


CCMenu* MyEditorUI::setupRightMenu(float scale) {
	auto menu = CCMenu::create(); 
	auto spr = CCSprite::create("OG_button_editMode.png"_spr);
	spr->setScale(0.6);
	auto tBtn = CCMenuItemSpriteExtra::create(spr, this, 
		menu_selector(MyEditorUI::toggleEditGroupsMode));
	menu->addChild(tBtn);
	addChild(menu);
	menu->setAnchorPoint({1,0});

	menu->setLayout(RowLayout::create()->setAxisAlignment(AxisAlignment::End));
	menu->setPosition(ccp(CCDirector::get()->getWinSize().width - (7 + 
		getChildByID("toolbar-toggles-menu")->getContentWidth()) * scale, 2.5));
	menu->setScale(scale);
	menu->setContentWidth(100);
	menu->setContentHeight(30);
	menu->setZOrder(2);
	menu->updateLayout();
	menu->setID("toggle_menu"_spr);
	menu->setScale(0.9f * scale);
	return menu;
}


CCMenu* MyEditorUI::setupLeftMenu(float scale) {
	auto menu = CCMenu::create();
	
	if (Global::get().m_settings.m_enableGoToObject) {
		auto spr = CCSprite::create("OG_button_findObject.png"_spr);
		spr->setScale(0.6);
		
		auto tBtn = CCMenuItemSpriteExtra::create(spr, this, 
			menu_selector(MyEditorUI::onGotoObjectBtn));
		menu->addChild(tBtn);
	}

	addChild(menu);
	menu->setAnchorPoint({0,0});

	menu->setLayout(RowLayout::create()->setAxisAlignment(AxisAlignment::Start));
	auto cat = getChildByID("toolbar-categories-menu");
	auto minX = cat->getPositionX() - cat->getAnchorPoint().x * cat->getScaledContentWidth();
	menu->setPosition(ccp((6 + minX + cat->getContentWidth()) * scale, 2.5));

	menu->setScale(scale);
	menu->setContentWidth(100);
	menu->setContentHeight(30);
	menu->setZOrder(2);
	menu->updateLayout();
	menu->setID("goto_obj_menu"_spr);
	menu->setScale(0.9f * scale);
	return menu;
}


// create bar according to object groups config
void MyEditorUI::loadGroups(EditButtonBar* bar, CCArray* oldButtons, int tab, int p1, int p2, bool p3) {
	auto &allGroups = m_fields->GROUPS;
	if (tab >= allGroups.size() || allGroups[tab] == nullptr) {
		bar->loadFromItems(oldButtons, p1, p2, p3);
		// fix arrows overlapping
        if (bar->m_scrollLayer) bar->m_scrollLayer->setZOrder(2);
		return;
	}

	Global::editor()->m_createButtonArray->removeObjectsInArray(oldButtons);

	std::vector<short> allOldIdsOrdered;
	for (auto* btn : CCArrayExt<CreateMenuItem*>(oldButtons)) {
		allOldIdsOrdered.push_back(btn->m_objectID);
	}

	std::set<short> allOldIds(allOldIdsOrdered.begin(), allOldIdsOrdered.end());
	std::set<short> clearedIds;

	auto buttons = CCArray::create();
	auto config = allGroups[tab];

	// fill group-cleared ids
	for (auto* group : CCArrayExt<Group*>(config)) {
		if (!group->isSingle()) {
			auto matrix = group->getMatrix();
			for (int i = 0; i < matrix.size(); i++) {
				for (int j = 0; j < matrix[i].size(); j++) {
					clearedIds.insert(matrix[i][j]);
				}
			}
		} else if (group->isUserCreated()) {
			clearedIds.insert(group->getObjIds()[0]);
		}
	}

	// create new array obj buttons
	for (auto* group : CCArrayExt<Group*>(config)) {
		if (!group->isSingle() || group->isUserCreated()) {
			buttons->addObject(group->getCmi());
		} else {
			short id = group->getObjIds()[0];
			if (allOldIds.contains(id) && !clearedIds.contains(id)) {
				buttons->addObject(group->getCmi());
				clearedIds.insert(id);
			}
		}
	}

	// add objects, that were not used in any of the groups
	for (auto objId : allOldIdsOrdered) {
		if (clearedIds.contains(objId)) continue;
		auto btn = getCustomCreateBtn(objId, getItemBtnColor(objId));
		buttons->addObject(btn);
	}

	bar->loadFromItems(buttons, p1, p2, p3);

	// don't need it anymore, release buttons
	allGroups[tab] = nullptr;

	// fix arrows overlapping
	if (bar->m_scrollLayer) bar->m_scrollLayer->setZOrder(2);
}


void MyEditorUI::setupExtraTabs(std::set<uint8_t> const &which) {
	int rows, cols;
	getBarSize(&rows, &cols);

	for (int i = 0; i < 6; i++) {
		if (!which.contains(i)) continue;

		std::string tabId = fmt::format("extra-tab-{}"_spr, i+1);

		alpha::editor_tabs::addTab(tabId, alpha::editor_tabs::BUILD,
			[this, rows, cols, i] { // Crate the tab
				auto ret = alpha::editor_tabs::createEditButtonBar({});

				ret->setUserObject(BAR_USER_OBJ_ID, new BarInfo(13+i));
				loadGroups(ret, ret->m_buttonArray, 13+i, cols, rows, true);

				return ret;
			}, 
			[] {
				return CCSprite::create();
			}
		);

		if (auto toggler = alpha::editor_tabs::togglerForTab(tabId)) {
			auto objStr = Mod::get()->getSavedValue<std::string>(fmt::format("tab_{}_icon", 13+i), "");
			setSpiteToTabByIndexFromString(objStr, *toggler, 13+i);
		}
	}
}


void MyEditorUI::setupVanillaTabs() {
	int rows, cols;
	getBarSize(&rows, &cols);
	auto tabsMenu = getChildByID("build-tabs-menu");

	for (int i = 0; i < 13; i++) {
		auto tabId = vanillaTabsInfo[i].barId;
		
		if (auto bar = static_cast<EditButtonBar*>(getChildByID(tabId))) {
			bar->setUserObject(BAR_USER_OBJ_ID, new BarInfo(i));
			loadGroups(bar, bar->m_buttonArray, i, cols, rows, true);
		}

		auto shortTabId = convertToEditorTabApiId(tabId);
		if (auto toggler = alpha::editor_tabs::togglerForTab(shortTabId)) {
			auto objStr = Mod::get()->getSavedValue<std::string>(fmt::format("tab_{}_icon", i), "");
			if (!objStr.empty()) {
				setSpiteToTabByIndexFromString(objStr, static_cast<CCMenuItemToggler*>(*toggler), i);
			}
		}
	}
}


void MyEditorUI::setupSearchTab() {
	if (!Global::get().m_settings.m_enableSearchTab) return;

	alpha::editor_tabs::addTab("search-tab"_spr, alpha::editor_tabs::BUILD,
		[this] { // Crate the tab
			auto ret = alpha::editor_tabs::createEditButtonBar({});
			m_fields->searchTabBar = ret;
			return ret;
		},
		[] { // crate tab icon
			auto icon = CCSprite::create("OG_search_icon.png"_spr);
			icon->setScale(0.4);
			return icon;
		},
		[this] (bool state, auto) { // do something when the tab is entered and exited
			// log::info("ts {}", state);
			if (!state) { // means other tab was opened
				toggleSearch(true);
				return;
			};
			toggleSearch();
		}
	);

	// keybinds
	// todo: callbacks are broken
	// addEventListener(KeybindSettingPressedEventV3(GEODE_MOD_ID, "toggle-search"), [this](const Keybind& keybind, bool down, bool repeat, double timestamp) {
	// 	if (down && !repeat) {
	// 		alpha::editor_tabs::switchTab("search-tab"_spr);
	// 		return ListenerResult::Stop;
	// 	}
	// 	return ListenerResult::Propagate;
	// });
	// add this to mod.json
	// "toggle-search": {
	// 	"type": "keybind",
	// 	"name": "Toggle search",
	// 	"description": "Activate object search\n\n<co>This will work only when the 'Group search' setting is enabled</c>",
	// 	"default": "Ctrl+Shift+F",
	// 	"category": "editor"
	// }
}


// helper function that sets a frame to given cmi (cmi can be nullptr)
void MyEditorUI::setNewFocusedCmi(CreateMenuItem* cmi) {
	m_fields->buttonFrame->removeFromParent();
	if (cmi) {
		cmi->addChild(m_fields->buttonFrame, 5);
		if (auto buttonSpr = cmi->getNormalImage()) {
			m_fields->buttonFrame->setPosition(buttonSpr->getPosition());
		}
	}
}


// helper function that returns a button on which the frame is set (or nullptr)
CreateMenuItem* MyEditorUI::getFocusedCmi() {
	return typeinfo_cast<CreateMenuItem*>(m_fields->buttonFrame->getParent());
}


// pass nullptr to close
void MyEditorUI::setNewOpenedGroup(Group* newGroup, CreateMenuItem* cmi) {
	// close opened group if exists
	if (m_fields->openedGroup.group) {
		// m_fields->openedGroup.group->removeFromParentAndCleanup(false); <-- no way, another mem leak
		m_fields->openedGroup.group->removeFromParent();
		// m_fields->openedGroup = {nullptr, nullptr}; <-- memory leak
		m_fields->openedGroup.cmi = nullptr;
		m_fields->openedGroup.group = nullptr;
	}
	// open new group
	if (newGroup != nullptr && cmi != nullptr) {
		newGroup->removeFromParent();
		cmi->getParent()->addChild(newGroup);
		newGroup->setPosition(cmi->getPosition());
		m_fields->openedGroup.group = newGroup;
		m_fields->openedGroup.cmi = cmi;
	}
}


Group* MyEditorUI::getOpenedGroup() {
	return m_fields->openedGroup.group;
}


void MyEditorUI::setNewSelectedGroupCmi(CreateMenuItem* groupCmi) {
	if (m_fields->selectedGroupCmi) {
		setColorToCreateBtnNew(m_fields->selectedGroupCmi, true);
	}
	if (groupCmi) {
		setColorToCreateBtnNew(groupCmi, false);
	}
	m_fields->selectedGroupCmi = groupCmi;
}


inline void setEditModeEnabled(MyEditorUI* editor, CCNodeRGBA* btn, bool enable) {
	if (auto menu = editor->m_fields->rowMenu) menu->setVisible(enable);
	if (btn) btn->setColor(enable ? ccc3(127, 127, 127) : ccc3(255, 255, 255));
	editor->m_fields->buttonFrame->setVisible(enable);
}


// enable/disable the row menu
void MyEditorUI::toggleEditGroupsMode(CCObject* sender) {
	auto btn = static_cast<CCMenuItemSpriteExtra*>(sender);
	Global::get().m_isEditMode = !Global::get().m_isEditMode;
	// setNewOpenedGroup(nullptr, nullptr);
	if (!Global::get().m_isEditMode) {
		// disable
		if (Global::get().m_hasUnsavedOGChanges) {
			createQuickPopup("Unsaved Changes Warning",
				"You have <co>unsaved</c> changes in <cy>Object Groups</c> configuration.\n"
				"<cj>Do you want to save them?</c>",
				"No, save later", "Yes, save now", 
				[this, btn] (auto, bool isBtn2) {
					if (isBtn2) {
						Global::editor()->onSaveButton(nullptr);
					}

					setEditModeEnabled(this, btn, false);
				}, 
				true, true
			);

		} else {
			setEditModeEnabled(this, btn, false);
		}

	} else if (m_selectedMode == 2 /* build mode */) {
		// enable
		setEditModeEnabled(this, btn, true);
	}

	// update all pinned groups
	if (auto pinned = m_fields->pinnedGroups->getChildren()) {
		for (auto group : CCArrayExt<Group*>(pinned)) {
			group->setUpdateRequired(true);
			group->updateMenu(true);
		}
	}

	// update opened group
	if (auto group = getOpenedGroup()) {
		group->setUpdateRequired(true);
		group->updateMenu(true);
	}
}


// helper function to find out whether my tab is opened now
EditButtonBar* MyEditorUI::getCurrentTabIfAllowed() {
	if (auto currTabId = alpha::editor_tabs::getCurrentTab()) {
		if (auto node = alpha::editor_tabs::nodeForTab(*currTabId)) {
			if (tryGetBarInfo(*node)) {
				return static_cast<EditButtonBar*>(**node);
			}
		}
	}
	return nullptr;
}


void MyEditorUI::onNewObjectButton(CCObject*) {

	// make sure this is my tab
	if (!getCurrentTabIfAllowed()) {
		alert("You can't create a button in this tab");
		return;
	}

	// get selected object
	auto ids = getUniqueIds(getSelectedObjects());
	if (ids.size() == 0) {
		alert("You must select at least one object to create a new object button.\n"
				"<cj>(this object must be placed and selected in the editor)</c>");
		return;
	}

	// create item on EditButtonBar for this obj
	if (ids.size() == 1) { 
		auto newGroup = Group::createSingle(ids[0], true);
		auto newBtn = newGroup->getCmi();

		addButtonsAndReloadCurrentBar(CCArray::createWithObject(newBtn));
		shortAlert("Created!");
		Global::get().m_hasUnsavedOGChanges = true;

	} else {
		createQuickPopup("Object Groups", 
			fmt::format("Are you sure you want to add buttons for <cy>{}</c> objects?", ids.size()),
			"Yes", "No",
			[ids, this] (auto, bool btn2) {
				if (!btn2) {
					auto arr = CCArray::create();
					for (short id : ids) {
						auto newGroup = Group::createSingle(id, true);
						auto newBtn = newGroup->getCmi();
						arr->addObject(newBtn);
					}
					addButtonsAndReloadCurrentBar(arr);
					shortAlert("Created!");
					Global::get().m_hasUnsavedOGChanges = true;
				}
			},
			true, true
		);
	}
}


void MyEditorUI::onAddAsSingleCustomObjectButton(CCObject*) {
	// make sure this is my tab
	if (!getCurrentTabIfAllowed()) {
		alert("You can't create a button in this tab");
		return;
	}
	
	// get selected object
	auto selected = getSelectedObjects();
	if (selected->count() < 2) {
		alert("You must select at least 2 objects to create a new <cy>custom object</c> button.");
		return;
	}

	// create custom
	auto levelLayer = LevelEditorLayer::get();
	std::string str;
	for (auto* obj : CCArrayExt<GameObject*>(selected)) {
		str = str.append(obj->getSaveString(levelLayer)).append(";");
	}
	short newId = registerNewCustomObject(str);

	// create item on EditButtonBar for this obj
	auto newGroup = Group::createSingle(newId, true);
	newGroup->setUserCreated(true);
	auto newBtn = newGroup->getCmi();
	addButtonsAndReloadCurrentBar(CCArray::createWithObject(newBtn));

	shortAlert("Created!");
	Global::get().m_hasUnsavedOGChanges = true;
}


void MyEditorUI::onDeleteItemButton(CCObject*) {
	// make sure this is my tab
	auto currentTab = getCurrentTabIfAllowed();
	if (!currentTab) {
		alert("You can't edit this tab");
		return;
	}

	// make sure the button is selected
	auto btn = getFocusedCmi();
	if (btn == nullptr) {
		alert("Button is not selected");
		return;
	}

	// make sure that button is in this tab
	uint32_t index = currentTab->m_buttonArray->indexOfObject(btn);
	if (index == UINT_MAX) {
		alert("Button is not selected or selected in another tab");
		return;
	}
	
	if (auto group = static_cast<Group*>(btn->getUserObject(CMI_USER_OBJ_ID))) {
		btn->retain();
		group->retain();

		if (group->isSingle()) {
			// single object
			group->removeFromParent();
			currentTab->m_buttonArray->removeObjectAtIndex(index);
			m_createButtonArray->fastRemoveObject(btn);
		} else {
			// group
			group->clearAllCreateMenuItems(); // proper group deletion
			group->removeFromParent();
			currentTab->m_buttonArray->removeObjectAtIndex(index);
		}

		// basically swap parent and child roles to never lose original cmi
		btn->setUserObject(CMI_USER_OBJ_ID, nullptr);
		group->setUserObject("abc"_spr, btn);

		btn->release();
		group->release();

	} else {
		// not a group
		if (btn->m_objectID == 0) {
			alert("You can't delete this button");
			return;
		}
		currentTab->m_buttonArray->removeObjectAtIndex(index);
		m_createButtonArray->fastRemoveObject(btn);
	}
	addButtonsAndReloadCurrentBar(CCArray::create()); // only reload
	shortAlert("Deleted!");
	Global::get().m_hasUnsavedOGChanges = true;
}


void MyEditorUI::onMoreOptionsButton(CCObject*) {
	MoreOptionsPopup::create()->show();
}


void MyEditorUI::onMoveForwardButton(CCObject*) {
	moveSelectedButton(true);
}


void MyEditorUI::onMoveBackwardButton(CCObject*) {
	moveSelectedButton(false);
}


// move selected button in create editButtonBar forward or backward
void MyEditorUI::moveSelectedButton(bool forward) {
	// make sure this is my tab
	auto currentTab = getCurrentTabIfAllowed();
	if (!currentTab) {
		alert("You can't edit this tab");
		return;
	}

	// make sure the button is selected
	auto btn = getFocusedCmi();
	if (btn == nullptr) {
		alert("Button is not selected");
		return;
	}

	// make sure that button is in this tab
	uint32_t index = currentTab->m_buttonArray->indexOfObject(btn);
	if (index == UINT_MAX) {
		alert("Button is not selected or selected in another tab");
		return;
	}

	// make sure this is not the first nor the last button
	if ((forward && index + 1 >= currentTab->m_buttonArray->count()) || 
				(!forward && index == 0)) return;
	
	currentTab->m_buttonArray->exchangeObjectAtIndex(index, index + (forward ? 1 : -1));

	addButtonsAndReloadCurrentBar(CCArray::create()); // only reload
	Global::get().m_hasUnsavedOGChanges = true;
}


void MyEditorUI::onSaveButton(CCObject*) {
	if (!Global::get().m_hasUnsavedOGChanges) {
		shortAlert("No changes were made!", 0.8);
	} else {
		auto file = Mod::get()->getConfigDir(true).append("OGv2_config.json");
		int result = writeConfigToJson(file.string());
		if (result == 0) {
			Global::get().m_hasUnsavedOGChanges = false;
			shortAlert("Saved!", 2);
			updateGroupUIDs();
		} else if (result == -1) {
			alert(fmt::format("<cr>ERROR:</c> Can't access config file:\n{}\n"
	 					"Configuration wasn't saved! Check that file exists and "
						"isn't locked", file.string()).c_str());
		}
	}
}


void MyEditorUI::onNewGroupButton(CCObject*) {

	// make sure this is my tab
	if (!getCurrentTabIfAllowed()) {
		alert("You can't create a group in this tab");
		return;
	}

	Group* group;
	auto const selected = getSelectedObjects();

	if (selected->count() == 0) {
		group = Group::createDefault();

	} else {
		auto ids = getUniqueIds(selected);
		group = Group::createFromArray("New Group", {ids[0],0,0,0}, std::move(ids));
	}

	auto newBtn = group->getCmi();
	addButtonsAndReloadCurrentBar(CCArray::createWithObject(newBtn));
	Global::get().m_hasUnsavedOGChanges = true;
	shortAlert("Created!");

	newBtn->activate(); // instant open
}


bool MyEditorUI::addItemToActiveGroupByCmi(CreateMenuItem* cmi) {
	// this function is used in "shift-add" feature
	if (cmi->m_objectID == 0) return false;

	const std::vector<short> objId = {(short)cmi->m_objectID};
	const auto pinned = m_fields->pinnedGroups->getChildren();
	const auto focusedCmi = getFocusedCmi();
	std::vector<Group*> candidates;

	if (auto gr = getOpenedGroup(); gr && isGroupVisibleOnScreen(gr)) {
		if (gr->containsButton(focusedCmi)) {
			gr->addObjects(objId);
			return true;
		}
		candidates.push_back(gr);
	}

	for (auto gr : CCArrayExt<Group*>(pinned)) { 
		if (gr->containsButton(focusedCmi)) {
			gr->addObjects(objId);
			return true;
		}
		candidates.push_back(gr);
	}

	if (candidates.size() == 0) {
		alert("<co>Object wasn't added to the group</c> because there are no opened "
			"groups on screen. \n<cy>Tip:</c>You can <cp>pin</c> groups to keep them on screen");
		return false;
	}
	if (candidates.size() == 1) {
		candidates[0]->addObjects(objId);
		return true;
	}
	if (candidates.size() == 2) {
		if (candidates[0]->containsButton(cmi)) {
			candidates[1]->addObjects(objId);
			return true;
		}
		if (candidates[1]->containsButton(cmi)) {
			candidates[0]->addObjects(objId);
			return true;
		}
	}
	alert("<cj>Object wasn't added to the group</c> because the group choice is ambiguous");
	return false;
}


short MyEditorUI::registerNewCustomObject(std::string oldStr) {
	// get next free id
	int id = CUSTOM_OBJECT_ID_OFFSET - 1;
	auto &custom = m_fields->myCustomObjects;
	while (custom.contains(std::to_string(id))) id--;

	// get original string and save to the map
	custom.insert({std::to_string(id), oldStr});

	Global::get().m_hasUnsavedOGChanges = true;
	return id;
}


std::map<std::string, std::string> MyEditorUI::getCustomObjects(std::set<short> const &which) {
	std::map<std::string, std::string> ret;
	for (short id : which) {
		ret.insert({std::to_string(id), GameManager::get()->stringForCustomObject(id)});
	}
	return ret;
}


void MyEditorUI::updateGroupUIDs() {
	uint16_t iter = 0;
	execForeachGroup([&iter](Group* g, auto){g->setGroupUID(++iter);});
}


void MyEditorUI::onNewGroupFromLayoutButton(CCObject*) {
	// make sure this is my tab
	if (!getCurrentTabIfAllowed()) {
		alert("You can not create a group in this tab");
		return;
	}

	auto const selected = getSelectedObjects();

	if (selected->count() == 0) {
		alert("You must select at least <cy>one</c> object to create a group from layout");
		return;
	}
	// try to detect the grid-alignment
	auto res = divideGridAlignedObjects(selected);
	short firstId = static_cast<GameObject*>(selected->objectAtIndex(0))->m_objectID;

	if (!res.empty()) {
		auto group = Group::createGroup("New Group", {firstId,0,0,0}, std::move(res));
		auto newBtn = group->getCmi();
		addButtonsAndReloadCurrentBar(CCArray::createWithObject(newBtn));
		Global::get().m_hasUnsavedOGChanges = true;
		shortAlert("Created!");

		newBtn->activate(); // instant open
	} else {
		alert("<co>Layout not detected</c>: Selected objects cannot be arranged while "
					"preserving their relative positions. Check that there are no multiple "
					"objects at the same spot");
	}
}


void MyEditorUI::createIconForTheTabFromSelectedObjects() {
	auto selected = getSelectedObjects();
	auto levelLayer = LevelEditorLayer::get();
	
	if (selected->count() > 48) {
		alert("<cg>Sorry, but I've set a limit of 48 objects. Use them wisely</c>");
		return;
	} 
	
	// set new icon to the current tab (if it's mine)
	if (auto currentTab = getCurrentTabIfAllowed()) {
		auto shortTabId = convertToEditorTabApiId(currentTab->getID());
		if (auto toggler = alpha::editor_tabs::togglerForTab(shortTabId)) {
			std::string str;
			for (auto* obj : CCArrayExt<GameObject*>(selected)) {
				str = str.append(obj->getSaveString(levelLayer)).append(";");
			}
			if (auto uObj = tryGetBarInfo(currentTab)) {
				if (setSpiteToTabByIndexFromString(str, *toggler, uObj->m_tabIndx)) {
					Mod::get()->setSavedValue(fmt::format("tab_{}_icon", uObj->m_tabIndx), str);
					shortAlert(str.empty() ? "Reset icon!" : "Set icon!");
					return;
				}
			}
		}
	}

	alert("You can't change the icon of this tab.\n<cl>You can only change default "
				"editor tabs and tabs added by</c> <cy>Object Groups</c>");
}


// if str is empty, reset to default
bool MyEditorUI::setSpiteToTabByIndexFromString(const std::string& objectString, CCMenuItemToggler* tab, uint8_t tabIdx) {

	if (objectString.empty()) {
		if (tabIdx < 13) {
			auto icon = CCSprite::createWithSpriteFrameName(vanillaTabsInfo[tabIdx].textureName);
			icon->setScale(vanillaTabsInfo[tabIdx].textureScale);
			setTabIcons(tab, icon, icon);
		} else {
			auto icon = CCLabelBMFont::create(std::to_string(tabIdx-13+1).c_str(), "bigFont.fnt");
			icon->setScale(0.5f);
			setTabIcons(tab, icon, icon);
		}
		
		return true;
	}

	auto levelLayer = LevelEditorLayer::get();
	auto arr = CCArray::create();

	auto spr = spriteFromObjectString(objectString, false, false, 0, arr, nullptr, nullptr);

	for (auto* el : CCArrayExt<GameObject*>(arr)) {
		setColorToGameObjectNew(el, true);
	}

	// max size is 13x26
	float vScaleRatio = 13.f / spr->getContentHeight();
	float hScaleRatio = 26.f / spr->getContentWidth();

	spr->setScale(std::min(vScaleRatio, hScaleRatio));

	spr->setCascadeOpacityEnabled(true);
	spr->setOpacity(150);

	setTabIcons(tab, spr, spr);

	return true;
}

// return json array
matjson::Value MyEditorUI::barToJsonValue(EditButtonBar* bar, std::set<short> &custom) {
	matjson::Value jsonArray(std::vector<int>{});
	// foreach item in my tab
	for (auto* cmi : CCArrayExt<CreateMenuItem*>(bar->m_buttonArray)) {
		if (auto group = static_cast<Group*>(cmi->getUserObject(CMI_USER_OBJ_ID))) {
			jsonArray.push(group->toJson(custom));
		} else if (cmi->m_objectID != 0) {
			// single not user-created object without any info
			auto unkObj = matjson::makeObject({{"obj", cmi->m_objectID}});
			jsonArray.push(unkObj);
			if (cmi->m_objectID < 0) custom.insert(cmi->m_objectID);
		}
	}
	return jsonArray;
}


void MyEditorUI::execForeachGroup(std::function<void(Group*, int tabIndex)> func, int whatTab) {
	if (auto allTabs = alpha::editor_tabs::getAllTabs()) {
		for (auto bar : *allTabs) {
			if (auto uObj = tryGetBarInfo(bar)) {
				int tabIdx = uObj->m_tabIndx;
				if (whatTab != -1 && whatTab != tabIdx) continue;
				// foreach item in my tab
				for (auto* cmi : CCArrayExt<CreateMenuItem*>(static_cast<EditButtonBar*>(bar)->m_buttonArray)) {
					if (auto group = static_cast<Group*>(cmi->getUserObject(CMI_USER_OBJ_ID))) {
						func(group, tabIdx);
					}
				}
			}
		}
	}
}


void MyEditorUI::addButtonsAndReloadCurrentBar(CCArrayExt<CreateMenuItem*> buttons) {

	auto currentTab = getCurrentTabIfAllowed();
	if (!currentTab) {
		log::warn("Editing this tab is not allowed (addButtonsAndReloadCurrentBar)");
		return;
	}

	int currentPage = mod(currentTab->m_scrollLayer->m_page, 
		currentTab->m_scrollLayer->getTotalPages());

	int rows, cols;
	getBarSize(&rows, &cols);
	int firstIndex = cols * rows * currentPage; // index the first obj on current page

	// check if the selected cmi on the current page
	if (auto selectedCmi = getFocusedCmi()) {
		auto array = currentTab->m_buttonArray;
		for (int i = 0; i < cols * rows; i++) {
			if (firstIndex + i >= array->count()) break;
			if (array->objectAtIndex(firstIndex + i) == selectedCmi) { // found
				firstIndex = firstIndex + i;
				break;
			}
		}
	}
	
	for (auto* btn : buttons) {
		currentTab->m_buttonArray->insertObject(btn, firstIndex++);

		// select (or set frame to) newly created button
		if (btn->m_objectID != 0 || btn->getUserObject(CMI_USER_OBJ_ID)) {
			if (m_selectedObjectIndex == btn->m_objectID)
				setColorToCreateBtnNew(btn, false);
			setNewFocusedCmi(btn);
		}
	}
	
	currentTab->loadFromItems(currentTab->m_buttonArray, cols, rows, true);

	// preserve the page
	currentTab->goToPage(currentPage);
}


void MyEditorUI::toggleSearch(bool forceToggleOff) {
	if (!m_fields->searchTabBar) return;
	if (auto oldPopup = CCScene::get()->getChildByID("search-popup"_spr)) {
		// already opened
		static_cast<GroupSearchPopup*>(oldPopup)->onClose(nullptr);
		return; 
	}
	if (!forceToggleOff) {
		auto popup = GroupSearchPopup::create();
		popup->setID("search-popup"_spr);
		popup->setPositionY(popup->getPositionY() + 70);
		popup->show();
	}
}


void MyEditorUI::performSearchResult(const std::string& query) {
	int rows, cols;
	getBarSize(&rows, &cols);
	int resultCount = rows * cols;
	std::vector<std::pair<Group*, float>> res;
	auto bar = m_fields->searchTabBar;
	if (!bar) return;

	setNewOpenedGroup(nullptr, nullptr); // close

	if (query.size() == 0) {
		bar->m_buttonArray->removeAllObjects();
		bar->loadFromItems(bar->m_buttonArray, cols, rows, true);
		return;
	}

	execForeachGroup([&query, &res](Group* g, int) {
		if (!g->isSingle() && !g->getName().empty()) {
			auto ratio = computeMatchRatio(g->getName(), query);
			if (ratio > 0) res.push_back({g, ratio});
		}
	});

	if (res.size() == 0) return;

	std::sort(res.begin(), res.end(), 
		[](std::pair<Group*, float> a, std::pair<Group*, float> b) {
			return a.second > b.second;
		}
	);

	const float ratioLimit = res[0].second * 0.5f;
	bar->m_buttonArray->removeAllObjects();
	for (int i = 0; i < res.size(); i++) {
		if (i == resultCount || res[i].second <= ratioLimit) break;
		if (auto newCmi = cloneGroupCmi(res[i].first->getCmi(), res[i].first)) {
			bar->m_buttonArray->addObject(newCmi);
		}
	}
	bar->loadFromItems(bar->m_buttonArray, cols, rows, true);
}


void MyEditorUI::onGotoObjectBtn(CCObject*) {
	auto selected = getSelectedObjects();
	if (auto obj = static_cast<GameObject*>(selected->firstObject())) {
		goToObject(obj->m_objectID, false);
		if (obj->m_objectID != m_selectedObjectIndex) {
			m_selectedObjectIndex = obj->m_objectID;
			updateCreateMenu(false);
		}
	} else {
		alert("<cr>Objects not selected!</c>\nSelect an object in the editor and then press this <cl>button</c> or <cl>Ctrl+F</c> shortcut.\n"
			"<cy>Selected object/group will be highlighted in the build tab</c>\n"
			"(you can toggle off '<cj>Object Search</c>' in mod settings to hide this button, "
			"and option will be available only via shortcut)");
	}
}


void MyEditorUI::goToObject(int id, bool openIfInGroup, bool playEffect) {

	int rows, cols;
	getBarSize(&rows, &cols);
	int const pgSize = cols * rows;

	auto allTabs = alpha::editor_tabs::getAllTabs();
	if (!allTabs) return;

	// foreach tab
	int barIndex = -1;
	for (auto node : *allTabs) {
		barIndex++;
		if (!tryGetBarInfo(node)) continue;

		auto bar = static_cast<EditButtonBar*>(node);
		auto shortTabId = convertToEditorTabApiId(node->getID());
		
		// foreach item in my tab
		int buttonIndex = -1;
		for (auto* cmi : CCArrayExt<CreateMenuItem*>(bar->m_buttonArray)) {
			buttonIndex++;
			auto group = static_cast<Group*>(cmi->getUserObject(CMI_USER_OBJ_ID));
			if (!group || group->isSingle()) { // single item
				if (id == cmi->m_objectID) {
					// found single
					int currentPage = buttonIndex / pgSize;
					alpha::editor_tabs::switchTab(shortTabId);
					bar->goToPage(currentPage);
					
					if (playEffect) {
						playCircleEffectOnCmi(cmi);
					}
					return;
				}
			} else { // group
				auto &matrix = group->getMatrix();
				for (int i = 0; i < matrix.size(); i++) {
					auto &row = matrix[i];
					for (int j = 0; j < row.size(); j++) {
						if (id == row[j]) {
							// found in group
							int currentPage = buttonIndex / pgSize;
							alpha::editor_tabs::switchTab(shortTabId);
							bar->goToPage(currentPage);

							bool openedOrPinned = (group == getOpenedGroup()) || 
										(group->getParent() == m_fields->pinnedGroups);

							if (!openedOrPinned && openIfInGroup) {
								cmi->activate(); // open group
								openedOrPinned = true;
							}

							if (playEffect) {
								if (openedOrPinned) {
									if (auto innerCmi = group->getCmiByPosition(j,i)) {
										playCircleEffectOnCmi(innerCmi);
									}
								} else {
									playCircleEffectOnCmi(cmi);
								}
							}
							return;
						}
					}
				}
			}
		}
	}
}


