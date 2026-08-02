#include "EditorUI.hpp"
#include "popups/AddObjectActionPopup.hpp"
#include "popups/MoreOptionsPopup.hpp"
#include "popups/DeleteGroupActionPopup.hpp"
#include "popups/SearchPopup.hpp"
#include "ObjectFoundEvent.hpp"
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


static std::string convertToEditorTabApiTabId(const std::string &normalId) {
	return utils::string::replace(normalId, "-tab-bar", "");
}

struct BarSize {int rows; int cols;};

struct BarSize getBarSize() {
	return {
		.rows = GameManager::get()->getIntGameVariable("0050"),
		.cols = GameManager::get()->getIntGameVariable("0049")
	};
}

static void reloadBarItems(EditButtonBar* bar, bool keepPage=true) {
	if (bar && bar->m_buttonArray) {
		auto sz = getBarSize();
		// 'columns' and 'rows' args are in wrong order here, bruh
		bar->loadFromItems(bar->m_buttonArray, sz.cols, sz.rows, keepPage);
	}
}

void MyEditorUI::setupRowMenu() {
	const auto rowMenu = CCMenu::create();
	const auto rowMenu2 = CCMenu::create();
	addChild(rowMenu);
	addChild(rowMenu2);
	rowMenu->setAnchorPoint({0.5, 0});
	rowMenu->setLayout(RowLayout::create()->setGap(8));
	rowMenu->setID("row_menu"_spr);

	rowMenu2->setAnchorPoint({0.5, 0});
	rowMenu2->setLayout(RowLayout::create()->setGap(8));
	rowMenu2->setID("row_menu_2"_spr);

	auto newObjectBtn = CCMenuItemSpriteExtra::create(
		CCSprite::create("OG_rowBtn_newObject.png"_spr), this, 
		menu_selector(MyEditorUI::onNewObjectButton)
	);
	auto newGroupBtn = CCMenuItemSpriteExtra::create(
		CCSprite::create("OG_rowBtn_newGroup.png"_spr), this, 
		menu_selector(MyEditorUI::onNewGroupButton)
	);
	auto moveAllBtn = CCMenuItemSpriteExtra::create(
		CCSprite::create("OG_rowBtn_moveAll.png"_spr), this, 
		menu_selector(MyEditorUI::onToggleSecondRowMenu)
	);
	auto moveDownBtn = CCMenuItemSpriteExtra::create(
		CCSprite::create("OG_rowBtn_moveDown.png"_spr), this, 
		menu_selector(MyEditorUI::onMoveButton)
	);
	auto moveBackwardBtn = CCMenuItemSpriteExtra::create(
		CCSprite::create("OG_rowBtn_moveLeft.png"_spr), this, 
		menu_selector(MyEditorUI::onMoveButton)
	);
	auto moveForwardBtn = CCMenuItemSpriteExtra::create(
		CCSprite::create("OG_rowBtn_moveRight.png"_spr), this, 
		menu_selector(MyEditorUI::onMoveButton)
	);
	auto moveUpBtn = CCMenuItemSpriteExtra::create(
		CCSprite::create("OG_rowBtn_moveUp.png"_spr), this, 
		menu_selector(MyEditorUI::onMoveButton)
	);
	// auto moveTabBackwardBtn = CCMenuItemSpriteExtra::create(
	// 	CCSprite::create("OG_rowBtn_moveLeft.png"_spr), this, 
	// 	menu_selector(MyEditorUI::onMoveTabButton)
	// );
	// auto moveTabForwardBtn = CCMenuItemSpriteExtra::create(
	// 	CCSprite::create("OG_rowBtn_moveRight.png"_spr), this, 
	// 	menu_selector(MyEditorUI::onMoveTabButton)
	// );
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

	moveUpBtn->setTag('u');
	moveDownBtn->setTag('d');
	moveForwardBtn->setTag('f');
	moveBackwardBtn->setTag('b');
	// moveTabForwardBtn->setTag('f');
	// moveTabBackwardBtn->setTag('b');

	rowMenu->addChild(newObjectBtn);
	rowMenu->addChild(newGroupBtn);
	rowMenu->addChild(moveAllBtn);
	rowMenu->addChild(saveMeBtn);
	rowMenu->addChild(deleteItemButton);
	rowMenu->addChild(moreOptionsButton);

	// auto text1 = CCLabelBMFont::create("Buttons:", "bigFont.fnt");
	// auto text2 = CCLabelBMFont::create("    Tabs:", "bigFont.fnt");
	// rowMenu2->addChild(text1);
	rowMenu2->addChild(moveUpBtn);
	rowMenu2->addChild(moveDownBtn);
	rowMenu2->addChild(moveBackwardBtn);
	rowMenu2->addChild(moveForwardBtn);
	// rowMenu2->addChild(text2);
	// rowMenu2->addChild(moveTabBackwardBtn);
	// rowMenu2->addChild(moveTabForwardBtn);

	rowMenu->setContentWidth(550);
	rowMenu->updateLayout();
	rowMenu2->setContentWidth(225);
	rowMenu2->updateLayout();

	m_fields->rowMenu = rowMenu;
	m_fields->rowMenu2 = rowMenu2;
}


