#include "EditorUI.hpp"
#include "MoreOptionsPopup.hpp"


// mathematically correct a % b
inline int mod(int a, int b) {return (a % b + b) % b;}


CCMenu* MyEditorUI::setupRowMenu(float scale) {
	const auto rowMenu = CCMenu::create();
	this->addChild(rowMenu); // todo: add children not to editorUI directly
	rowMenu->setAnchorPoint({0.5, 0});
	rowMenu->setLayout(RowLayout::create());
	rowMenu->setPosition(ccp(CCDirector::get()->getWinSize().width / 2, 111 * scale));
	rowMenu->setScale(scale * 0.5);
	rowMenu->setID("row_menu"_spr);

	auto newObjectBtn = CCMenuItemSpriteExtra::create(
		ButtonSprite::create("New\nobject"), this, 
		menu_selector(MyEditorUI::onNewObjectButton)
	);
	auto moveForwardBtn = CCMenuItemSpriteExtra::create(
		ButtonSprite::create("Move\n -->"), this, 
		menu_selector(MyEditorUI::onMoveForwardButton)
	);
	auto moveBackwardBtn = CCMenuItemSpriteExtra::create(
		ButtonSprite::create("Move\n <--"), this, 
		menu_selector(MyEditorUI::onMoveBackwardButton)
	);
	auto saveMeBtn = CCMenuItemSpriteExtra::create(
		ButtonSprite::create("Save\nchanges"), this, 
		menu_selector(MyEditorUI::onSaveButton)
	);
	auto newGroupBtn = CCMenuItemSpriteExtra::create(
		ButtonSprite::create("New\ngroup"), this, 
		menu_selector(MyEditorUI::onNewGroupButton)
	);
	auto newGroupFromLayoutBtn = CCMenuItemSpriteExtra::create(
		ButtonSprite::create("New group\nfrom layout"), this, 
		menu_selector(MyEditorUI::onNewGroupFromLayoutButton)
	);
	auto deleteItemButton = CCMenuItemSpriteExtra::create(
		ButtonSprite::create("Delete\nbutton"), this, 
		menu_selector(MyEditorUI::onDeleteItemButton)
	);
	auto moreOptionsButton = CCMenuItemSpriteExtra::create(
		ButtonSprite::create("More\n "), this, 
		menu_selector(MyEditorUI::onMoreOptionsButton)
	);	

	rowMenu->addChild(newObjectBtn);
	rowMenu->addChild(moveForwardBtn);
	rowMenu->addChild(moveBackwardBtn);
	rowMenu->addChild(saveMeBtn);
	rowMenu->addChild(newGroupBtn);
	rowMenu->addChild(newGroupFromLayoutBtn);
	rowMenu->addChild(deleteItemButton);
	rowMenu->addChild(moreOptionsButton);
	
	rowMenu->updateLayout();

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
				auto icon = CCLabelBMFont::create(std::to_string(i+1).c_str(), "bigFont.fnt");
				icon->setScale(0.5f);
				EditorTabUtils::setTabIcon(toggler, icon);
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
	const char* const names[] = {
		"block-tab-bar", 
		"outline-tab-bar",
		"slope-tab-bar",
		"hazard-tab-bar",
		"3d-tab-bar",
		"portal-tab-bar",
		"monster-tab-bar",
		"pixel-tab-bar",
		"collectible-tab-bar",
		"icon-tab-bar",
		"deco-tab-bar",
		"sawblade-tab-bar",
		"trigger-tab-bar"
	};

	int rows, cols;
	getBarSize(&rows, &cols);

	for (int i = 0; i < 13; i++) {
		auto bar = static_cast<EditButtonBar*>(getChildByID(names[i]));
		bar->setUserObject(BAR_USER_OBJ_ID, new BarInfo(i, false));
		bar->loadFromItems(bar->m_buttonArray, cols, rows, true);
	}
}


// helper function that sets a frame to given cmi (cmi can be nullptr)
void MyEditorUI::setSelectedCmi(CreateMenuItem* cmi) {
	m_fields->buttonFrame->removeFromParent();
	if (cmi) cmi->addChild(m_fields->buttonFrame, 5);
}


