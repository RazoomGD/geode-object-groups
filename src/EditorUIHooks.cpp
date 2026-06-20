#include "EditorUI.hpp"
#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>
#include <alphalaneous.tinker/include/ObjectTooltips.hpp>
#include <alphalaneous.tinker/include/UIScaling.hpp>

// support for Creative Mode new tab UI
// inline bool isCreativeModeNewTabUI() {
// 	if (Loader::get()->isModInstalled("alphalaneous.creative_mode")) {
// 		auto creativeMode = Loader::get()->getInstalledMod("alphalaneous.creative_mode");
// 		if (creativeMode->isLoaded() && creativeMode->hasSetting("enable-new-tab-ui")) {
// 			return creativeMode->getSettingValue<bool>("enable-new-tab-ui");
// 		}
// 	}
// 	return false;
// }


// // ! Note: that this is called before EditorUI::init
// inline void pinnedObjectsByViperGoofyAhhFix() {
// 	// Object Pinning by Viper contains a game-crashing bug related to the custom objects. 
// 	// Viper doesn't respond to me anywhere and doesn't accept my PR on GH.
// 	// So to keep mods compatible I have no choice other than edit saved values of that mod 
// 	// before it uses them to prevent mod from executing game-crashing code 

// 	if (!Loader::get()->isModInstalled("viper.object_pinning")) return;
// 	auto objPinning = Loader::get()->getInstalledMod("viper.object_pinning");
// 	if (objPinning->getVersion() != VersionInfo(1, 0, 3)) return; // only v1.0.3 

// 	auto savedDataJson = objPinning->getSavedValue<std::string>("Pinned-Items");
// 	std::map<std::string, bool> validIds;
// 	auto gm = GameManager::get();

// 	for (const auto &pair : matjson::parse(savedDataJson).unwrapOrDefault()) {
// 		if (auto str = pair.getKey()) {
// 			int id = std::atoi(str->c_str());
// 			if (id > 0 || !gm->stringForCustomObject(id).empty()) {
// 				validIds.insert({*str, true});
// 			}
// 		}
// 	}
// 	objPinning->setSavedValue("Pinned-Items", matjson::Value(validIds).dump(0));
// }



void MyEditorUI::showUI(bool show) {
	EditorUI::showUI(show);
	CCNode* menus[] = {m_fields->rowMenu, m_fields->rowMenu2, m_fields->buildTabRightMenu, m_fields->buildTabLeftMenu};
	for (auto menu : menus) {
		if (!menu) continue;
		for (auto ch : menu->getChildrenExt()) {
			ch->setVisible(show);
		}
	}
	if (m_fields->pinnedGroupsNode) {
		m_fields->pinnedGroupsNode->setVisible(show ? m_selectedMode == 2 : false);
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

	// that fix
	// pinnedObjectsByViperGoofyAhhFix();

	if (!EditorUI::init(editorLayer)) return false; // ! init 

	// prevent overlapping with my menus
	getChildByID("build-tabs-menu")->setZOrder(6); 
	getChildByID("editor-buttons-menu")->setZOrder(6);
	getChildByID("layer-menu")->setZOrder(6);
	
	// other mods
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
		setupRowMenu();
		setupRightMenu();
		setupLeftMenu();
	}

	// frame
	auto frame = CCSprite::create("OG_button_frame.png"_spr);
	// frame->setAnchorPoint({0,0}); 
	// frame->setColor(isNewTabUI ? ccc3(0, 255, 255) : ccc3(255, 255, 0));
	frame->setColor(ccc3(255, 255, 0));
	m_fields->buttonFrame = CCNode::create();
	m_fields->buttonFrame->addChild(frame);
	m_fields->buttonFrame->setID("frame"_spr);

	// pin layer
	m_fields->pinnedGroupsNode = CCNodeRGBA::create();
	m_fields->pinnedGroupsNode->setID("pinned-groups"_spr);
	addChild(m_fields->pinnedGroupsNode, 15);

	if (fileOk) {
		onToggleEditGroupsMode(nullptr);
		onToggleEditGroupsMode(nullptr);
	}

	Global::editor()->updateGroupUIDs();

	// keep pinned groups
	if (Global::get().m_settings.m_keepPinned) {
		Global::editor()->execForeachGroup(
			[&states = Global::get().m_pinnedGroupsStates, pinLayer = m_fields->pinnedGroupsNode] (Group* g, auto) {
				if (!g->isSingle() && states.find(g->getGroupUID()) != states.end()) {
					g->changeGroupState(GroupState::PINNED);
					g->setPosition(pinLayer->convertToNodeSpace(states[g->getGroupUID()]));
				}
			}
		);
	}
	Global::get().m_pinnedGroupsStates.clear();

	// add toggleMode() callback because EditorTabAPI breaks this hook
	alpha::editor_tabs::addModeSwitchCallback([this](auto id){
		bool isBuildMode = id == alpha::editor_tabs::BUILD;
		if (auto menu = m_fields->rowMenu) {
			menu->setVisible(isBuildMode && Global::get().m_isEditMode);
			m_fields->rowMenu2->setVisible(false);
			m_fields->buildTabRightMenu->setVisible(isBuildMode);
			m_fields->buildTabLeftMenu->setVisible(isBuildMode);
		}
		if (m_fields->pinnedGroupsNode) {
			m_fields->pinnedGroupsNode->setVisible(isBuildMode);
		}
	});

	// add tab switch callback to hide and show groups
	alpha::editor_tabs::addTabSwitchCallback([this](auto id) {
		if (auto group = getOpenedOrHoveredGroupV2()) {
			if (auto tabRes = alpha::editor_tabs::nodeForTab(id)) {
				if (tryGetBarInfo(**tabRes)) { // make sure this is my tab
					auto bar = static_cast<EditButtonBar*>(**tabRes);
					if (bar->m_buttonArray->containsObject(group->getCmi())) {
						group->setVisible(true);
						return;
					}
				}
			}
			group->setVisible(false);
		}
	});

	// keybinds
	addEventListener(KeybindSettingPressedEventV3(GEODE_MOD_ID, "goto-object"), [this](const Keybind& keybind, bool down, bool repeat, double timestamp) {
		if (down && !repeat) {
			auto selected = getSelectedObjects();
			if (auto obj = static_cast<GameObject*>(selected->firstObject())) {
				goToObjectV2(obj->m_objectID);
				if (obj->m_objectID != m_selectedObjectIndex) {
					m_selectedObjectIndex = obj->m_objectID;
					updateCreateMenu(false);
				}
				return ListenerResult::Stop;
			}
		}
		return ListenerResult::Propagate;
	});

	addEventListener(KeybindSettingPressedEventV3(GEODE_MOD_ID, "close-all"), [this](const Keybind& keybind, bool down, bool repeat, double timestamp) {
		if (down && !repeat) {
			auto arr = CCArray::create();
			arr->addObjectsFromArray(m_fields->pinnedGroupsNode->getChildren());
			for (int i = 0; i < arr->count(); i++) {
				static_cast<Group*>(arr->objectAtIndex(i))->changeGroupState(GroupState::CLOSED);
			}
			return ListenerResult::Stop;
		}
		return ListenerResult::Propagate;
	});

	// setup hover
	schedule(schedule_selector(MyEditorUI::updateHover));

	// ui scale setup
	addEventListener(tinker::api::ui_scaling::UIScaleUpdated(), [this] (float scale, bool scaleToolbars, bool topAlign) {
		updateUiScale();
		return ListenerResult::Propagate;
	});
	updateUiScale();


	return true;
}


