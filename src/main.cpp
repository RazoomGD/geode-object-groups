#include "config.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include <Geode/modify/EditButtonBar.hpp>
#include <Geode/modify/EditorUI.hpp>

#include <string>

// colors: 1-green, 2-blue, 3-pink, 4-gray, 5-darker gray, 6-red
#define ITEM_COLOR 4
#define GROUP_COLOR 1
#define GROUP_ITEM_COLOR 4

// global variables
struct {
	EditorUI* editorUI = nullptr;
} GLOBAL;

// parameters for group buttons
struct GroupInfo : CCObject {
	Group m_group;
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
	SEL_MenuHandler m_defaultSelector;
	GroupItemInfo(SEL_MenuHandler defaultSelector) : m_defaultSelector(defaultSelector) {
		this->autorelease();
	}
};



class $modify(MyEditButtonBar, EditButtonBar) {
	struct Fields {
		CCNode* m_menuNode = nullptr;
		CreateMenuItem* m_buttonWithGroupOpened = nullptr;
	};

	void removeOldMenuIfExists() {
		if (m_fields->m_menuNode) {
			m_fields->m_menuNode->removeFromParent();
			m_fields->m_menuNode = nullptr;
			m_fields->m_buttonWithGroupOpened = nullptr;
		}
	}

	$override 
	void loadFromItems(CCArray* p0, int p1, int p2, bool p3) {
		if (p0->count() == 0) return EditButtonBar::loadFromItems(p0, p1, p2, p3);
		
		// check first object to find out what category we are building now
		bool barCreated = false;
		if (auto cmi = typeinfo_cast<CreateMenuItem*>(p0->objectAtIndex(0))) {
			if (auto btnSpr = typeinfo_cast<ButtonSprite*>(cmi->getChildren()->objectAtIndex(0))) {
				auto btnChildren = btnSpr->getChildren();
				for (unsigned i = 0; i < btnChildren->count(); i++) {
					if (auto gameObj = typeinfo_cast<GameObject*>(btnChildren->objectAtIndex(i))) {
						// gg! found game object in children
						if (categoryByFirstObjectId.contains(gameObj->m_objectID)) {
							short category = (*categoryByFirstObjectId.find(gameObj->m_objectID)).second;
							barCreated = createCustomBarForCategory(category, p1, p2, p3);
							log::debug("Category: {}; done: {}", category, barCreated);
						}
						break;
					}
				}
			}
		}

		if (!barCreated) {
			EditButtonBar::loadFromItems(p0, p1, p2, p3);
		}
	};

	bool createCustomBarForCategory(short category, int p1, int p2, bool p3) {
		if (!CONFIG.contains(category)) return false;

		auto buttons = CCArray::create();
		auto groups = (*CONFIG.find(category)).second;
		for (auto& group : groups) {
			switch (group.m_objectIds.size()) {
				case 0: break;
				case 1: {
					// make simple item
					auto btn = GLOBAL.editorUI->getCreateBtn(group.m_thumbnailObjectId, ITEM_COLOR);
					btn->setUserObject(new ItemInfo(btn->m_pfnSelector));
					btn->m_pfnSelector = menu_selector(MyEditButtonBar::onItemClick); 
					btn->m_objectID = group.m_objectIds.at(0);

					buttons->addObject(btn);
					break;
				}
				default: {
					// make group
					auto btn = GLOBAL.editorUI->getCreateBtn(group.m_thumbnailObjectId, GROUP_COLOR);
					btn->setUserObject(new GroupInfo(group));
					btn->m_pfnSelector = menu_selector(MyEditButtonBar::onGroupClick); 

					buttons->addObject(btn);
					break;
				}
			}
		}
		// todo: add objects, that were not used in any of the groups
		EditButtonBar::loadFromItems(buttons, p1, p2, p3);
		return true;
	}

