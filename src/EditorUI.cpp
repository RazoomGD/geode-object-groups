#include "ObjectGroups.hpp"


// mathematically correct a % b
inline int mod(int a, int b) {return (a % b + b) % b;}


class $modify(MyEditorUI, EditorUI) {
	struct Fields {
		Ref<CCMenu> rowMenu = nullptr;
		Ref<CCMenu> toggleMenu = nullptr;
		bool isEditGroupsMode = false;
		Ref<CCNode> buttonFrame = nullptr;

		Fields() {
			// init global config and load data from json
			for (int i = 0; i < Global::get().m_groups.size(); i++) {
				Global::get().m_groups[i] = CCArray::create();
			}
			auto file = Mod::get()->getResourcesDir().append("OGv2_config.json");
			readConfigFromJson(file.string()); // todo: move the file to save directory
		}
		~Fields() {
			for (int i = 0; i < Global::get().m_groups.size(); i++) {
				Global::get().m_groups[i] = nullptr;
			}
		}
	};

	inline void alert(const char* text) {
		FLAlertLayer::create("Object Groups", text, "Ok")->show();
	}


	// support for BetterEdit scale factor
	float getBetterEditInterfaceScale() {
		if (Loader::get()->isModInstalled("hjfod.betteredit")) {
			auto betterEdit = Loader::get()->getInstalledMod("hjfod.betteredit");
			if (betterEdit->isEnabled() && betterEdit->hasSetting("scale-factor")) {
				float scale = betterEdit->getSettingValue<double>("scale-factor");
				if (scale > 0.1) return scale;
			}
		}
		return 1;
	}

	CCMenu* setupRowMenu(float scale) {
		const auto rowMenu = CCMenu::create();
		this->addChild(rowMenu);
		rowMenu->setAnchorPoint({0.5, 0});
		rowMenu->setLayout(RowLayout::create());
		rowMenu->setPosition(ccp(CCDirector::get()->getWinSize().width / 2, 111 * scale));
		rowMenu->setScale(scale * 0.5);
		rowMenu->setID("razoom.object_groups.row_menu");

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
			ButtonSprite::create("Save\n "), this, 
			menu_selector(MyEditorUI::onSaveButton)
		);
		auto newGroupBtn = CCMenuItemSpriteExtra::create(
			ButtonSprite::create("New\ngroup"), this, 
			menu_selector(MyEditorUI::onNewGroupButton)
		);

		rowMenu->addChild(newObjectBtn);
		rowMenu->addChild(moveForwardBtn);
		rowMenu->addChild(moveBackwardBtn);
		rowMenu->addChild(saveMeBtn);
		rowMenu->addChild(newGroupBtn);
		
		rowMenu->updateLayout();