// helper function that returns a button on which the frame is set (or nullptr)
CreateMenuItem* MyEditorUI::getSelectedCmi() {
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


// enable/disable the row menu
void MyEditorUI::toggleEditGroupsMode(CCObject* sender) {
	auto btn = static_cast<CCMenuItemSpriteExtra*>(sender);
	Global::get().m_isEditMode = !Global::get().m_isEditMode;
	setNewOpenedGroup(nullptr, nullptr);
	if (!Global::get().m_isEditMode) {
		// disable
		// todo: handle unsaved changes
		m_fields->rowMenu->setVisible(false);
		if (btn) btn->setColor(ccc3(255, 255, 255));
	} else if (m_selectedMode == 2 /* build mode */) {
		// enable
		m_fields->rowMenu->setVisible(true);
		if (btn) btn->setColor(ccc3(127, 127, 127));
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
		addButtonsAndReloadCurrentBar(CCArray::createWithObject(newBtn));
		Global::get().m_hasUnsavedOGChanges = true;

	} else {
		std::vector<short> ids;
		for (int i = 0; i < selected->count(); i++) {
			ids.push_back(static_cast<GameObject*>(selected->objectAtIndex(i))->m_objectID);
		}
		createQuickPopup("Object Groups", 
			fmt::format("Are you sure you want to add buttons for <cy>{}</c> objects?", selected->count()),
			"Yes", "No",
			[ids, this] (auto, bool btn2) {
				if (!btn2) {
					auto arr = CCArray::create();
					for (short id : ids) {
						auto newBtn = getCustomCreateBtn(id, 1);
						arr->addObject(newBtn);
					}
					addButtonsAndReloadCurrentBar(arr);
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
	auto btn = getSelectedCmi();
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
	auto btn = getSelectedCmi();
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
			shortAlert("Saved!", 2);
			Global::get().m_hasUnsavedOGChanges = false;
		} else if (result == -1) {
			alert(fmt::format("<cr>ERROR:</c> Can't access config file:\n{}\n\
	Configuration wasn't saved! Check that file exists and isn't locked", file.string()).c_str());
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
	int const selCount = selected->count();

	if (selCount == 0) {
		group = Group::createDefault();

	} else {
		std::vector<std::vector<short>> matrix;
		short firstId = static_cast<GameObject*>(selected->objectAtIndex(0))->m_objectID;
		int rowCount = (selCount < 5) ? selCount : ((selCount < 7 || selCount == 9) ? 3 : 4);
		int columnCount = ceil((float)selCount / (float)rowCount);

		for (int objIter = 0; objIter < selCount;) {
			std::vector<short> newRow;
			for (int j = 0; j < columnCount; j++) {
				auto obj = static_cast<GameObject*>(selected->objectAtIndex(objIter++));
				newRow.push_back(obj->m_objectID);
				if (objIter == selCount) break;
			}
			matrix.push_back(newRow);
		}
		group = Group::createGroup("New Group", firstId, std::move(matrix));
	}

	auto newBtn = group->getCmi();
	addButtonsAndReloadCurrentBar(CCArray::createWithObject(newBtn));
	Global::get().m_hasUnsavedOGChanges = true;
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
		log::debug("grid group created");
		auto newBtn = group->getCmi();
		addButtonsAndReloadCurrentBar(CCArray::createWithObject(newBtn));
		Global::get().m_hasUnsavedOGChanges = true;
	} else {
		alert("<co>Layout not detected</c>: Selected objects cannot be arranged while \
preserving their relative positions. Check that there are no multiple objects at the same spot");
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
			alert("You can not change the icon of this tab.\n\
<cl>You can only change default editor tabs and tabs added by</c> <cy>Object Groups</c>");
		}

	}
}


// if str is empty, reset to default
bool MyEditorUI::setSpiteToTabByIndexFromString(std::string objectString, CCMenuItemToggler* tab, uint8_t tabIdx) {

	if (objectString.empty()) {
		struct SprInfo {const char* name; float sc;};
		const SprInfo defaultSprites[13] = {
			{"square_01_001.png", 0.45f},
			{"blockOutline_01_001.png", 0.45f},
			{"triangle_a_02_001.png", 0.45f},
			{"spike_01_001.png", 0.45f},
			{"persp_outline_01_001.png", 0.8f},
			{"ring_01_001.png", 0.45f},
			{"GJBeast01_01_001.png", 0.346f},
			{"pixelb_03_01_001.png", 1.227f},
			{"pixelitem_001_001.png", 0.844f},
			{"particle_01_001.png", 0.844f},
			{"d_spikes_01_001.png", 0.188f},
			{"sawblade_02_001.png", 0.225f},
			{"edit_eTintCol01Btn_001.png", 0.482f},
		};
		if (tabIdx < 13) {
			auto sprInfo = defaultSprites[tabIdx];
			auto icon = CCSprite::createWithSpriteFrameName(sprInfo.name);
			icon->setScale(sprInfo.sc);
			EditorTabUtils::setTabIcon(tab, icon);
		} else {
			auto icon = CCLabelBMFont::create(std::to_string(tabIdx-13+1).c_str(), "bigFont.fnt");
			icon->setScale(0.5f);
			EditorTabUtils::setTabIcon(tab, icon);
		}
		
		return true;
	}

	auto levelLayer = LevelEditorLayer::get();
	auto arrA = CCArray::create();
	// auto arrB = CCArray::create();

	auto sprA = spriteFromObjectString(objectString, false, false, 0, arrA, nullptr, nullptr);
	// auto sprB = spriteFromObjectString(objectString, false, false, 0, arrB, nullptr, nullptr);
	// levelLayer->updateObjectColors(arr);

	for (auto* el : CCArrayExt<GameObject*>(arrA)) {
		setColorToGameObjectNew(el, true);
	}

	// for (auto* el : CCArrayExt<GameObject*>(arrB)) {
	// 	setColorToGameObjectNew(el, true);
	// }
	
	// max size is 13x26
	float vScaleRatio = 13.f / sprA->getContentHeight();
	float hScaleRatio = 26.f / sprA->getContentWidth();
	float scl = std::min(vScaleRatio, hScaleRatio);

	sprA->setScale(scl);
	// sprB->setScale(scl);

	sprA->setCascadeOpacityEnabled(true);
	sprA->setOpacity(150);

	EditorTabUtils::setTabIcon(tab, sprA);

	// auto childA = tab->m_offButton->getChildByType<CCSprite>(0);
	// auto childB = tab->m_onButton->getChildByType<CCSprite>(0);
	
	// childA->removeAllChildren();
	// childB->removeAllChildren();

	// childA->addChildAtPosition(sprA, Anchor::Center, ccp(0, -1));
	// childB->addChildAtPosition(sprB, Anchor::Center, ccp(0, -1));

	return true;
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
			setSelectedCmi(btn);
		}
	}
	
	m_createButtonBar->loadFromItems(m_createButtonBar->m_buttonArray, cols, rows, true);

	// preserve the page
	if (currentPage > 0) {
		m_createButtonBar->m_scrollLayer->instantMoveToPage(currentPage - 1);
		m_createButtonBar->m_scrollLayer->instantMoveToPage(currentPage);
	}
}