	void onItemClick(CCObject* sender) {
		auto btn = static_cast<CreateMenuItem*>(sender);
		auto defaultInfo = static_cast<ItemInfo*>(btn->getUserObject());
		auto method = static_cast<void (CCObject::*)(CCObject*)>(defaultInfo->m_defaultSelector);

		bool wasActive = isButtonActivated(btn);
		(this->*method)(sender); // call original selector
		if (!wasActive) {
			activateButton(btn); // for some reason it does not change color itself
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

		auto menu = createMenu(&groupInfo->m_group, btn->getScale());

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

		(this->*method)(sender); // call original selector

		removeOldMenuIfExists();
	}


	bool isButtonActivated(CreateMenuItem* cmi) {
		auto btnSpr = typeinfo_cast<ButtonSprite*>(cmi->getChildren()->objectAtIndex(0));
		if (!btnSpr) return false;
		auto btnChildren = btnSpr->getChildren();
		return (static_cast<CCSprite*>(btnChildren->objectAtIndex(1))->getColor().r < 128);
	}

	void activateButton(CreateMenuItem* cmi) {
		auto btnSpr = typeinfo_cast<ButtonSprite*>(cmi->getChildren()->objectAtIndex(0));
		if (!btnSpr) return;
		auto btnChildren = btnSpr->getChildren();
		for (unsigned i = 0; i < btnChildren->count(); i++) {
			auto child = static_cast<CCSprite*>(btnChildren->objectAtIndex(i));
			auto oldColor = child->getColor();
			uint8_t newR = oldColor.r * 0.5;
			uint8_t newG = oldColor.g * 0.5;
			uint8_t newB = oldColor.b * 0.5;
			child->setColor(ccc3(newR, newG, newB));
		}
	}

	CCNode* createMenu(Group* group, float controlScale, bool useBackground=true) {
		auto baseNode = CCNode::create();
		auto groupSize = group->m_objectIds.size();

		auto buttonArray = CCArray::create();
		for (unsigned i = 0; i < groupSize; i++) {
			auto btn = GLOBAL.editorUI->getCreateBtn(group->m_objectIds.at(i), GROUP_ITEM_COLOR);
			btn->setUserObject(new GroupItemInfo(btn->m_pfnSelector));
			btn->m_pfnSelector = menu_selector(MyEditButtonBar::onGroupItemClick); 
			buttonArray->addObject(btn);
		}

		auto bar = EditButtonBar::create(buttonArray, ccp(0,0), 0, true, 1, groupSize); // buttons, shift, ?, ?, columns, rows
		baseNode->addChild(bar);

		// fix all scaling and positioning issues
		float scale = static_cast<CreateMenuItem*>(buttonArray->objectAtIndex(0))->getScale();
		float barScale = controlScale / scale;
		bar->setScale(barScale);

		auto firstButton = static_cast<CreateMenuItem*>(buttonArray->objectAtIndex(0));
		float firstY = firstButton->getPositionY();
		float shiftUp =  (firstButton->getContentHeight() + 5) * scale * 1.5;
		float zeroPosition = firstY + shiftUp; // zero point
		float oneDistance = (firstButton->getContentHeight() + 5) * scale; // distance between two button centers
		for (unsigned i = 0; i < groupSize; i++) {
			auto btn = static_cast<CreateMenuItem*>(buttonArray->objectAtIndex(i));
			btn->setPositionY(zeroPosition + i * oneDistance);
		}
		// log::debug("scale = {} / {} = {}", controlScale, scale,  barScale);

		// create background
		

		baseNode->setID("RaZooM");
		return baseNode;
	}

	void createMenuBG(CCMenu * menu) { // todo 
		auto buttons = menu->getChildren();
		float firstBtnY;
		float lastBtnY;
		float maxBtnWidth = 0;
		for (unsigned i = 0; i < buttons->count(); i++) {
			auto btn = static_cast<CCMenuItemSpriteExtra*>(buttons->objectAtIndex(i));
			if (i == 0) firstBtnY = btn->getPositionY();
			lastBtnY = btn->getPositionY();
			if (btn->getContentWidth() > maxBtnWidth) maxBtnWidth = btn->getContentWidth();
		}
		// add base
		const float scaleFactor = 5; // for CCScale9Sprite not to be destroyed 
		auto base = CCScale9Sprite::create("square02_001.png", {0, 0, 80, 80});
		menu->addChild(base);
		base->setOpacity(140);
		base->setContentSize({(maxBtnWidth + 14) * scaleFactor, (abs(firstBtnY - lastBtnY) + 15) * scaleFactor});
		base->setPositionY((firstBtnY + lastBtnY) / 2 - 1);
		base->setPositionX(menu->getContentSize().width / 2 + 5);
		base->setScale(1 / scaleFactor);
		base->setZOrder(-10);
	}
};


class $modify(MyEditorUI, EditorUI) {
	bool init(LevelEditorLayer *editorLayer) {
		GLOBAL.editorUI = this;
		if(!EditorUI::init(editorLayer)) return false;

		this->getChildByID("build-tabs-menu")->setZOrder(6); // prevent overlapping with my menus
		return true;
	}

	// CreateMenuItem* getCreateBtn(int id, int bg) {
	// 	log::debug("id={}, bg={}", id, bg);
	// 	return EditorUI::getCreateBtn(id, bg);
	// }

	// void onMyButton(CCObject*) {
	// 	short objIds[] = {83, 69, 3, 4, 5, 502, 6, 7};
	// 	int btnWidth = 40;
	// 	auto myNode = CCNode::create();
	// 	myNode->setID("RaZooM");
	// 	LevelEditorLayer::get()->getChildByID("main-node")->addChild(myNode);
	// 	myNode->setPosition(CCDirector::sharedDirector()->getWinSize() / 2);

	// 	auto buttonArray = CCArray::create();
	// 	for (unsigned i = 0; i < 8; i++) {
	// 		auto obj = GameObject::createWithKey(objIds[i]);
	// 		auto btn = ButtonSprite::create(CCSprite::create(), btnWidth, false, btnWidth, "GJ_button_04.png", .5f);

	// 		btn->addChild(obj, 2);
	// 		obj->setPosition(ccp(btnWidth, btnWidth) / 2);

	// 		auto cmi = CreateMenuItem::create(btn, nullptr, this, menu_selector(
	// 			MyEditorUI::onCreateButton)); // sprite, disabledSprite, target, callback
	// 		cmi->m_objectID = objIds[i];

	// 		buttonArray->addObject(cmi);
	// 	}

	// 	auto bar = EditButtonBar::create(buttonArray, ccp(0,0), 0, true, 2, 3); // buttons, ?, ?, ?, columns, rows
		
	// 	myNode->addChild(bar);
	// }

	
};