#include "config.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include <Geode/modify/EditButtonBar.hpp>
#include <Geode/modify/EditorUI.hpp>

// colors: 1-green, 2-blue, 3-pink, 4-gray, 5-darker gray, 6-red
#define ITEM_COLOR 4
#define DARKER_ITEM_COLOR 5
#define GROUP_ITEM_COLOR 4


// global variables
struct {
	EditorUI* editorUI = nullptr;
	struct {
		int m_maxMenuHeight = 4;
		bool m_showBg = true;
		std::string m_customConfig = "";
		std::string m_defaultConfig = "";
		ccColor4B m_bgColor = ccc4(0, 0, 0, 140);
		short m_groupColor = 1;
	} m_settings;

	void updateSettings() {
		m_settings.m_maxMenuHeight = Mod::get()->getSettingValue<int64_t>("max-rows");
		m_settings.m_showBg = Mod::get()->getSettingValue<bool>("show-bg");
		m_settings.m_customConfig = Mod::get()->getSettingValue<std::filesystem::path>("config-path").string();
		m_settings.m_defaultConfig = Mod::get()->getSettingValue<std::filesystem::path>("default-config-path").string();
		m_settings.m_bgColor = Mod::get()->getSettingValue<ccColor4B>("bg-color");
		int col = std::atoi(Mod::get()->getSettingValue<std::string>("group-button-color").c_str());
		m_settings.m_groupColor = (col >= 1 && col <= 6) ? col : 1;
	};
} GLOBAL;


$on_mod(Loaded) {
	GLOBAL.updateSettings();
	Mod::get()->getConfigDir(true); // create config dir if not exists
	// write default config
	if(!writeConfigToJson(GLOBAL.m_settings.m_defaultConfig)) {
		log::error("Default config: FAILED to write file: {}", GLOBAL.m_settings.m_defaultConfig);
	}
	// check if custom config exists and if it is not, then create it and init with default config
	if (!std::filesystem::exists(GLOBAL.m_settings.m_customConfig)) {
		writeConfigToJson(GLOBAL.m_settings.m_customConfig);
	}
}


// parameters for group buttons
struct GroupInfo : CCObject {
	Group m_group;
	Ref<CCNode> m_existingMenu = nullptr;
	GroupInfo(Group group) : m_group(group) {
		this->autorelease();
	}
};

// default parameters for buttons
struct ItemInfo : CCObject {
	SEL_MenuHandler m_defaultSelector;
	ItemInfo(SEL_MenuHandler defaultSelector) : m_defaultSelector(defaultSelector) {
		this->autorelease();
	}
};

// default parameters for group items
struct GroupItemInfo : CCObject {
	CreateMenuItem* m_groupButton;
	SEL_MenuHandler m_defaultSelector;
	GroupItemInfo(CreateMenuItem* groupButton, SEL_MenuHandler defaultSelector) : 
		m_defaultSelector(defaultSelector), m_groupButton(groupButton) {
		this->autorelease();
	}
};



class $modify(MyEditButtonBar, EditButtonBar) {
	struct Fields {
		CCNode* m_menuNode = nullptr;
		CreateMenuItem* m_buttonWithGroupOpened = nullptr;
		CreateMenuItem* m_maybeActiveItemBtn = nullptr;
		CreateMenuItem* m_maybeActiveGroupBtn = nullptr;
	};

	void removeOldMenuIfExists() {
		if (m_fields->m_menuNode) {
			m_fields->m_menuNode->removeFromParent();
			m_fields->m_menuNode = nullptr;
			m_fields->m_buttonWithGroupOpened = nullptr;
		}
	}

	void registerButtons(CCArray* buttons) {
		for (unsigned i = 0; i < buttons->count(); i++) {
			auto obj = buttons->objectAtIndex(i);
			GLOBAL.editorUI->m_createButtonArray->addObject(obj);
		}
	}