		return rowMenu;
	}

	CCMenu* setupToggleMenu(float scale) {
		auto tMenu = CCMenu::create(); 
		auto tBtn = CCMenuItemSpriteExtra::create(
			ButtonSprite::create("T"), this, 
			menu_selector(MyEditorUI::toggleEditGroupsMode)
		);
		tMenu->addChild(tBtn);
		this->addChild(tMenu);
		tMenu->setAnchorPoint({1,0});
		auto layout = RowLayout::create();
		layout->setAxisAlignment(AxisAlignment::End);
		tMenu->setLayout(layout);
		tMenu->setPosition(ccp(CCDirector::get()->getWinSize().width - 100 * scale, 3));
		tMenu->setScale(scale * 0.9);
		tMenu->setContentWidth(100);
		tMenu->setZOrder(2);
		tMenu->updateLayout();
		tMenu->setID("razoom.object_groups.toggle_menu");
		return tMenu;
	}

	void setupExtraTabs(int count) {
		for (int i = 0; i < count; i++) {
			EditorTabs::addTab(this, TabType::BUILD, fmt::format("extra-tab-{}", i+1),
				// is called once on first create
				[=](EditorUI* ui, CCMenuItemToggler* toggler) -> CCNode* {
					auto icon = CCLabelBMFont::create(std::to_string(i+1).c_str(), "bigFont.fnt");
					icon->setScale(0.5f);
					EditorTabUtils::setTabIcon(toggler, icon);

					auto ret = EditorTabUtils::createEditButtonBar(CCArray::create(), ui);

					int rows, cols;
					getBarSize(&rows, &cols);

					// set user obj and call my hook
					ret->setUserObject(BAR_USER_OBJ_ID, new BarInfo(13+i, false));
					ret->loadFromItems(ret->m_buttonArray, cols, rows, true);
					return ret;
				},
				// is called on every tab click
				[](EditorUI*, bool state, CCNode*) {
					// log::info("tab toggle");
				}
			);
		}
	}

	$override
	bool init(LevelEditorLayer* editorLayer) {
		Global::get().m_editorUI = this;
		Global::get().m_settings.update();
		
		if (!EditorUI::init(editorLayer)) return false;

		// prevent overlapping with my menus
		if (auto ch = this->getChildByID("build-tabs-menu")) ch->setZOrder(6); 
		if (auto ch = this->getChildByID("editor-buttons-menu")) ch->setZOrder(6);
		if (auto ch = this->getChildByID("layer-menu")) ch->setZOrder(6);

		setupExtraTabs(Global::get().m_settings.m_extraTabsCount);

		const float scale = getBetterEditInterfaceScale();

		m_fields->rowMenu = setupRowMenu(scale);
		m_fields->toggleMenu = setupToggleMenu(scale);

		auto frame = CCSprite::create("OG_button_frame.png"_spr);
		frame->setAnchorPoint({0,0});
		m_fields->buttonFrame = CCNode::create();
		m_fields->buttonFrame->addChild(frame);
		m_fields->buttonFrame->setID("razoom.object-groups.frame");

		toggleEditGroupsMode(nullptr);
		// toggleEditGroupsMode(nullptr); // todo: uncomment so that edit mode was disabled by default 

		return true;
	}

	$override
	void toggleMode(CCObject* sender) {
		EditorUI::toggleMode(sender);
		if (auto menu = m_fields->rowMenu) {
			menu->setVisible(sender == m_buildModeBtn && m_fields->isEditGroupsMode);
			m_fields->toggleMenu->setVisible(sender == m_buildModeBtn);
		}
	}

	$override
	void updateCreateMenu(bool p0) {
		EditorUI::updateCreateMenu(p0);
		
		// darken all buttons with the selected object 
		// (as we now can have more buttons of type than 1)
		int indx = m_selectedObjectIndex;
		if (indx > 0) {
			CCArrayExt<CreateMenuItem*> buttons = m_createButtonArray;
			for (auto* btn : buttons) {
				if (btn->m_objectID == indx) {
					// setColorToCreateBtn(btn, ccc3(127, 127, 127));
					setColorToCreateBtnNew(btn, false);
				}
			}
		}
	}

	// helper function that sets a frame to given cmi (cmi can be nullptr)
	void setSelectedCmi(CreateMenuItem* cmi) {
		m_fields->buttonFrame->removeFromParent();
		if (cmi) cmi->addChild(m_fields->buttonFrame);
	}

	CreateMenuItem* getSelectedCmi() {
		return typeinfo_cast<CreateMenuItem*>(m_fields->buttonFrame->getParent());
	}

	$override
	void onCreateButton(CCObject* sender) {
		EditorUI::onCreateButton(sender);
		auto cmi = typeinfo_cast<CreateMenuItem*>(sender);
		if (cmi && m_selectedObjectIndex == cmi->m_objectID) {
			setSelectedCmi(cmi);
		} else {
			setSelectedCmi(nullptr);
		}
	}

	void toggleEditGroupsMode(CCObject*) {
		m_fields->isEditGroupsMode = !m_fields->isEditGroupsMode;
		if (!m_fields->isEditGroupsMode) {
			m_fields->rowMenu->setVisible(false);
		} else if (m_selectedMode == 2) { // 2 -  build mode
			m_fields->rowMenu->setVisible(true);
		}
	}

	// helper function to find out whether my tab is opened now
	inline bool isMyTab(EditButtonBar* tab) {
		return tab->getUserObject(BAR_USER_OBJ_ID) != nullptr;
	}


	// ------------------------------- handlers of rowMenu ------------------------------- 
	void onNewObjectButton(CCObject*) {
		
		// get selected object
		if (!m_selectedObject) {
			int selCount = m_selectedObjects ? m_selectedObjects->count() : 0;
			alert(fmt::format("You must select exactly <cy>one</c> object to create \
new object button.\n(now selected: <cy>{}</c>)", selCount).c_str());
			return;
		}

		// make sure this is my tab (current bar is editor->m_createButtonBar)
		if (!isMyTab(m_createButtonBar)) {
			alert("Can't create an object in this tab");
			return;
		}

		int newObjId = m_selectedObject->m_objectID;
		auto newBtn = getCustomCreateBtn(newObjId, 1);

		// create item on EditButtonBar for this obj
		int currentPage = mod(m_createButtonBar->m_scrollLayer->m_page, 
								m_createButtonBar->m_scrollLayer->getTotalPages());

		int rows, cols;
		getBarSize(&rows, &cols);
		int firstIndex = cols * rows * currentPage; // index the first obj on current page

		m_createButtonBar->m_buttonArray->insertObject(newBtn, firstIndex);
		m_createButtonBar->loadFromItems(m_createButtonBar->m_buttonArray, cols, rows, true);

		// preserve the page
		if (currentPage > 0) {
			m_createButtonBar->m_scrollLayer->instantMoveToPage(currentPage - 1);
			m_createButtonBar->m_scrollLayer->instantMoveToPage(currentPage);
		}

		// set frame to newly created button
		if (m_selectedObjectIndex == newObjId) {
			// setColorToCreateBtn(newBtn, ccc3(127, 127, 127));
			setColorToCreateBtnNew(newBtn, false);
			setSelectedCmi(newBtn);
		} else {
			onCreateButton(newBtn);
		}
	}

	void onMoveForwardButton(CCObject*) {
		moveSelectedButton(true);
	}

	void onMoveBackwardButton(CCObject*) {
		moveSelectedButton(false);
	}

	// move selected button in create editButtonBar forward or backward
	void moveSelectedButton(bool forward) {
		// make sure this is my tab
		if (!isMyTab(m_createButtonBar)) {
			alert("You can't edit this tab");
			return;
		}

		auto btn = getSelectedCmi();

		// make sure the button is selected
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
					(!forward && index == 0)) {
			return;
		}
		m_createButtonBar->m_buttonArray->exchangeObjectAtIndex(index, index + (forward ? 1 : -1));

		int rows, cols;
		getBarSize(&rows, &cols);

		int currentPage = mod(m_createButtonBar->m_scrollLayer->m_page, 
			m_createButtonBar->m_scrollLayer->getTotalPages());
		m_createButtonBar->loadFromItems(m_createButtonBar->m_buttonArray, cols, rows, true);

		// preserve the page
		if (currentPage > 0) {
			m_createButtonBar->m_scrollLayer->instantMoveToPage(currentPage - 1);
			m_createButtonBar->m_scrollLayer->instantMoveToPage(currentPage);
		}
	}

	void onSaveButton(CCObject*) {
		// auto file = Mod::get()->getResourcesDir().append("OGv2_config.json");
		// // update arrays of groups first
		// for (int i = 0; i < m_createButtonBars->count(); i++) {
		// 	auto bar = static_cast<EditButtonBar*>(m_createButtonBars->objectAtIndex(i));
		// 	// is my bar
		// 	if (auto obj = static_cast<BarInfo*>(bar->getUserObject(BAR_USER_OBJ_ID))) {
		// 		auto myArray = Global::get().m_groups[obj->m_tabIndx].data();
		// 		// myArray contains objects from previous load. Buttons may be added or deleted from that time
		// 		myArray->removeAllObjects();
		// 		for (int j = 0; j < bar->m_buttonArray->count(); j++) {
		// 			auto cmi = typeinfo_cast<CreateMenuItem*>(bar->m_buttonArray->objectAtIndex(j));
		// 			if (cmi == nullptr || cmi->m_objectID <= 0) continue;
		// 			if (auto btnInfo = static_cast<BtnInfo*>(bar->getUserObject(CMI_USER_OBJ_ID))) {
						
		// 			}
		// 		}
		// 	}
		// }
		// writeConfigToJson(file.string());
	}

	void onNewGroupButton(CCObject*) {

		// make sure this is my tab (current bar is editor->m_createButtonBar)
		if (!isMyTab(m_createButtonBar)) {
			alert("You can not create a group in this tab");
			return;
		}

		Group* group;
		auto const selected = getSelectedObjects();
		int const selectedCount = selected->count();

		if (selectedCount == 0) {
			group = Group::createDefault();
			auto newBtn = group->getCmi();
			addButtonsToCurrentBar(CCArray::createWithObject(newBtn));
		} else {
			std::vector<std::vector<short>> res;
			bool success = divideGridAlignedObjects(selected, &res);

			if (success) {
				short id = res[0][0];
				group = Group::createGroup("new group", id, std::move(res));

			} else {
				int rowCount = selectedCount;
				rowCount = (rowCount < 5) ? rowCount : (
					(rowCount < 7 || rowCount == 9) ? 3 : 4
				);
				int columnCount = ceil((float)selectedCount / (float)rowCount);

				res.clear();

				int objIter = 0;
				for (int i = 0;; i++) {
					res.push_back({});
					for (int j = 0; j < columnCount; j++) {
						auto obj = static_cast<GameObject*>(selected->objectAtIndex(objIter++));
						res[i].push_back(obj->m_objectID);
						if (objIter == selectedCount) break;
					}
					if (objIter == selectedCount) break;
				}

				short id = res[0][0];
				group = Group::createGroup("new group", id, std::move(res));
			}
		}



		// select and set frame to newly created button
		// if (m_selectedObjectIndex == newObjId) {
		// 	setColorToCreateBtn(newBtn, ccc3(127, 127, 127));
		// 	setSelectedCmi(newBtn);
		// } else {
		// 	onCreateButton(newBtn);
		// }
	

	}

	void addButtonsToCurrentBar(CCArrayExt<CreateMenuItem*> buttons) {
		int currentPage = mod(m_createButtonBar->m_scrollLayer->m_page, 
			m_createButtonBar->m_scrollLayer->getTotalPages());

		int rows, cols;
		getBarSize(&rows, &cols);
		int firstIndex = cols * rows * currentPage; // index the first obj on current page

		for (auto* btn : buttons) {
			m_createButtonBar->m_buttonArray->insertObject(btn, firstIndex);
			firstIndex++;
		}

		m_createButtonBar->loadFromItems(m_createButtonBar->m_buttonArray, cols, rows, true);

		// preserve the page
		if (currentPage > 0) {
			m_createButtonBar->m_scrollLayer->instantMoveToPage(currentPage - 1);
			m_createButtonBar->m_scrollLayer->instantMoveToPage(currentPage);
		}
	}
};

// class $modify(BoomScrollLayer) {
// 	void instantMoveToPage(int p0) {
// 		log::debug("insta move {}", p0);
// 		BoomScrollLayer::instantMoveToPage(p0);
// 	}
// };