void MyEditorUI::setupRightMenu() {
	auto menu = CCMenu::create(); 
	auto spr = CCSprite::create("OG_button_editMode.png"_spr);
	spr->setScale(0.6);
	auto tBtn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(MyEditorUI::onToggleEditGroupsMode));
	menu->addChild(tBtn);
	addChild(menu);
	menu->setAnchorPoint({1,0});

	menu->setLayout(RowLayout::create()->setAxisAlignment(AxisAlignment::End));
	menu->setContentWidth(100);
	menu->setContentHeight(30);
	menu->setZOrder(2);
	menu->updateLayout();
	menu->setID("toggle_menu"_spr);
	
	m_fields->buildTabRightMenu = menu;
}


void MyEditorUI::setupLeftMenu() {
	auto menu = CCMenu::create();
	
	if (Global::get().m_settings.m_enableGoToObject) {
		auto spr = CCSprite::create("OG_button_findObject.png"_spr);
		spr->setScale(0.6);
		auto tBtn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(MyEditorUI::onGotoObjectBtn));
		menu->addChild(tBtn);
	}

	addChild(menu);
	menu->setAnchorPoint({0,0});

	menu->setLayout(RowLayout::create()->setAxisAlignment(AxisAlignment::Start));
	menu->setContentWidth(100);
	menu->setContentHeight(30);
	menu->setZOrder(2);
	menu->updateLayout();
	menu->setID("goto_obj_menu"_spr);
	
	m_fields->buildTabLeftMenu = menu;
}


void MyEditorUI::updateUiScale() {
	EditorScale scaleStruct = getEditorScale();
	float commonScale = scaleStruct.commonScale;
	float tabScale = scaleStruct.tabScale;

	auto rowMenu = m_fields->rowMenu;
	auto rowMenu2 = m_fields->rowMenu2;

	rowMenu->setPosition(ccp(CCDirector::get()->getWinSize().width / 2, 111 * tabScale));
	float maxWidth = std::min(320.f, CCDirector::get()->getWinSize().width - 96 * 2 + 10);
	float rowMenuScale = maxWidth / rowMenu->getContentWidth() * commonScale;
	rowMenu->setScale(rowMenuScale);
	rowMenu2->setScale(rowMenuScale);

	m_fields->buildTabRightMenu->setPosition(ccp(CCDirector::get()->getWinSize().width - (7 + 
		getChildByID("toolbar-toggles-menu")->getContentWidth()) * tabScale, 2.5));
	m_fields->buildTabRightMenu->setScale(0.9 * tabScale);

	auto cat = getChildByID("toolbar-categories-menu");
	auto minX = cat->getPositionX() - cat->getAnchorPoint().x * cat->getScaledContentWidth();
	m_fields->buildTabLeftMenu->setPosition(ccp((6 + minX + cat->getContentWidth()) * tabScale, 2.5));
	m_fields->buildTabLeftMenu->setScale(0.9 * tabScale);
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

	std::vector<int> allOldIdsOrdered;
	for (auto* btn : CCArrayExt<CreateMenuItem*>(oldButtons)) {
		allOldIdsOrdered.push_back(btn->m_objectID);
	}

	std::set<int> allOldIds(allOldIdsOrdered.begin(), allOldIdsOrdered.end());
	std::set<int> clearedIds;

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
			auto id = group->getObjIds()[0];
			if (allOldIds.contains(id) && !clearedIds.contains(id)) {
				buttons->addObject(group->getCmi());
				clearedIds.insert(id);
			}
		}
	}

	// add objects, that were not used in any of the groups
	if (Global::get().m_settings.m_appendDeleted) {
		for (auto objId : allOldIdsOrdered) {
			if (clearedIds.contains(objId)) continue;
			auto btn = getCustomCreateBtn(objId, getItemBtnColor(objId));
			buttons->addObject(btn);
		}
	}

	bar->loadFromItems(buttons, p1, p2, p3);

	// don't need it anymore, release buttons
	allGroups[tab] = nullptr;

	// fix arrows overlapping
	if (bar->m_scrollLayer) bar->m_scrollLayer->setZOrder(2);
}


