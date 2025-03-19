#include "EditorUI.hpp"
#include "UpdateNotificationManager.hpp"
// #include <geode.custom-keybinds/include/Keybinds.hpp>


// keybinds
// $execute {
//     keybinds::BindManager::get()->registerBindable({
//         "toggle-search"_spr,
//         "Toggle Group Search",
//         "Open group search text field",
//         { keybinds::Keybind::create(KEY_F, keybinds::Modifier::Control | keybinds::Modifier::Shift) },
//         "Object Groups"
//     });
// }


// support for BetterEdit scale factor
inline float getBetterEditInterfaceScale() {
	if (Loader::get()->isModInstalled("hjfod.betteredit")) {
		auto betterEdit = Loader::get()->getInstalledMod("hjfod.betteredit");
		if (betterEdit->isEnabled() && betterEdit->hasSetting("scale-factor")) {
			float scale = betterEdit->getSettingValue<double>("scale-factor");
			if (scale > 0.1) return scale;
		}
	}
	return 1;
}


// support for Creative Mode new tab UI
inline bool isCreativeModeNewTabUI() {
	if (Loader::get()->isModInstalled("alphalaneous.creative_mode")) {
		auto creativeMode = Loader::get()->getInstalledMod("alphalaneous.creative_mode");
		if (creativeMode->isEnabled() && creativeMode->hasSetting("enable-new-tab-ui")) {
			return creativeMode->getSettingValue<bool>("enable-new-tab-ui");
		}
	}
	return false;
}


void MyEditorUI::showUI(bool show) {
	EditorUI::showUI(show);
	if (m_fields->rowMenu) {
		if (auto children = m_fields->rowMenu->getChildren()) {
			for (int i = 0; i < children->count(); i++) {
				auto btn = static_cast<CCNode*>(children->objectAtIndex(i));
				btn->setVisible(show);
			}
		}
	}
	if (m_fields->toggleMenu) {
		if (auto children = m_fields->toggleMenu->getChildren()) {
			for (int i = 0; i < children->count(); i++) {
				auto btn = static_cast<CCNode*>(children->objectAtIndex(i));
				btn->setVisible(show);
			}
		}
	}
}

// CreateMenuItem* MyEditorUI::getCreateBtn(int id, int bg) {
// 	log::debug("call {}", id);
// 	auto ret = EditorUI::getCreateBtn(id, bg);
// 	log::debug("ret");
// 	return ret;
// }

bool MyEditorUI::init(LevelEditorLayer* editorLayer) {
	Global::get().m_editorUI = this;
	Global::get().m_isEditMode = false;
	Global::get().m_hasUnsavedOGChanges = false;
	Global::get().m_settings.update();

	for (int i = 0; i < m_fields->GROUPS.size(); i++) {
		m_fields->GROUPS[i] = nullptr;
	}
	
	if (!EditorUI::init(editorLayer)) return false;

	// try to load saved configuration
	auto file = Mod::get()->getConfigDir(true).append("OGv2_config.json");
	bool fileOk = false;
	switch (readConfigFromJson(file.string())) {
		case 0: {fileOk = true; break;}; // ok
		case -1: { // file error
			callAfterTransition([](){
				alert("<cr>ERROR</c>: couldn't load <cy>Object Groups</c> "
					"configuration because of file error");
			});
			break;
		}
		case -2: { // json error
			callAfterTransition([](){
				alert("<cr>ERROR</c>: couldn't load <cy>Object Groups</c> "
					"configuration because of JSON format error");
			});
			break;
		}
		default: break;
	}

	// prevent overlapping with my menus
	getChildByID("build-tabs-menu")->setZOrder(6); 
	getChildByID("editor-buttons-menu")->setZOrder(6);
	getChildByID("layer-menu")->setZOrder(6);

	const float scale = getBetterEditInterfaceScale();
	const bool isNewTabUI = isCreativeModeNewTabUI();

	if (fileOk) {
		setupVanillaTabs();
		setupExtraTabs(Global::get().m_settings.m_extraTabsCount);
		setupSearchTab();
		m_fields->rowMenu = setupRowMenu(scale);
		m_fields->toggleMenu = setupToggleMenu(scale);
	}

	auto frame = CCSprite::create("OG_button_frame.png"_spr);
	frame->setAnchorPoint({0,0});
	frame->setColor(isNewTabUI ? ccc3(0, 255, 255) : ccc3(255, 255, 0));
	m_fields->buttonFrame = CCNode::create();
	m_fields->buttonFrame->addChild(frame);
	m_fields->buttonFrame->setID("frame"_spr);

	if (fileOk) {
		toggleEditGroupsMode(nullptr);
		toggleEditGroupsMode(nullptr);
	}

	if (Global::get().m_isFirstEditorEnter || !Global::get().m_isCurrentVersionSafe) {
		// notify user about an important update
		UpdateNotificationManager::get()->goodMorning();
		Global::get().m_isFirstEditorEnter = false;
	}

	if (isDeveloperMode()) {
		log::info("Developer mode enabled");
	}

	return true;
}


void MyEditorUI::toggleMode(CCObject* sender) {
	EditorUI::toggleMode(sender);
	if (auto menu = m_fields->rowMenu) {
		menu->setVisible(m_selectedMode == 2 && Global::get().m_isEditMode);
		m_fields->toggleMenu->setVisible(m_selectedMode == 2);
	}
}


void MyEditorUI::updateCreateMenu(bool p0) {
	EditorUI::updateCreateMenu(p0);
	
	// darken all buttons with the selected object 
	// (as we now can have more buttons of type than 1)
	int indx = m_selectedObjectIndex;
	if (indx != 0) {
		for (auto* btn : CCArrayExt<CreateMenuItem*>(m_createButtonArray)) {
			if (btn->m_objectID == indx) {
				setColorToCreateBtnNew(btn, false);
			}
		}
	}
}


void MyEditorUI::onCreateButton(CCObject* sender) {
	int indexBefore = m_selectedObjectIndex;
	EditorUI::onCreateButton(sender);
	
	auto cmi = static_cast<CreateMenuItem*>(sender);
	if (cmi && m_selectedObjectIndex == cmi->m_objectID) {
		setNewFocusedCmi(cmi);
	} else {
		setNewFocusedCmi(nullptr);
	}

	if (indexBefore != m_selectedObjectIndex) {
		setNewSelectedGroupCmi(nullptr); // deselect
	}

	if (!Global::get().m_isEditMode) {
		if (Global::get().m_settings.m_autoClose) {
			setNewOpenedGroup(nullptr, nullptr); // close
		}
	}

}

// todo: delete this

// #include <Geode/modify/CreateMenuItem.hpp>

// static int aaaaa = 0;

// class $modify(MyCreateMenuItem, CreateMenuItem) {
// 	struct Fields {
// 		int a;
// 		Fields() {
// 			log::debug("CMI+ {}", ++aaaaa);
// 		}
// 		~Fields() {
// 			log::debug("CMI- {}", --aaaaa);
// 		}
// 	};

// 	static CreateMenuItem* create(CCNode* p0, CCNode* p1, CCObject* p2, SEL_MenuHandler p3) {
// 		CreateMenuItem* ret = CreateMenuItem::create(p0, p1, p2, p3);
// 		static_cast<MyCreateMenuItem*>(ret)->m_fields->a ++;
// 		return ret;
// 	}
// };