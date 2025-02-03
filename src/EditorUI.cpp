#include "ObjectGroups.hpp"



class $modify(MyEditorUI, EditorUI) {
	struct Fields {
		Ref<CCMenu> rowMenu = nullptr;
		Ref<CCMenu> toggleMenu = nullptr;
		bool isEditGroupsMode = false;
	};

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

		auto newEmptyGroupBtn = CCMenuItemSpriteExtra::create(
			ButtonSprite::create("New\nobject"), this, 
			menu_selector(MyEditorUI::onNewObjectButton)
		);
		auto btn2 = CCMenuItemSpriteExtra::create(
			ButtonSprite::create("Button\n2"), this, 
			menu_selector(MyEditorUI::onButton2)
		);
		auto btn3 = CCMenuItemSpriteExtra::create(
			ButtonSprite::create("Button\n3"), this, 
			menu_selector(MyEditorUI::onButton3)
		);
		auto btn4 = CCMenuItemSpriteExtra::create(
			ButtonSprite::create("Button\n4"), this, 
			menu_selector(MyEditorUI::onButton4)
		);

		rowMenu->addChild(newEmptyGroupBtn);
		rowMenu->addChild(btn2);
		rowMenu->addChild(btn3);
		rowMenu->addChild(btn4);
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

	$override
	bool init(LevelEditorLayer* editorLayer) {
		Global::get().m_editorUI = this;
		Global::get().m_controlledBars.fill(nullptr);
		Global::get().m_settings.update();
		
		if (!EditorUI::init(editorLayer)) return false;

		const float scale = getBetterEditInterfaceScale();

		m_fields->rowMenu = setupRowMenu(scale);
		m_fields->toggleMenu = setupToggleMenu(scale);

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
		int oldId = m_selectedObjectIndex;
		EditorUI::updateCreateMenu(p0);
		int newId = m_selectedObjectIndex;
		if (newId == oldId) return;
		
		CCArrayExt<CreateMenuItem*> buttons = m_createButtonArray;

		// darken all buttons with the selected object 
		// (as we now can have more than 1)
		if (newId > 0) {
			for (auto* btn : buttons) {
				if (btn->m_objectID == newId) {
					setColorToCreateBtn(btn, ccc3(127, 127, 127));
				}
			}
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

	// handlers of rowMenu
	void onNewObjectButton(CCObject*) {
		// make sure this is my tab
		int tabIndex = m_createButtonBar->m_tabIndex; // current bar is editor->m_createButtonBar
		if (tabIndex < 0 || tabIndex >= Global::get().m_controlledBars.size() || 
					Global::get().m_controlledBars[tabIndex] != m_createButtonBar) {
			FLAlertLayer::create("Object Groups", "Can't create an object in this tab", "Ok")->show();
			return;
		}
		
		// get selected object
		if (!m_selectedObject) {
			int selCount = m_selectedObjects ? m_selectedObjects->count() : 0;
			FLAlertLayer::create("Object Groups", 
				fmt::format("You must select exactly <cy>one</c> object to create \
new object button.\n(now selected: <cy>{}</c>)", selCount), "Ok"
			)->show();
			return;
		}

		int newObjId = m_selectedObject->m_objectID;
		auto newBtn = getCustomCreateBtn(newObjId, 1);

		// create item on EditButtonBar for this obj
		int currentPage = mod(m_createButtonBar->m_scrollLayer->m_page, 
								m_createButtonBar->m_scrollLayer->getTotalPages());

		int cols = GameManager::sharedState()->getIntGameVariable("0049");
		int rows = GameManager::sharedState()->getIntGameVariable("0050");
		int firstIndex = cols * rows * currentPage; // index the first obj on current page

		m_createButtonBar->m_buttonArray->insertObject(newBtn, firstIndex);
		m_createButtonBar->loadFromItems(m_createButtonBar->m_buttonArray, cols, rows, true);

		// preserve the page
		if (currentPage > 0) {
			m_createButtonBar->m_scrollLayer->instantMoveToPage(currentPage - 1);
			m_createButtonBar->m_scrollLayer->instantMoveToPage(currentPage);
		}

		if (m_selectedObjectIndex == newObjId) {
			setColorToCreateBtn(newBtn, ccc3(127, 127, 127));
		}

		
	}

	void onButton2(CCObject*) {
		log::debug("button 2");
		
	}

	void onButton3(CCObject*) {
		log::debug("button 3");
	}

	void onButton4(CCObject*) {
		log::debug("button 4");
	}
};

// class $modify(BoomScrollLayer) {
// 	void instantMoveToPage(int p0) {
// 		log::debug("insta move {}", p0);
// 		BoomScrollLayer::instantMoveToPage(p0);
// 	}
// };