void MyEditorUI::setupExtraTabs(std::set<uint8_t> const &which) {
	auto sz = getBarSize();

	for (int i = 0; i < 16; i++) {
		if (!which.contains(i)) continue;

		std::string tabId = fmt::format("extra-tab-{}"_spr, i+1);

		alpha::editor_tabs::addTab(tabId, alpha::editor_tabs::BUILD,
			[this, sz, i] { // Crate the tab
				auto ret = alpha::editor_tabs::createEditButtonBar({});
				ret->m_hasCreateItems = true;

				ret->setUserObject(BAR_USER_OBJ_ID, new BarInfo(13+i));
				loadGroups(ret, ret->m_buttonArray, 13+i, sz.cols, sz.rows, true);

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
	auto sz = getBarSize();
	auto tabsMenu = getChildByID("build-tabs-menu");

	for (int i = 0; i < 13; i++) {
		auto tabId = vanillaTabsInfo[i].barId;
		
		if (auto bar = static_cast<EditButtonBar*>(getChildByID(tabId))) {
			bar->setUserObject(BAR_USER_OBJ_ID, new BarInfo(i));
			loadGroups(bar, bar->m_buttonArray, i, sz.cols, sz.rows, true);
		}

		auto shortTabId = convertToEditorTabApiTabId(tabId);
		if (auto toggler = alpha::editor_tabs::togglerForTab(shortTabId)) {
			auto objStr = Mod::get()->getSavedValue<std::string>(fmt::format("tab_{}_icon", i), "");
			if (!objStr.empty()) {
				setSpiteToTabByIndexFromString(objStr, static_cast<CCMenuItemToggler*>(*toggler), i);
			}
		}
	}
}

void MyEditorUI::setupSearchTab() {
	// if (!Global::get().m_settings.m_enableSearchTab) return;

	// alpha::editor_tabs::addTab("search-tab"_spr, alpha::editor_tabs::BUILD,
	// 	[this] { // Crate the tab
	// 		auto ret = alpha::editor_tabs::createEditButtonBar({});
	// 		m_fields->searchTabBar = ret;
	// 		return ret;
	// 	},
	// 	[] { // crate tab icon
	// 		auto icon = CCSprite::create("OG_search_icon.png"_spr);
	// 		icon->setScale(0.4);
	// 		return icon;
	// 	},
	// 	[this] (bool state, auto) { // do something when the tab is entered and exited
	// 		// log::info("ts {}", state);
	// 		if (!state) { // means other tab was opened
	// 			toggleSearch(true);
	// 			return;
	// 		};
	// 		toggleSearch();
	// 	}
	// );

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
	if (!cmi) return;
	cmi->addChild(m_fields->buttonFrame, 5);
	if (auto buttonSpr = cmi->getNormalImage()) {
		m_fields->buttonFrame->setPosition(buttonSpr->getPosition());
	}
}


// helper function that returns a button on which the frame is set (or nullptr)
CreateMenuItem* MyEditorUI::getFocusedCmi() {
	return static_cast<CreateMenuItem*>(m_fields->buttonFrame->getParent());
}


Group* MyEditorUI::getOpenedOrHoveredGroupV2() {
	for (auto ch : m_fields->pinnedGroupsNode->getChildrenExt<Group>()) {
		if (ch->getState() == GroupState::OPENED) return ch;
		if (ch->getState() == GroupState::HOVERED) return ch;
	}
	return nullptr;
}


void MyEditorUI::setNewSelectedGroupCmi(CreateMenuItem* groupCmi) {
	if (m_fields->selectedGroupCmi) setColorToCreateBtnNew(m_fields->selectedGroupCmi, true);
	if (groupCmi) setColorToCreateBtnNew(groupCmi, false);
	m_fields->selectedGroupCmi = groupCmi;
}

// enable/disable the row menu
void MyEditorUI::onToggleEditGroupsMode(CCObject* sender) {
	
	const auto enableEditMode = [this, sender](bool enable) {
		auto btn = static_cast<CCMenuItemSpriteExtra*>(sender);
		if (auto menu = m_fields->rowMenu) menu->setVisible(enable);
		if (auto menu2 = m_fields->rowMenu2) menu2->setVisible(false);
		if (btn) btn->setColor(enable ? ccc3(127, 127, 127) : ccc3(255, 255, 255));
		m_fields->buttonFrame->setVisible(enable);
	};

	Global::get().m_isEditMode = !Global::get().m_isEditMode;

	if (!Global::get().m_isEditMode) {
		// disable
		if (Global::get().m_hasUnsavedOGChanges) {
			createQuickPopup("Unsaved Changes Warning",
				"You have <co>unsaved</c> changes in <cy>Object Groups</c> configuration.\n"
				"<cj>Do you want to save them?</c>",
				"No, save later", "Yes, save now", 
				[enableEditMode] (auto, bool isBtn2) {
					if (isBtn2) Global::editor()->onSaveButton(nullptr);
					enableEditMode(false);
				}, true, true
			);
		} else {
			enableEditMode(false);
		}
	} else if (m_selectedMode == 2 /* build mode */) {
		// enable
		enableEditMode(true);
	}

	// update all opened and pinned groups
	for (auto group : m_fields->pinnedGroupsNode->getChildrenExt<Group>()) {
		group->setUpdateRequired(true);
		group->updateMenu(true);
	}
}


// my brick-implementation of hovering buttons (verbose commenting: enabled)
void MyEditorUI::updateHover(float dt) {
	// hover modes: 1 - disabled, 2 - click then hover, 3 - always, 4 - while holding
	if (Global::get().m_settings.m_hoverMode == 1) {
		return;	// disabled
	}

	#ifdef GEODE_IS_DESKTOP
		// check that mouse moved
		static CCPoint mouseLast;
		const CCPoint mouse = getMousePos();
		// hover mode is using mouse and mouse didn't move
		if (mouse == mouseLast && Global::get().m_settings.m_hoverMode != 4) {
			return;
		}
		mouseLast = mouse;
	#else
		// only 4-th mode makes sense on mobile
		if (Global::get().m_settings.m_hoverMode != 4) { // while holding
			return;
		}
		const CCPoint mouse = ccp(-100, -100);
	#endif

	const auto openedOrHoveredGroup = getOpenedOrHoveredGroupV2();

	// function that closes the hovered group
	const auto closeHoveredGroup = [this, targetGroup = Ref(openedOrHoveredGroup)] () {
		if (targetGroup && targetGroup->getState() == GroupState::HOVERED) {
			if (Global::get().m_settings.m_hoverMode != 2) { // click then hover
				targetGroup->changeGroupState(GroupState::CLOSED);
			}
		}
	};

	// I know that you, Alphalaneous, is the only person (besides me) who might ever read this
	// I hope you don't mind if I just throw you tooltip away from the screen ;)
	const auto hideTinkerTooltip = [this] {
		if (auto tooltipHover = getChildByID("alphalaneous.tinker/tooltip-hover-node")) {
			if (auto tooltip = tooltipHover->getChildByID("alphalaneous.tinker/tooltip-background")) {
				tooltip->setPositionY(-999);
			}
		}
	};

	const auto foundHoveredCmi = [this, hideTinkerTooltip, closeHoveredGroup] (CreateMenuItem* cmi) {
		if (auto group = Group::get(cmi)) {
			if (group->isSingle()) {
				return closeHoveredGroup();
			}
			switch (group->getState()) {
				case GroupState::CLOSED: group->changeGroupState(GroupState::HOVERED); // no break
				case GroupState::OPENED:
				case GroupState::HOVERED: {
					return hideTinkerTooltip();
				}
				default: break;
			}
		}
		closeHoveredGroup();
	};

	// if holding something in EditorUI
	if (m_rotationTouchID != -1 || m_transformTouchID != -1 || m_scaleTouchID != -1 || m_touchID != -1) {
		return closeHoveredGroup();
	}

	// check possible touch-blocking children
	if (CCScene::get()->getChildByType<FLAlertLayer>(0) || m_editorLayer->getChildByID("EditorPauseLayer")) {
		return closeHoveredGroup();
	}

	// special cases
	if (m_editorLayer->m_playbackMode == PlaybackMode::Playing || m_selectedMode != 2) {
		return closeHoveredGroup();
	}

	// in this mode there must be already opened/hovered group to continue hovering
	if (Global::get().m_settings.m_hoverMode == 2) { // click, then hover
		if (!openedOrHoveredGroup) return;
	}

	// if we are in mode-4, we don't care about coordinates as we can 
	// get touched button directly from CCMenu
	if (Global::get().m_settings.m_hoverMode == 4) {
		if (auto currentBar = getCurrentTabIfAllowed()) {
			int currentPage = mod(currentBar->m_scrollLayer->m_page, currentBar->m_scrollLayer->getTotalPages());
			auto sz = getBarSize();
			int firstIndex = sz.cols * sz.rows * currentPage; // index the first obj on current page		
			if (firstIndex < currentBar->m_buttonArray->count()) {
				if (auto button = static_cast<CCNode*>(currentBar->m_buttonArray->objectAtIndex(firstIndex))) {
					if (auto menu = static_cast<CCMenu*>(button->getParent())) {
						if (menu->m_eState == tCCMenuState::kCCMenuStateTrackingTouch) {
							if (auto cmi = static_cast<CreateMenuItem*>(menu->m_pSelectedItem)) {
								return foundHoveredCmi(cmi);
							}
						}
					}
				}
			}
		}
		return closeHoveredGroup();
	}

	// now check, which elements in EditorUI are actually hovered

	// check OPENED/HOVERED/PINNED groups at first
	CCPoint posOnPinnedLayer = m_fields->pinnedGroupsNode->convertToNodeSpace(mouse);

	// check HOVERED group from last frame
	if (openedOrHoveredGroup && openedOrHoveredGroup->getState() == GroupState::HOVERED) {
		if (openedOrHoveredGroup->boundingBox().containsPoint(posOnPinnedLayer)) {
			if (!Global::get().m_settings.m_ignoreHoveredGroups) {
				return;
			}
		}
	}

	// check OPENED or PINNED groups
	for (auto ch : m_fields->pinnedGroupsNode->getChildrenExt<Group>()) {
		if (ch->boundingBox().containsPoint(posOnPinnedLayer)) {
			if (Global::get().m_settings.m_ignoreHoveredGroups) {
				if (ch->getState() == GroupState::HOVERED) {
					continue;
				}
			}
			return closeHoveredGroup();
		}
	}

	// get visible menu
	auto currentBar = getCurrentTabIfAllowed();
	if (!currentBar) {
		return closeHoveredGroup();
	}

	// check hover over EditButtonBar
	auto posOnBar = currentBar->convertToNodeSpace(mouse);
	if (posOnBar.x < 0 || posOnBar.y < 0 || posOnBar.x > currentBar->getContentWidth() || posOnBar.y > currentBar->getContentHeight()) {
		return closeHoveredGroup();
	}
	
	// find visible buttons
	int firstIndex, lastIndex;
	bool isScroll = currentBar->getChildByID("alphalaneous.tinker/buttons-scroll-layer");
	if (isScroll) {
		firstIndex = 0;
		lastIndex = currentBar->m_buttonArray->count() - 1;
	} else {
		int currentPage = mod(currentBar->m_scrollLayer->m_page, currentBar->m_scrollLayer->getTotalPages());
		auto sz = getBarSize();
		firstIndex = sz.cols * sz.rows * currentPage; // index the first obj on current page
		lastIndex = std::min(int(currentBar->m_buttonArray->count()), firstIndex + sz.rows * sz.cols) - 1;
	}

	if (firstIndex >= currentBar->m_buttonArray->count()) {
		return closeHoveredGroup();
	}

	auto firstButton = static_cast<CCNode*>(currentBar->m_buttonArray->objectAtIndex(firstIndex));
	auto menu = firstButton->getParent();

	// check hover over button menu
	CCPoint posOnMenu = menu->convertToNodeSpace(mouse);
	if (posOnMenu.x < 0 || posOnMenu.y < 0 || posOnMenu.x > menu->getContentWidth() || posOnMenu.y > menu->getContentHeight()) {
		return closeHoveredGroup();
	}
	
	// find hovered button in menu
	for (int i = firstIndex; i <= lastIndex; i++) {
		auto cmi = static_cast<CreateMenuItem*>(currentBar->m_buttonArray->objectAtIndex(i));
		if (cmi->boundingBox().containsPoint(posOnMenu)) {
			// found
			return foundHoveredCmi(cmi);
		}
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

	// function to create new buttons
	const auto addObjects = [this](std::vector<int> ids, bool focus=false) {
		auto arr = CCArray::create();
		if (!ids.size()) return;
		for (int id : ids) {
			auto newBtn = Group::createSingle(id, true)->getCmi();
			arr->addObject(newBtn);
		}
		addButtonsAndReloadButtonBar(arr);
		goToPageWithCmi(static_cast<CreateMenuItem*>(arr->lastObject()), focus);
		shortAlert("Created!");
		Global::get().m_hasUnsavedOGChanges = true;
	};

	// allow adding focused from groups
	if (auto cmi = getFocusedCmi(); cmi && cmi->m_objectID != 0) {
		for (auto ch : m_fields->pinnedGroupsNode->getChildrenExt<Group*>()) {
			if (ch->containsButton(cmi)) { // found in group
				if (!nodeIsVisible(ch)) break;
				addObjects({cmi->m_objectID});
				return;
			}
		}
	}

	// try to get selected objects
	auto objects = getSelectedObjects();
	if (objects->count() == 0) {
		alert("To add new object to the tab you must select at least <cy>1</c> object "
			"in editor <cy>or</c> have a <cy>focused</c> button in the group");
		return;
	}

	// create item on EditButtonBar for this obj
	if (objects->count() == 1) { 
		addObjects(getUniqueIds(objects), true);
	} else {
		auto tmp = CCArray::create();
		tmp->addObjectsFromArray(objects);

		AddObjectActionPopup::create(
			[addObjects, tmp = Ref(tmp)]{ // add multiple objects
				addObjects(getUniqueIds(*tmp), true);
			},
			[this, addObjects, tmp = Ref(tmp)]{ // add custom object
				auto lel = LevelEditorLayer::get();
				std::string str;
				for (auto* obj : CCArrayExt<GameObject*>(tmp)) {
					str = str.append(obj->getSaveString(lel)).append(";");
				}
				int newId = registerNewCustomObject(str);
				addObjects({newId}, true);
			},
			objects->count()
		)->show();
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
	if (selected->count() == 0) {
		alert("You must select at least one object to create a new <cy>custom object</c> button.");
		return;
	}

	// create custom
	auto levelLayer = LevelEditorLayer::get();
	std::string str;
	for (auto* obj : CCArrayExt<GameObject*>(selected)) {
		str = str.append(obj->getSaveString(levelLayer)).append(";");
	}
	int newId = registerNewCustomObject(str);

	// create item on EditButtonBar for this obj
	auto newBtn = Group::createSingle(newId, true)->getCmi();
	addButtonsAndReloadButtonBar(CCArray::createWithObject(newBtn));
	goToPageWithCmi(newBtn, true);
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
	auto cmi = getFocusedCmi();
	if (cmi == nullptr) {
		alert("Button is not selected");
		return;
	}

	// make sure that button is in this tab
	const auto index = currentTab->m_buttonArray->indexOfObject(cmi);
	if (index == UINT_MAX) {
		alert("Button is not selected or selected in another tab");
		return;
	}

	auto group = Group::get(cmi);
	if (!group || group->isSingle()) {
		deleteButtonFromTab(currentTab, index, false);
	} else {
		DeleteGroupActionPopup::create([this, currentTab, index] {
			// on delete
			deleteButtonFromTab(currentTab, index, false);
		},
		[this, currentTab, index] {
			// on ungroup
			deleteButtonFromTab(currentTab, index, true);
		})->show();
	}
}


void MyEditorUI::onMoreOptionsButton(CCObject*) {
	MoreOptionsPopup::create()->show();
}


// move selected button in create editButtonBar forward or backward
void MyEditorUI::onMoveButton(CCObject* sender) {
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
	const auto index = currentTab->m_buttonArray->indexOfObject(btn);
	if (index == UINT_MAX) {
		alert("Button is not selected or selected in another tab");
		return;
	}

	// amount of indexes in this function is crazy
	int newIndex = index;
	auto sz = getBarSize();

	if (currentTab->getChildByID("alphalaneous.tinker/buttons-scroll-layer")) {
		switch (sender->getTag()) {
			case 'f': newIndex += sz.rows; break;
			case 'b': newIndex += -sz.rows; break;
			case 'd': newIndex += 1; break;
			case 'u': newIndex += -1; break;
		}
	} else {
		switch (sender->getTag()) {
			case 'f': newIndex += 1; break;
			case 'b': newIndex += -1; break;
			case 'd': newIndex += sz.cols; break;
			case 'u': newIndex += -sz.cols; break;
		}
	}

	// overflow
	if (newIndex >= currentTab->m_buttonArray->count() || newIndex < 0) {
		int sign = 0;
		if (newIndex < 0) {
			if (index == 0) sign = -1;
		} else {
			if (index == currentTab->m_buttonArray->count() - 1) sign = 1;
		}
		if (sign) { // move to next/prev tab
			if (auto tabIndex = alpha::editor_tabs::indexForTab(currentTab)) {
				for (int i = 1; i < 20; i++) {
					int tryTabIndex = *tabIndex + i * sign;
					if (auto tryTab = alpha::editor_tabs::tabForIndex(tryTabIndex, alpha::editor_tabs::BUILD)) {
						if (tryGetBarInfo(*tryTab)) { // found, move object to this tab
							auto newTab = static_cast<EditButtonBar*>(**tryTab);
							newIndex = sign == 1 ? 0 : newTab->m_buttonArray->count(); 
							newTab->m_buttonArray->insertObject(btn, newIndex);
							currentTab->m_buttonArray->removeObjectAtIndex(index);
							reloadBarItems(currentTab);
							reloadBarItems(newTab);
							goToPageWithCmi(btn);
							Global::get().m_hasUnsavedOGChanges = true;
							if (auto singleGroup = Group::get(btn)) {
								singleGroup->setUserCreated(true);
							}
							break;
						}
					}
				}
			}
			return;
		} else { // clamp
			newIndex = std::clamp(newIndex, 0, int(currentTab->m_buttonArray->count() - 1));
		}
	}

	int step = newIndex > index ? 1 : -1;
	for (int i = index; i != newIndex; i += step) {
		currentTab->m_buttonArray->exchangeObjectAtIndex(i, i + step);
	}
	reloadBarItems(currentTab);
	goToPageWithCmi(btn);
	Global::get().m_hasUnsavedOGChanges = true;
}


void MyEditorUI::onToggleSecondRowMenu(CCObject*) {
	auto rowMenu = m_fields->rowMenu;
	auto rowMenu2 = m_fields->rowMenu2;
	if (!rowMenu || !rowMenu2) return;
	rowMenu2->setPosition(CCDirector::get()->getWinSize().width / 2, 4 + rowMenu->getPosition().y + rowMenu->getScaledContentHeight());
	rowMenu2->setVisible(!rowMenu2->isVisible());
	rowMenu2->setScale(rowMenu->getScale());
}

// todo: some time in the future
// void MyEditorUI::onMoveTabButton(CCObject* sender) {
// 	if (auto currTabId = alpha::editor_tabs::getCurrentTab()) {
// 		if (auto toggler = alpha::editor_tabs::togglerForTab(*currTabId)) {
// 			if (auto menu = (*toggler)->getParent()) {
// 				auto children = menu->getChildren();
// 				auto idx = children->indexOfObject(*toggler);
// 				if (idx == UINT_MAX) return;
// 				if (sender->getTag() == 'f') {
// 					for (int i = idx + 1; i < children->count(); i++) {
// 						auto next = static_cast<CCNode*>(children->objectAtIndex(i));
// 						menu->swapChildIndices(*toggler, next);
// 						if (next->isVisible()) break;
// 					}
// 				} else {
// 					for (int i = idx - 1; i >= 0; i--) {
// 						auto prev = static_cast<CCNode*>(children->objectAtIndex(i));
// 						menu->swapChildIndices(*toggler, prev);
// 						if (prev->isVisible()) break;
// 					}
// 				}
// 				menu->updateLayout();
// 			}
// 		}
// 	}
// }


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

	Group* group = nullptr;
	auto const selected = getSelectedObjects();

	if (selected->count() == 0) {
		group = Group::createDefault();
	} else {
		auto ids = getUniqueIds(selected);
		group = Group::createFromArray("New Group", {ids[0],0,0,0}, std::move(ids));
	}

	auto newBtn = group->getCmi();
	addButtonsAndReloadButtonBar(CCArray::createWithObject(newBtn));
	goToPageWithCmi(newBtn, true);
	Global::get().m_hasUnsavedOGChanges = true;
	shortAlert("Created!");

	group->changeGroupState(GroupState::OPENED);
}


void MyEditorUI::deleteButtonFromTab(EditButtonBar* bar, int idx, bool ungroup) {
	if (!tryGetBarInfo(bar)) {
		log::warn("Can't delete from this tab (deleteButtonFromTab)");
		return;
	}

	if (idx >= bar->m_buttonArray->count()) return;
	auto cmi = static_cast<CreateMenuItem*>(bar->m_buttonArray->objectAtIndex(idx));
	bool ungrouped = false;

	if (getFocusedCmi() == cmi) setNewFocusedCmi(nullptr);

	if (auto group = Group::get(cmi)) {
		cmi->retain();
		group->retain();

		if (group->isSingle()) {
			// single object
			group->removeFromParent();
			bar->m_buttonArray->removeObjectAtIndex(idx);
			m_createButtonArray->fastRemoveObject(cmi);
		} else {
			// group
			group->clearAllCreateMenuItems(); // proper group deletion
			group->removeFromParent();
			bar->m_buttonArray->removeObjectAtIndex(idx);

			if (ungroup) {
				int currIdx = idx;
				for (auto row : group->getMatrix()) {
					for (auto id : row) {
						if (id == 0) continue;
						auto newBtn = Group::createSingle(id, true)->getCmi();
						if (m_selectedObjectIndex == id)
							setColorToCreateBtnNew(newBtn, false);
						bar->m_buttonArray->insertObject(newBtn, currIdx++); // i'm slow
					}
				}
				ungrouped = true;
			}
		}

		// basically swap parent and child roles to never lose original cmi
		cmi->setUserObject(CMI_USER_OBJ_ID, nullptr);
		group->setUserObject("abc"_spr, cmi);

		cmi->release();
		group->release();

	} else {
		// not a group
		if (cmi->m_objectID == 0) {
			alert("You can't delete this button");
			return;
		}
		bar->m_buttonArray->removeObjectAtIndex(idx);
		m_createButtonArray->fastRemoveObject(cmi);
	}

	// reload items
	int currentPage = mod(bar->m_scrollLayer->m_page, bar->m_scrollLayer->getTotalPages());
	reloadBarItems(bar);
	bar->goToPage(currentPage);

	shortAlert(ungrouped ? "Ungrouped!" : "Deleted!");
	Global::get().m_hasUnsavedOGChanges = true;
}


bool MyEditorUI::addItemToActiveGroupByCmi(CreateMenuItem* cmi) {
	// this function is used in "shift-add" feature
	if (cmi->m_objectID == 0) return false;

	const std::vector<int> objId = {cmi->m_objectID};
	const auto pinned = m_fields->pinnedGroupsNode->getChildren();
	const auto focusedCmi = getFocusedCmi();
	std::vector<Group*> candidates;

	if (auto gr = getOpenedOrHoveredGroupV2(); gr && nodeIsVisible(gr)) {
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


int MyEditorUI::registerNewCustomObject(std::string oldStr) {
	// get next free id
	int id = CUSTOM_OBJECT_ID_OFFSET - 1;
	auto &custom = m_fields->myCustomObjects;
	while (custom.contains(std::to_string(id))) id--;

	// get original string and save to the map
	custom.insert({std::to_string(id), oldStr});

	Global::get().m_hasUnsavedOGChanges = true;
	return id;
}


std::map<std::string, std::string> MyEditorUI::getCustomObjects(std::set<int> const &which) {
	std::map<std::string, std::string> ret;
	for (int id : which) {
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
	int firstId = static_cast<GameObject*>(selected->objectAtIndex(0))->m_objectID;

	if (!res.empty()) {
		auto group = Group::createGroup("New Group", {firstId,0,0,0}, std::move(res));
		auto newBtn = group->getCmi();
		addButtonsAndReloadButtonBar(CCArray::createWithObject(newBtn));
		goToPageWithCmi(newBtn, true);
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
		auto shortTabId = convertToEditorTabApiTabId(currentTab->getID());
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
matjson::Value MyEditorUI::barToJsonValue(EditButtonBar* bar, std::set<int> &custom) {
	matjson::Value jsonArray(std::vector<int>{});
	// foreach item in my tab
	for (auto* cmi : CCArrayExt<CreateMenuItem*>(bar->m_buttonArray)) {
		if (auto group = Group::get(cmi)) {
			jsonArray.push(group->toJson(custom));
		} else if (cmi->m_objectID != 0) {
			// single not user-created object without any info
			auto unkObj = matjson::makeObject({{"obj", cmi->m_objectID}});
			jsonArray.push(unkObj);
			if (isMyCustomObject(cmi->m_objectID)) custom.insert(cmi->m_objectID);
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
					if (auto group = Group::get(cmi)) {
						func(group, tabIdx);
					}
				}
			}
		}
	}
}


void MyEditorUI::addButtonsAndReloadButtonBar(CCArrayExt<CreateMenuItem*> buttons, EditButtonBar* forceTab) {

	auto tab = forceTab ? forceTab : getCurrentTabIfAllowed();
	if (!tab || !tryGetBarInfo(tab)) {
		log::warn("Editing this tab is not allowed (addButtonsAndReloadButtonBar)");
		return;
	}

	int currentPage = mod(tab->m_scrollLayer->m_page, tab->m_scrollLayer->getTotalPages());

	auto sz = getBarSize();
	int firstIndex = sz.cols * sz.rows * currentPage; // index the first obj on current page

	// check if the selected cmi on the current page
	if (auto selectedCmi = getFocusedCmi()) {
		auto array = tab->m_buttonArray;
		for (int i = 0; i < sz.cols * sz.rows; i++) {
			if (firstIndex + i >= array->count()) break;
			if (array->objectAtIndex(firstIndex + i) == selectedCmi) { // found
				firstIndex = firstIndex + i;
				break;
			}
		}
	}
	
	for (auto* cmi : buttons) {
		tab->m_buttonArray->insertObject(cmi, firstIndex++);
		// select (or set frame to) newly created button
		if (cmi->m_objectID != 0 && m_selectedObjectIndex == cmi->m_objectID) {
			setColorToCreateBtnNew(cmi, false);
		}
	}
	
	reloadBarItems(tab);

	// preserve the page
	tab->goToPage(currentPage);
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
	auto sz = getBarSize();
	int resultCount = sz.rows * sz.cols;
	std::vector<std::pair<Group*, float>> res;
	auto bar = m_fields->searchTabBar;
	if (!bar) return;

	// close
	if (auto opened = getOpenedOrHoveredGroupV2()) {
		opened->changeGroupState(GroupState::CLOSED);
	}

	if (query.size() == 0) {
		bar->m_buttonArray->removeAllObjects();
		reloadBarItems(bar);
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

	reloadBarItems(bar);
}


void MyEditorUI::onGotoObjectBtn(CCObject*) {
	auto selected = getSelectedObjects();
	if (auto obj = static_cast<GameObject*>(selected->firstObject())) {
		goToObjectV2(obj->m_objectID, true);
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

// void MyEditorUI::updateGroupItem(int id) {

// 	auto allTabs = alpha::editor_tabs::getAllTabs();
// 	if (!allTabs) return;

// 	// foreach tab
// 	for (auto node : *allTabs) {
// 		if (!tryGetBarInfo(node)) continue;

// 		auto bar = static_cast<EditButtonBar*>(node);
// 		auto shortTabId = convertToEditorTabApiTabId(node->getID());
		
// 		// foreach item in my tab
// 		int buttonIndex = -1;
// 		for (auto* cmi : CCArrayExt<CreateMenuItem*>(bar->m_buttonArray)) {
// 			buttonIndex++;
// 			auto group = Group::get(cmi);
// 			if (group) { // group
// 				auto &matrix = group->getMatrix();
// 				for (int i = 0; i < matrix.size(); i++) {
// 					auto &row = matrix[i];
// 					for (int j = 0; j < row.size(); j++) {
// 						if (id == row[j]) {
// 							setNewSelectedGroupCmi(cmi);
// 							razoom::ObjectFoundEvent().send(cmi);
// 							return;
// 						}
// 					}
// 				}
// 			}
// 		}
// 	}
// }

void MyEditorUI::goToPageWithCmi(CreateMenuItem* target, bool focus) {
	
	auto sz = getBarSize();
	int const pgSize = sz.cols * sz.rows;

	auto allTabs = alpha::editor_tabs::getAllTabs();
	if (!allTabs) return;

	// foreach tab
	for (auto node : *allTabs) {
		if (!tryGetBarInfo(node)) continue;
		auto bar = static_cast<EditButtonBar*>(node);
		auto shortTabId = convertToEditorTabApiTabId(node->getID());
		
		// foreach item in my tab
		for (int i = 0; i < bar->m_buttonArray->count(); i++) {
			if (target == bar->m_buttonArray->objectAtIndex(i)) {
				alpha::editor_tabs::switchTab(shortTabId);
				bar->goToPage(i / pgSize);
				if (focus) setNewFocusedCmi(target);
				return;
			}
		}
	}
}

void MyEditorUI::goToObjectV2(int id, bool playEffect) {
	auto sz = getBarSize();
	int const pgSize = sz.cols * sz.rows;

	auto allTabs = alpha::editor_tabs::getAllTabs();
	if (!allTabs) return;

	// foreach tab
	for (auto node : *allTabs) {
		if (!tryGetBarInfo(node)) continue;
		auto bar = static_cast<EditButtonBar*>(node);
		auto shortTabId = convertToEditorTabApiTabId(node->getID());
		
		// foreach item in my tab
		for (int btnIdx = 0; btnIdx < bar->m_buttonArray->count(); btnIdx++) {
			auto cmi = static_cast<CreateMenuItem*>(bar->m_buttonArray->objectAtIndex(btnIdx));

			if (cmi->m_objectID != 0) {
				if (cmi->m_objectID == id) {
					// found single object
					alpha::editor_tabs::switchTab(shortTabId);
					bar->goToPage(btnIdx / pgSize);
					if (playEffect) playCircleEffectOnCmi(cmi);
					razoom::ObjectFoundEvent().send(cmi);
					return;
				}
			} else if (auto group = Group::get(cmi); group && !group->isSingle()) {
				auto &matrix = group->getMatrix();
				for (int i = 0; i < matrix.size(); i++) {
					auto &row = matrix[i];
					for (int j = 0; j < row.size(); j++) {
						if (id == row[j]) {
							// found in group
							bool openedOrPinned = group->getState() == GroupState::OPENED
								|| group->getState() == GroupState::PINNED
								|| group->getState() == GroupState::HOVERED;
							
							auto targetCmi = cmi;
							if (openedOrPinned) {
								if (auto innerCmi = group->getCmiByPosition(j, i)) {
									targetCmi = innerCmi;
								}
							}

							if (id == m_selectedObjectIndex) {
								setNewSelectedGroupCmi(cmi);
							}

							alpha::editor_tabs::switchTab(shortTabId);
							bar->goToPage(btnIdx / pgSize);
							razoom::ObjectFoundEvent().send(cmi);
							if (playEffect) playCircleEffectOnCmi(targetCmi);
							return;
						}
					}
				}
			}
		}
	}
}

