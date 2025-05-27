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


// keybinds
#ifdef GEODE_IS_DESKTOP
	#include <geode.custom-keybinds/include/Keybinds.hpp>
	$execute {
		keybinds::BindManager::get()->registerBindable({
			"goto-object"_spr, "Find Object",
			"Find and highlight selected object in the build tab",
			{ keybinds::Keybind::create(KEY_F, keybinds::Modifier::Control) },
			"Object Groups"
		});
	}
#endif // GEODE_IS_DESKTOP


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


inline void pinnedObjectsByViperGoofyAhhFix() {
	// Object Pinning by Viper contains a game-crashing bug related to the custom objects. 
	// Viper doesn't respond to me anywhere and doesn't accept my PR on GH.
	// So to keep mods compatible I have no choice other than edit saved values of that mod 
	// before it uses them to prevent mod from executing game-crashing code 

	if (!Loader::get()->isModInstalled("viper.object_pinning")) return;
	auto objPinning = Loader::get()->getInstalledMod("viper.object_pinning");
	if (objPinning->getVersion() != VersionInfo(1, 0, 3)) return; // only v1.0.3 

	auto savedDataJson = objPinning->getSavedValue<std::string>("Pinned-Items");
	std::map<std::string, bool> validIds;
	auto gm = GameManager::get();

	for (const auto &pair : matjson::parse(savedDataJson).unwrapOrDefault()) {
		if (auto str = pair.getKey()) {
			int id = std::atoi(str->c_str());
			if (id > 0 || !gm->stringForCustomObject(id).empty()) {
				validIds.insert({*str, true});
			}
		}
	}
	objPinning->setSavedValue("Pinned-Items", matjson::Value(validIds).dump(0));
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
	if (m_fields->buildTabRightMenu) {
		if (auto children = m_fields->buildTabRightMenu->getChildren()) {
			for (int i = 0; i < children->count(); i++) {
				auto btn = static_cast<CCNode*>(children->objectAtIndex(i));
				btn->setVisible(show);
			}
		}
	}
	if (m_fields->buildTabLeftMenu) {
		if (auto children = m_fields->buildTabLeftMenu->getChildren()) {
			for (int i = 0; i < children->count(); i++) {
				auto btn = static_cast<CCNode*>(children->objectAtIndex(i));
				btn->setVisible(show);
			}
		}
	}
	if (m_fields->pinnedGroups) {
		m_fields->pinnedGroups->setVisible(show ? m_selectedMode == 2 : false);
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
	
	// other mods
	pinnedObjectsByViperGoofyAhhFix();
	const float scale = getBetterEditInterfaceScale();
	const bool isNewTabUI = isCreativeModeNewTabUI();

	if (fileOk) {
		// re-setup tabs
		setupVanillaTabs();
		std::string tmp;
		std::set<uint8_t> tabs;
		std::istringstream ss(Global::get().m_settings.m_extraTabs);
		while (std::getline(ss, tmp, ',')) {
			tabs.insert(std::atoi(tmp.c_str()));
		}
		setupExtraTabs(tabs);
		setupSearchTab();
		m_fields->rowMenu = setupRowMenu(scale);
		m_fields->buildTabRightMenu = setupRightMenu(scale);
		m_fields->buildTabLeftMenu = setupLeftMenu(scale);
	}

	// frame
	auto frame = CCSprite::create("OG_button_frame.png"_spr);
	frame->setAnchorPoint({0,0});
	frame->setColor(isNewTabUI ? ccc3(0, 255, 255) : ccc3(255, 255, 0));
	m_fields->buttonFrame = CCNode::create();
	m_fields->buttonFrame->addChild(frame);
	m_fields->buttonFrame->setID("frame"_spr);

	// pin layer
	m_fields->pinnedGroups = CCNode::create();
	m_fields->pinnedGroups->setID("pinned-groups"_spr);
	addChild(m_fields->pinnedGroups, 15);

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

	// keybinds
	#ifdef GEODE_IS_DESKTOP
		this->template addEventListener<keybinds::InvokeBindFilter>([this](keybinds::InvokeBindEvent* event) {
			static bool isHolding = false;
			if (event->isDown()) {
				if (!isHolding) {
					auto selected = getSelectedObjects();
					if (auto obj = static_cast<GameObject*>(selected->firstObject())) {
						goToObject(obj->m_objectID, false);
					}
				}
				isHolding = true;
			} else {
				isHolding = false;
			}
			return ListenerResult::Propagate;
		}, "goto-object"_spr);
	#endif // GEODE_IS_DESKTOP

	return true;
}


void MyEditorUI::toggleMode(CCObject* sender) {
	EditorUI::toggleMode(sender);
	if (auto menu = m_fields->rowMenu) {
		menu->setVisible(m_selectedMode == 2 && Global::get().m_isEditMode);
		m_fields->buildTabRightMenu->setVisible(m_selectedMode == 2);
		m_fields->buildTabLeftMenu->setVisible(m_selectedMode == 2);
	}
	if (m_fields->pinnedGroups) {
		m_fields->pinnedGroups->setVisible(m_selectedMode == 2);
	}
}


void MyEditorUI::updateCreateMenu(bool p0) {
	EditorUI::updateCreateMenu(p0);

	// darken all buttons with the selected object 
	// (as we now can have more buttons of type than 1)
	if (m_selectedObjectIndex != 0) {
		for (auto* btn : CCArrayExt<CreateMenuItem*>(m_createButtonArray)) {
			if (btn->m_objectID == m_selectedObjectIndex) {
				setColorToCreateBtnNew(btn, false);
			}
		}
		if (p0) { // goto object
			goToObject(m_selectedObjectIndex, false);
		}
	}
}


void MyEditorUI::onCreateButton(CCObject* sender) {
	auto cmi = static_cast<CreateMenuItem*>(sender);

	#ifdef GEODE_IS_DESKTOP
		if (Global::get().m_isEditMode && Global::get().m_settings.m_shiftAdd 
					&& CCKeyboardDispatcher::get()->getShiftKeyPressed()) {
			addItemToActiveGroupByCmi(cmi);
			return;
		}
	#endif /* GEODE_IS_DESKTOP */

	int indexBefore = m_selectedObjectIndex;
	EditorUI::onCreateButton(sender);
	
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
// 		static_cast<MyCreateMenuItem*>(ret)->m_fields->a++;
// 		return ret;
// 	}
// };