	$override 
	void loadFromItems(CCArray* p0, int p1, int p2, bool p3) {
		if (p0->count() == 0) return EditButtonBar::loadFromItems(p0, p1, p2, p3);
		
		// check first object to find out what category we are building now
		bool barCreated = false;
		bool isGameObjectTab = false;
		if (auto cmi = typeinfo_cast<CreateMenuItem*>(p0->objectAtIndex(0))) {
			if (auto btnSpr = typeinfo_cast<ButtonSprite*>(cmi->getChildren()->objectAtIndex(0))) {
				auto btnChildren = btnSpr->getChildren();
				for (unsigned i = 0; i < btnChildren->count(); i++) {
					if (auto gameObj = typeinfo_cast<GameObject*>(btnChildren->objectAtIndex(i))) {
						// gg! found game object in children
						isGameObjectTab = true;
						if (categoryByFirstObjectId.contains(gameObj->m_objectID)) {
							short category = (*categoryByFirstObjectId.find(gameObj->m_objectID)).second;
							barCreated = createCustomBarForCategory(p0, category, p1, p2, p3);
							log::debug("Category: {}; done: {}", category, barCreated);
						}
						break;
					}
				}
			}
		}

		if (!barCreated) {
			EditButtonBar::loadFromItems(p0, p1, p2, p3);
			if (isGameObjectTab) {
				registerButtons(p0);
			}
		} else {
			// fix overlapping with arrows
			auto myChildren = this->getChildren();
			for (unsigned i = 0; i < myChildren->count(); i++) {
				if (auto bsl = typeinfo_cast<BoomScrollLayer*>(myChildren->objectAtIndex(i))) {
					bsl->setZOrder(bsl->getZOrder() + 1);
					break;
				}
			}
		}
	};

	bool createCustomBarForCategory(CCArray* oldButtons, short category, int p1, int p2, bool p3) {
		if (!getCONFIG()->contains(category)) return false;

		std::vector<short> allOldIdsOrdered;
		std::set<short> clearedIds;
		for (unsigned i = 0; i < oldButtons->count(); i++) {
			auto btn = static_cast<CreateMenuItem*>(oldButtons->objectAtIndex(i));
			allOldIdsOrdered.push_back(btn->m_objectID);
		}

		auto buttons = CCArray::create();
		auto groups = (*getCONFIG()->find(category)).second;
		
		for (auto& group : groups) {
			if ((group.m_properties & GROUP_HIDE) || group.m_objectIds.size() == 0) {
				for (auto objId : group.m_objectIds) {
					clearedIds.insert(objId);
				}
				continue;
			}
			if ((group.m_properties & GROUP_UNGROUP) || group.m_objectIds.size() == 1) {
				// make simple item(s)
				for (auto objId : group.m_objectIds) {
					auto btnCol = darkerButtonBgObjIds.contains(objId) ? DARKER_ITEM_COLOR : ITEM_COLOR;
					auto btn = GLOBAL.editorUI->getCreateBtn(objId, btnCol);

					btn->setUserObject(new ItemInfo(btn->m_pfnSelector));
					btn->m_pfnSelector = menu_selector(MyEditButtonBar::onItemClick); 
					btn->m_objectID = objId;

					clearedIds.insert(objId);
					buttons->addObject(btn);
				}
			} else {
				// make a group
				auto btn = GLOBAL.editorUI->getCreateBtn(group.m_thumbnailObjectId, GLOBAL.m_settings.m_groupColor);
				btn->setUserObject(new GroupInfo(group));
				btn->m_pfnSelector = menu_selector(MyEditButtonBar::onGroupClick); 

				for (auto objId : group.m_objectIds) {
					clearedIds.insert(objId);
				}

				buttons->addObject(btn);
			}
		}
		// add objects, that were not used in any of the groups
		for (auto objId : allOldIdsOrdered) {
			if (clearedIds.contains(objId)) continue;
			auto btnCol = darkerButtonBgObjIds.contains(objId) ? DARKER_ITEM_COLOR : ITEM_COLOR;
			auto btn = GLOBAL.editorUI->getCreateBtn(objId, btnCol);
			btn->setUserObject(new ItemInfo(btn->m_pfnSelector));
			btn->m_pfnSelector = menu_selector(MyEditButtonBar::onItemClick);

			buttons->addObject(btn);
		}
		
		EditButtonBar::loadFromItems(buttons, p1, p2, p3);
		registerButtons(buttons);
		return true;
	}