void MyEditorUI::toggleMode(CCObject* sender) {
	EditorUI::toggleMode(sender);
	// if (auto menu = m_fields->rowMenu) {
	// 	menu->setVisible(m_selectedMode == 2 && Global::get().m_isEditMode);
	// 	m_fields->buildTabRightMenu->setVisible(m_selectedMode == 2);
	// 	m_fields->buildTabLeftMenu->setVisible(m_selectedMode == 2);
	// }
	// if (m_fields->pinnedGroups) {
	// 	m_fields->pinnedGroups->setVisible(m_selectedMode == 2);
	// }
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
			goToObjectV2(m_selectedObjectIndex, false);
			// updateGroupItem(m_selectedObjectIndex);
		}
	}
}


void MyEditorUI::clickOnPosition(CCPoint p0) {
	int before = m_selectedObjectIndex;
	EditorUI::clickOnPosition(p0);
	if (before == 0 && m_selectedObjectIndex != before) {
		runAction(CallFuncExt::create([this]{
			goToObjectV2(m_selectedObjectIndex);
		}));
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
			// setNewOpenedGroup(nullptr, nullptr); // close
			if (auto opened = getOpenedOrHoveredGroupV2()) {
				opened->changeGroupState(GroupState::CLOSED);
			}
		}
	}
}

bool MyEditorUI::ccTouchBegan(CCTouch* touch, CCEvent* event) {
	if (!EditorUI::ccTouchBegan(touch, event)) return false;
	// close hovered group on editor touch
	if (auto g = getOpenedOrHoveredGroupV2()) {
		if (g->getState() == GroupState::HOVERED) {
			g->changeGroupState(GroupState::CLOSED);
		}
	}
	return true;
}

// todo: delete this

// #include <Geode/modify/CreateMenuItem.hpp>

// static int aaaaa = 0;

// class $modify(MyCreateMenuItem, CreateMenuItem) {
// 	struct Fields {
// 		Fields() {
// 			log::debug("CMI+ {}", ++aaaaa);
// 		}
// 		~Fields() {
// 			log::debug("CMI- {}", --aaaaa);
// 		}
// 	};

// 	static CreateMenuItem* create(CCNode* p0, CCNode* p1, CCObject* p2, SEL_MenuHandler p3) {
// 		CreateMenuItem* ret = CreateMenuItem::create(p0, p1, p2, p3);
// 		static_cast<MyCreateMenuItem*>(ret)->m_fields.self();
// 		return ret;
// 	}
// };