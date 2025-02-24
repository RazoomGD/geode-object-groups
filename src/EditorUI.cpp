#include "EditorUI.hpp"
#include "MoreOptionsPopup.hpp"


// mathematically correct a % b
inline int mod(int a, int b) {return (a % b + b) % b;}


CCMenu* MyEditorUI::setupRowMenu(float scale) {
	const auto rowMenu = CCMenu::create();
	this->addChild(rowMenu); // todo: add children not to editorUI directly
	rowMenu->setAnchorPoint({0.5, 0});
	rowMenu->setLayout(RowLayout::create()->setGap(30));
	rowMenu->setPosition(ccp(CCDirector::get()->getWinSize().width / 2, 111 * scale));
	// rowMenu->setScale(scale * 0.5); ignore
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
	// auto newGroupFromLayoutBtn = CCMenuItemSpriteExtra::create(
	// 	ButtonSprite::create("New group\nfrom layout"), this, 
	// 	menu_selector(MyEditorUI::onNewGroupFromLayoutButton)
	// );
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
	// rowMenu->addChild(newGroupFromLayoutBtn);
	rowMenu->addChild(deleteItemButton);
	rowMenu->addChild(moreOptionsButton);
	
	rowMenu->updateLayout();

	// maxWidth = 320
	rowMenu->setScale(320.f / rowMenu->getContentWidth());

	return rowMenu;
}


CCMenu* MyEditorUI::setupToggleMenu(float scale) {
	auto tMenu = CCMenu::create(); 
	auto spr = CCSprite::create("OG_button_editMode.png"_spr);
	spr->setScale(0.6);
	auto tBtn = CCMenuItemSpriteExtra::create(spr, this, 
		menu_selector(MyEditorUI::toggleEditGroupsMode));
	tMenu->addChild(tBtn);
	this->addChild(tMenu);
	tMenu->setAnchorPoint({1,0});

	tMenu->setLayout(RowLayout::create()->setAxisAlignment(AxisAlignment::End));
	tMenu->setPosition(ccp(CCDirector::get()->getWinSize().width - (7 + 
		this->getChildByID("toolbar-toggles-menu")->getContentWidth()) * scale, 2.5));
	tMenu->setScale(scale);
	tMenu->setContentWidth(100);
	tMenu->setContentHeight(30);
	tMenu->setZOrder(2);
	tMenu->updateLayout();
	tMenu->setID("toggle_menu"_spr);
	return tMenu;
}


void MyEditorUI::setupExtraTabs(int count) {
	int rows, cols;
	getBarSize(&rows, &cols);

	for (int i = 0; i < count; i++) {
		EditorTabs::addTab(this, TabType::BUILD, fmt::format("extra-tab-{}"_spr, i+1),
			// is called once on creation
			[=](EditorUI* ui, CCMenuItemToggler* toggler) -> CCNode* {
				// changed tab icon
				auto objStr = Mod::get()->getSavedValue<std::string>(fmt::format("tab_{}_icon", 13+i), "");
				Global::get().m_editorUI->setSpiteToTabByIndexFromString(objStr, toggler, 13+i);

				auto ret = EditorTabUtils::createEditButtonBar(CCArray::create(), ui);

				// set user obj and call my hook
				ret->setUserObject(BAR_USER_OBJ_ID, new BarInfo(13+i, false));
				ret->loadFromItems(ret->m_buttonArray, cols, rows, true);
				return ret;
			},
			// is called on every tab click
			[](EditorUI*, bool state, CCNode*) {}
		);
	}
}


void MyEditorUI::setupVanillaTabs() {
	int rows, cols;
	getBarSize(&rows, &cols);
	auto tabsMenu = getChildByID("build-tabs-menu");

	for (int i = 0; i < 13; i++) {
		if (auto bar = static_cast<EditButtonBar*>(getChildByID(vanillaTabsInfo[i].barId))) {
			bar->setUserObject(BAR_USER_OBJ_ID, new BarInfo(i, false));
			bar->loadFromItems(bar->m_buttonArray, cols, rows, true);
		}
		if (auto toggler = tabsMenu->getChildByID(vanillaTabsInfo[i].togglerId)) {
			auto objStr = Mod::get()->getSavedValue<std::string>(fmt::format("tab_{}_icon", i), "");
			if (!objStr.empty()) {
				setSpiteToTabByIndexFromString(objStr, static_cast<CCMenuItemToggler*>(toggler), i);
			}
		}
	}
}