	void onItemClick(CCObject* sender) {
		auto btn = static_cast<CreateMenuItem*>(sender);
		auto defaultInfo = static_cast<ItemInfo*>(btn->getUserObject());
		auto method = static_cast<void (CCObject::*)(CCObject*)>(defaultInfo->m_defaultSelector);

		// bool wasActive = isButtonActivated(btn);
		(this->*method)(sender); // call original selector

		bool thisIsNowSelected = btn->m_objectID == GLOBAL.editorUI->m_selectedObjectIndex;

		if (!isButtonActivated(btn) && thisIsNowSelected) {
			activateButton(btn); // for some reason it does not change color itself
			m_fields->m_maybeActiveItemBtn = btn;
		}
		removeOldMenuIfExists();
	}

	void onGroupClick(CCObject* sender) {
		auto btn = static_cast<CreateMenuItem*>(sender);
		auto groupInfo = static_cast<GroupInfo*>(btn->getUserObject());
		// FLAlertLayer::create("title", groupInfo->m_group.m_groupName, "ok")->show();
		bool onlyClose = (btn == m_fields->m_buttonWithGroupOpened);

		removeOldMenuIfExists();
		if (onlyClose) return;

		if (!groupInfo->m_existingMenu)  {
			groupInfo->m_existingMenu = createMenu(&groupInfo->m_group, btn, btn->getScale());
			bool activeItemBtn = (m_fields->m_maybeActiveItemBtn && isButtonActivated(m_fields->m_maybeActiveItemBtn));
			bool activeGroupBtn = (m_fields->m_maybeActiveGroupBtn && isButtonActivated(m_fields->m_maybeActiveGroupBtn));
			GLOBAL.editorUI->updateCreateMenu(false); // fix color bug (false - no jump to another page)
			// fix custom buttons deactivating on update
			if (activeItemBtn && !isButtonActivated(m_fields->m_maybeActiveItemBtn)) {
				activateButton(m_fields->m_maybeActiveItemBtn);
			}
			if (activeGroupBtn && !isButtonActivated(m_fields->m_maybeActiveGroupBtn)) {
				activateButton(m_fields->m_maybeActiveGroupBtn);
			}
		}
		auto menu = groupInfo->m_existingMenu;

		// menu->setScale(1 / btn->getScale());
		m_fields->m_menuNode = menu;
		m_fields->m_buttonWithGroupOpened = btn;
		btn->getParent()->addChild(menu);
		menu->setPosition(btn->getPosition());
	}

	void onGroupItemClick(CCObject* sender) {
		auto btn = static_cast<CreateMenuItem*>(sender);
		auto groupItemInfo = static_cast<GroupItemInfo*>(btn->getUserObject());
		auto method = static_cast<void (CCObject::*)(CCObject*)>(groupItemInfo->m_defaultSelector);

		bool wasActive = isButtonActivated(btn);
		(this->*method)(sender); // call original selector

		bool thisIsNowSelected = btn->m_objectID == GLOBAL.editorUI->m_selectedObjectIndex;

		if (thisIsNowSelected) {
			if (!wasActive) {
				if (!isButtonActivated(btn)) activateButton(btn);
				m_fields->m_maybeActiveItemBtn = btn;
			}
			if (!isButtonActivated(groupItemInfo->m_groupButton)) {
				activateButton(groupItemInfo->m_groupButton);
				m_fields->m_maybeActiveGroupBtn = groupItemInfo->m_groupButton;
			}
		} else {
			if (isButtonActivated(groupItemInfo->m_groupButton)) {
				activateButton(groupItemInfo->m_groupButton, true); // deactivate
				m_fields->m_maybeActiveItemBtn = nullptr;
			}
		}
		removeOldMenuIfExists();
	}

