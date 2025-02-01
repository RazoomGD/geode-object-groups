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

		auto btn1 = CCMenuItemSpriteExtra::create(
			ButtonSprite::create("Button\n1"), this, 
			menu_selector(MyEditorUI::onButton1)
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

		rowMenu->addChild(btn1);
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

	void toggleEditGroupsMode(CCObject*) {
		m_fields->isEditGroupsMode = !m_fields->isEditGroupsMode;
		if (!m_fields->isEditGroupsMode) {
			m_fields->rowMenu->setVisible(false);
		} else if (m_selectedMode == 2) { // 2 -  build mode
			m_fields->rowMenu->setVisible(true);
		}
	}

	void onButton1(CCObject*) {
		log::debug("button 1");
		log::debug("mode {}", m_selectedMode);
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