// helper function that sets a frame to given cmi (cmi can be nullptr)
void MyEditorUI::setNewFocusedCmi(CreateMenuItem* cmi) {
	m_fields->buttonFrame->removeFromParent();
	if (cmi) cmi->addChild(m_fields->buttonFrame, 5);
}


// helper function that returns a button on which the frame is set (or nullptr)
CreateMenuItem* MyEditorUI::getFocusedCmi() {
	return typeinfo_cast<CreateMenuItem*>(m_fields->buttonFrame->getParent());
}


// pass nullptr to close
void MyEditorUI::setNewOpenedGroup(Group* newGroup, CreateMenuItem* cmi) {
	// close opened group if exists
	if (m_fields->openedGroup.group) {
		m_fields->openedGroup.group->removeFromParent();
		m_fields->openedGroup = {nullptr, nullptr};
	}
	// open new group
	if (newGroup != nullptr && cmi != nullptr) {
		newGroup->removeFromParent();
		cmi->getParent()->addChild(newGroup);
		newGroup->setPosition(cmi->getPosition());
		m_fields->openedGroup = {newGroup, cmi};
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
	editor->m_fields->rowMenu->setVisible(enable);
	if (btn) btn->setColor(enable ? ccc3(127, 127, 127) : ccc3(255, 255, 255));
	editor->m_fields->buttonFrame->setVisible(enable);
}


// enable/disable the row menu
void MyEditorUI::toggleEditGroupsMode(CCObject* sender) {
	auto btn = static_cast<CCMenuItemSpriteExtra*>(sender);
	Global::get().m_isEditMode = !Global::get().m_isEditMode;
	setNewOpenedGroup(nullptr, nullptr);
	if (!Global::get().m_isEditMode) {
		// disable
		if (Global::get().m_hasUnsavedOGChanges) {
			createQuickPopup("Unsaved Changes Warning",
"You have <co>unsaved</c> changes in <cy>Object Groups</c> configuration.\n\
<cj>Do you want to save them?</c>",
				"No, save later", "Yes, save now", 
				[this, btn] (auto, bool isBtn2) {
					if (isBtn2) {
						Global::get().m_editorUI->onSaveButton(nullptr);
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
}


// helper function to find out whether my tab is opened now
inline bool isMyTab(EditButtonBar* tab) {
	return tab->getUserObject(BAR_USER_OBJ_ID) != nullptr;
}


void MyEditorUI::onNewObjectButton(CCObject*) {
	// make sure this is my tab (current bar is editor->m_createButtonBar)
	if (!isMyTab(m_createButtonBar)) {
		alert("Can't create a button in this tab");
		return;
	}
	// get selected object
	auto selected = getSelectedObjects();
	if (selected->count() == 0) {
		alert("You must select at least one object to create a new object button.");
		return;
	}
	// create item on EditButtonBar for this obj
	int currentPage = mod(m_createButtonBar->m_scrollLayer->m_page, 
							m_createButtonBar->m_scrollLayer->getTotalPages());

	int rows, cols;
	getBarSize(&rows, &cols);
	int firstIndex = cols * rows * currentPage; // index the first obj on current page

	if (selected->count() == 1) { 
		int newObjId = static_cast<GameObject*>(selected->objectAtIndex(0))->m_objectID;
		auto newBtn = getCustomCreateBtn(newObjId, getItemBtnColor(newObjId));
		setColorToCreateBtnNew(newBtn, true);
		addButtonsAndReloadCurrentBar(CCArray::createWithObject(newBtn));
		shortAlert("Created!");
		Global::get().m_hasUnsavedOGChanges = true;

	} else {
		auto ids = getUniqueIds(selected);

		createQuickPopup("Object Groups", 
			fmt::format("Are you sure you want to add buttons for <cy>{}</c> objects?", ids.size()),
			"Yes", "No",
			[ids, this] (auto, bool btn2) {
				if (!btn2) {
					auto arr = CCArray::create();
					for (short id : ids) {
						auto newBtn = getCustomCreateBtn(id, getItemBtnColor(id));
						setColorToCreateBtnNew(newBtn, true);
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


void MyEditorUI::onDeleteItemButton(CCObject*) {
	// make sure this is my tab
	if (!isMyTab(m_createButtonBar)) {
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
	uint32_t index = m_createButtonBar->m_buttonArray->indexOfObject(btn);
	if (index == UINT_MAX) {
		alert("Button is not selected or selected in another tab");
		return;
	}
	
	if (auto group = static_cast<Group*>(btn->getUserObject(CMI_USER_OBJ_ID))) {
		if (group->isSingle()) {
			// single object
			group->removeFromParent();
			m_createButtonBar->m_buttonArray->removeObjectAtIndex(index);
			m_createButtonArray->fastRemoveObject(btn);
		} else {
			// group
			group->clearAllCreateMenuItems(); // proper group deletion
			group->removeFromParent();
			m_createButtonBar->m_buttonArray->removeObjectAtIndex(index);
		}
	} else {
		// not a group
		if (btn->m_objectID == 0) {
			alert("Can't delete this button");
			return;
		}
		m_createButtonBar->m_buttonArray->removeObjectAtIndex(index);
		m_createButtonArray->removeObject(btn);
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
	if (!isMyTab(m_createButtonBar)) {
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
	uint32_t index = m_createButtonBar->m_buttonArray->indexOfObject(btn);
	if (index == UINT_MAX) {
		alert("Button is not selected or selected in another tab");
		return;
	}

	// make sure this is not the first nor the last button
	if ((forward && index + 1 >= m_createButtonBar->m_buttonArray->count()) || 
				(!forward && index == 0)) return;
	
	m_createButtonBar->m_buttonArray->exchangeObjectAtIndex(index, index + (forward ? 1 : -1));

	addButtonsAndReloadCurrentBar(CCArray::create()); // only reload
	Global::get().m_hasUnsavedOGChanges = true;
}


void MyEditorUI::onSaveButton(CCObject*) {
	if (!Global::get().m_hasUnsavedOGChanges) {
		shortAlert("No changes were made!", 2);
	} else {
		auto file = Mod::get()->getConfigDir(true).append("OGv2_config.json");
		int result = writeConfigToJson(file.string());
		if (result == 0) {
			Global::get().m_hasUnsavedOGChanges = false;
			shortAlert("Saved!", 2);
		} else if (result == -1) {
			alert(fmt::format("<cr>ERROR:</c> Can't access config file:\n{}\n"
	 					"Configuration wasn't saved! Check that file exists and "
						"isn't locked", file.string()).c_str());
		}
	}
}


void MyEditorUI::onNewGroupButton(CCObject*) {

	// make sure this is my tab (current bar is editor->m_createButtonBar)
	if (!isMyTab(m_createButtonBar)) {
		alert("You can not create a group in this tab");
		return;
	}

	Group* group;
	auto const selected = getSelectedObjects();

	if (selected->count() == 0) {
		group = Group::createDefault();

	} else {
		auto ids = getUniqueIds(selected);
		auto total = ids.size();
		std::vector<std::vector<short>> matrix;

		int rowCount = (total < 5) ? total : ((total < 7 || total == 9) ? 3 : 4);
		int columnCount = ceil(total / (float)rowCount);

		for (int objIter = 0; objIter < total;) {
			std::vector<short> newRow;
			for (int j = 0; j < columnCount; j++) {
				newRow.push_back(ids[objIter++]);
				if (objIter == total) break;
			}
			matrix.push_back(newRow);
		}
		group = Group::createGroup("New Group", ids[0], std::move(matrix));
	}

	auto newBtn = group->getCmi();
	addButtonsAndReloadCurrentBar(CCArray::createWithObject(newBtn));
	Global::get().m_hasUnsavedOGChanges = true;
	shortAlert("Created!");

	newBtn->activate(); // instant open
}


void MyEditorUI::onNewGroupFromLayoutButton(CCObject*) {
	// make sure this is my tab (current bar is editor->m_createButtonBar)
	if (!isMyTab(m_createButtonBar)) {
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
		auto group = Group::createGroup("New Group", firstId, std::move(res));
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

	} else if (m_selectedTab < m_tabsArray->count() && m_selectedTab >= 0) {
		// set new icon to the current tab (if it's mine)
		if (auto uObj = static_cast<BarInfo*>(m_createButtonBar->getUserObject(BAR_USER_OBJ_ID))) {
			// my tab
			auto tabIcon = static_cast<CCMenuItemToggler*>(m_tabsArray->objectAtIndex(m_selectedTab));
			std::string str;
			for (auto* obj : CCArrayExt<GameObject*>(selected)) {
				str += obj->getSaveString(levelLayer) + ";";
			}

			if (setSpiteToTabByIndexFromString(str, tabIcon, uObj->m_tabIndx)) {
				Mod::get()->setSavedValue(fmt::format("tab_{}_icon", uObj->m_tabIndx), str);
				shortAlert(str.empty() ? "Icon reset!" : "Icon set!");
			}

		} else {
			alert("You can't change the icon of this tab.\n<cl>You can only change default "
						"editor tabs and tabs added by</c> <cy>Object Groups</c>");
		}

	}
}


// if str is empty, reset to default
bool MyEditorUI::setSpiteToTabByIndexFromString(std::string objectString, CCMenuItemToggler* tab, uint8_t tabIdx) {

	if (objectString.empty()) {
		if (tabIdx < 13) {
			auto icon = CCSprite::createWithSpriteFrameName(vanillaTabsInfo[tabIdx].textureName);
			icon->setScale(vanillaTabsInfo[tabIdx].textureScale);
			EditorTabUtils::setTabIcon(tab, icon);
		} else {
			auto icon = CCLabelBMFont::create(std::to_string(tabIdx-13+1).c_str(), "bigFont.fnt");
			icon->setScale(0.5f);
			EditorTabUtils::setTabIcon(tab, icon);
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

	EditorTabUtils::setTabIcon(tab, spr);

	return true;
}

// return json array
matjson::Value MyEditorUI::barToJsonValue(EditButtonBar* bar) {
	matjson::Value jsonArray(std::vector<int>{});
	if (isMyTab(bar)) {
		// foreach item in my tab
		for (auto* cmi : CCArrayExt<CreateMenuItem*>(bar->m_buttonArray)) {
			if (auto group = static_cast<Group*>(cmi->getUserObject(CMI_USER_OBJ_ID))) {
				jsonArray.push(group->toJson());
			} else if (cmi->m_objectID != 0) {
				// single not user-created object without any info
				auto unkObj = matjson::makeObject({{"obj", cmi->m_objectID}});
				jsonArray.push(unkObj);
			}
		}
	}
	return jsonArray;
}


void MyEditorUI::addButtonsAndReloadCurrentBar(CCArrayExt<CreateMenuItem*> buttons) {
	int currentPage = mod(m_createButtonBar->m_scrollLayer->m_page, 
		m_createButtonBar->m_scrollLayer->getTotalPages());

	int rows, cols;
	getBarSize(&rows, &cols);
	int firstIndex = cols * rows * currentPage; // index the first obj on current page
	
	for (auto* btn : buttons) {
		m_createButtonBar->m_buttonArray->insertObject(btn, firstIndex++);

		// select (or set frame to) newly created button
		if (btn->m_objectID != 0 && m_selectedObjectIndex == btn->m_objectID) {
			setColorToCreateBtnNew(btn, false);
			setNewFocusedCmi(btn);
		}
	}
	
	m_createButtonBar->loadFromItems(m_createButtonBar->m_buttonArray, cols, rows, true);

	// preserve the page
	if (currentPage > 0) {
		m_createButtonBar->m_scrollLayer->instantMoveToPage(currentPage - 1);
		m_createButtonBar->m_scrollLayer->instantMoveToPage(currentPage);
	}
}