	bool isButtonActivated(CreateMenuItem* cmi) {
		auto btnSpr = typeinfo_cast<ButtonSprite*>(cmi->getChildren()->objectAtIndex(0));
		if (!btnSpr) return false;
		auto btnChildren = btnSpr->getChildren();
		return (static_cast<CCSprite*>(btnChildren->objectAtIndex(0))->getColor().r < 128);
	}

	void activateButton(CreateMenuItem* cmi, bool deactivate=false) {
		float ratio = deactivate ? 2.0 : 0.5;
		auto btnSpr = typeinfo_cast<ButtonSprite*>(cmi->getChildren()->objectAtIndex(0));
		if (!btnSpr) return;

		if (auto btnChildren = btnSpr->getChildren())
		for (unsigned i = 0; i < btnChildren->count(); i++) {
			auto child = typeinfo_cast<CCSprite*>(btnChildren->objectAtIndex(i));
			if (!child) continue;
			auto oldColor = child->getColor();
			uint8_t newR = oldColor.r * ratio;
			uint8_t newG = oldColor.g * ratio;
			uint8_t newB = oldColor.b * ratio;
			child->setColor(ccc3(newR, newG, newB));

			if (auto children2 = child->getChildren())
			for (unsigned j = 0; j < children2->count(); j++) {
				child = typeinfo_cast<CCSprite*>(children2->objectAtIndex(j));
				if (!child) continue;
				oldColor = child->getColor();
				newR = oldColor.r * ratio;
				newG = oldColor.g * ratio;
				newB = oldColor.b * ratio;
				child->setColor(ccc3(newR, newG, newB));
			}
		}
	}

	CCNode* createMenu(Group* group, CreateMenuItem* groupButton, float controlScale, bool useBackground=true) {
		auto baseNode = CCNode::create();
		baseNode->setID("RaZooM");
		auto groupSize = group->m_objectIds.size();

		auto buttonArray = CCArray::create();
		for (unsigned i = 0; i < groupSize; i++) {
			auto objId = group->m_objectIds.at(i);
			auto btnCol = darkerButtonBgObjIds.contains(objId) ? DARKER_ITEM_COLOR : GROUP_ITEM_COLOR;
			auto btn = GLOBAL.editorUI->getCreateBtn(objId, btnCol);
			btn->setUserObject(new GroupItemInfo(groupButton, btn->m_pfnSelector));
			btn->m_pfnSelector = menu_selector(MyEditButtonBar::onGroupItemClick); 
			buttonArray->addObject(btn);
		}
		registerButtons(buttonArray);

		auto bar = CCMenu::create();
		bar->setContentSize({0,0});
		// auto bar = EditButtonBar::create(buttonArray, ccp(0,0), 0, true, 1, groupSize); // buttons, shift, ?, ?, columns, rows
		baseNode->addChild(bar);
		bar->setPosition({0,0});

		if (!buttonArray->count()) return baseNode; // empty
		auto firstButton = static_cast<CreateMenuItem*>(buttonArray->objectAtIndex(0));

		// fix all scaling and positioning issues
		float scale = static_cast<CreateMenuItem*>(buttonArray->objectAtIndex(0))->getScale();
		float barScale = controlScale / scale;
		bar->setScale(barScale);

		float firstY = firstButton->getPositionY();
		float shiftUp =  (firstButton->getContentHeight() + 5) * scale * 1.5;
		float zeroPosition = firstY + shiftUp; // zero point
		float oneDistance = (firstButton->getContentHeight() + 5) * scale; // distance between two button centers

		int columnCount = ceil(groupSize / (double) GLOBAL.m_settings.m_maxMenuHeight);
		int rowCount = ceil(groupSize / (double) columnCount);
		float centerShiftX = (columnCount - 1) * 0.5f * oneDistance;

		int rowIter = 0;
		int columnIter = 0;

		for (unsigned i = 0; i < groupSize; i++) {
			auto btn = static_cast<CreateMenuItem*>(buttonArray->objectAtIndex(i));
			bar->addChild(btn);
			btn->setPositionY(zeroPosition + rowIter * oneDistance - 10);
			btn->setPositionX(btn->getPositionX() + columnIter * oneDistance - centerShiftX);
			rowIter++;
			if (rowIter == rowCount) {
				columnIter++;
				rowIter = 0;
			}
		}
		// log::debug("scale = {} / {} = {}", controlScale, scale,  barScale);

		// create background
		if (GLOBAL.m_settings.m_showBg) {
			float top, bottom, left, right;
			left = right = firstButton->getPositionX();
			top = bottom = firstButton->getPositionY();
			for (unsigned i = 1; i < groupSize; i++) {
				auto btn = static_cast<CreateMenuItem*>(buttonArray->objectAtIndex(i));
				float posX = btn->getPositionX();
				float posY = btn->getPositionY();
				if (posY > top) top = posY;
				else if (posY < bottom) bottom = posY;
				if (posX > right) right = posX;
				else if (posX < left) left = posX;
			}
			const float scaleFactor = 2; // for CCScale9Sprite not to be destroyed 
			auto bg = CCScale9Sprite::create("square02b_001.png", {0, 0, 80, 80});
			auto col = GLOBAL.m_settings.m_bgColor;
			bar->addChild(bg);
			bg->setColor(ccc3(col.r, col.g, col.b));
			bg->setOpacity(col.a);
			bg->setZOrder(-10);
			bg->setContentSize({((right - left) + 1.5f * oneDistance) * scaleFactor, 
				((top - bottom) + 1.5f * oneDistance) * scaleFactor});
			bg->setPosition({0, (top + bottom) / 2});
			bg->setScale(1 / scaleFactor);
		}
		return baseNode;
	}
};



class $modify(MyEditorUI, EditorUI) {
	struct Fields {
		std::string m_loadErrorText;
	};

	void showErrorAfterTransition(float) {
		auto scene = CCScene::get();
		if (typeinfo_cast<CCTransitionScene*>(scene)) {
			// means we are in transition scene now
			return;
		}
		auto winWidth = CCDirector::sharedDirector()->getWinSize().width;
		geode::createQuickPopup(
			"Object Groups load config errors",
			m_fields->m_loadErrorText,
			"Close", nullptr, winWidth * .95,
			nullptr, true, true
		);

		this->unschedule(schedule_selector(MyEditorUI::showErrorAfterTransition));
	}

	$override 
	bool init(LevelEditorLayer *editorLayer) {
		GLOBAL.editorUI = this;
		GLOBAL.updateSettings();

		std::vector<std::string> loadConfigErrors;
		bool loadConfigSuccess = readConfigFromJson(GLOBAL.m_settings.m_customConfig, &loadConfigErrors);
		
		if(!EditorUI::init(editorLayer)) return false;

		// prevent overlapping with my menus
		this->getChildByID("build-tabs-menu")->setZOrder(6); 

		this->getChildByID("editor-buttons-menu")->setZOrder(6);
		this->getChildByID("layer-menu")->setZOrder(6);

		if (!loadConfigSuccess) {
			std::string text = "<co>Errors occurred</c> while loading the <cg>custom configuration</c> for \"<cl>Object Groups</c>\" mod:\n";
			text += "<cr>";
			for (auto& err : loadConfigErrors) text += err + "\n";
			text += "</c>";
			text += "<cy>Tip:</c> Fix configuration file, than re-enter the editor for config re-load. Or delete this file and restart the game - the new file will be created and initialized with the default configuration.\n";
			text += "<cp>Config file path:</c> ";
			text += GLOBAL.m_settings.m_customConfig;
			m_fields->m_loadErrorText = text;
			this->schedule(schedule_selector(MyEditorUI::showErrorAfterTransition), 0);
		}
		log::debug("total items: {}", this->m_createButtonArray->count());
		return true;
	}

	$override
	CreateMenuItem* getCreateBtn(int id, int bg) {
		auto ret = EditorUI::getCreateBtn(id, bg);
		if (ret) this->m_createButtonArray->removeLastObject();
		return ret;
	}
};
