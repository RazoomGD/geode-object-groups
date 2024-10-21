#include "config.hpp"

#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include <Geode/modify/EditButtonBar.hpp>
#include <Geode/modify/EditorUI.hpp>

// colors: 1-green, 2-blue, 3-pink, 4-gray, 5-darker gray, 6-red
#define ITEM_COLOR 4
#define DARKER_ITEM_COLOR 5
#define GROUP_COLOR 1
#define GROUP_ITEM_COLOR 4

// global variables
struct {
	EditorUI* editorUI = nullptr;
} GLOBAL;

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
		struct {
			int m_maxMenuHeight = 4; // todo
		} m_settings;
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
			switch (group.m_objectIds.size()) {
				case 0: break;
				case 1: {
					// make simple item
					auto btnCol = darkerButtonBgObjIds.contains(group.m_thumbnailObjectId) ? DARKER_ITEM_COLOR : ITEM_COLOR;
					auto btn = GLOBAL.editorUI->getCreateBtn(group.m_thumbnailObjectId, btnCol);
					btn->setUserObject(new ItemInfo(btn->m_pfnSelector));
					btn->m_pfnSelector = menu_selector(MyEditButtonBar::onItemClick); 
					btn->m_objectID = group.m_objectIds.at(0);
					clearedIds.insert(btn->m_objectID);

					buttons->addObject(btn);
					break;
				}
				default: {
					// make group
					auto btn = GLOBAL.editorUI->getCreateBtn(group.m_thumbnailObjectId, GROUP_COLOR);
					btn->setUserObject(new GroupInfo(group));
					btn->m_pfnSelector = menu_selector(MyEditButtonBar::onGroupClick); 

					for (auto objId : group.m_objectIds) {
						clearedIds.insert(objId);
					}

					buttons->addObject(btn);
					break;
				}
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

		// if (!wasActive) {
		if (thisIsNowSelected) {
			if (!wasActive)
				activateButton(btn); // for some reason it does not change color itself
			if (!isButtonActivated(groupItemInfo->m_groupButton))
				activateButton(groupItemInfo->m_groupButton);
		} else {
			if (isButtonActivated(groupItemInfo->m_groupButton)) {
				activateButton(groupItemInfo->m_groupButton, true);
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
		auto groupSize = group->m_objectIds.size();

		auto buttonArray = CCArray::create();
		for (unsigned i = 0; i < groupSize; i++) {
			auto btn = GLOBAL.editorUI->getCreateBtn(group->m_objectIds.at(i), GROUP_ITEM_COLOR);
			btn->setUserObject(new GroupItemInfo(groupButton, btn->m_pfnSelector));
			btn->m_pfnSelector = menu_selector(MyEditButtonBar::onGroupItemClick); 
			buttonArray->addObject(btn);
		}

		auto bar = EditButtonBar::create(buttonArray, ccp(0,0), 0, true, 1, groupSize); // buttons, shift, ?, ?, columns, rows
		baseNode->addChild(bar);
		auto firstButton = static_cast<CreateMenuItem*>(buttonArray->objectAtIndex(0));

		// fix all scaling and positioning issues
		float scale = static_cast<CreateMenuItem*>(buttonArray->objectAtIndex(0))->getScale();
		float barScale = controlScale / scale;
		bar->setScale(barScale);

		float firstY = firstButton->getPositionY();
		float shiftUp =  (firstButton->getContentHeight() + 5) * scale * 1.5;
		float zeroPosition = firstY + shiftUp; // zero point
		float oneDistance = (firstButton->getContentHeight() + 5) * scale; // distance between two button centers

		int columnCount = ceil(groupSize / (double) m_fields->m_settings.m_maxMenuHeight);
		int rowCount = ceil(groupSize / (double) columnCount);
		float centerShiftX = (columnCount - 1) * 0.5f * oneDistance;

		int rowIter = 0;
		int columnIter = 0;

		for (unsigned i = 0; i < groupSize; i++) {
			auto btn = static_cast<CreateMenuItem*>(buttonArray->objectAtIndex(i));
			btn->setPositionY(zeroPosition + rowIter * oneDistance);
			btn->setPositionX(btn->getPositionX() + columnIter * oneDistance - centerShiftX);
			rowIter++;
			if (rowIter == rowCount) {
				columnIter++;
				rowIter = 0;
			}
		}
		// log::debug("scale = {} / {} = {}", controlScale, scale,  barScale);

		// create background
		if (useBackground) {
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
			const float scaleFactor = 10; // for CCScale9Sprite not to be destroyed 
			auto bg = CCScale9Sprite::create("square02_001.png", {0, 0, 80, 80});
			bar->addChild(bg);
			bg->setOpacity(140);
			bg->setZOrder(-10);
			bg->setContentSize({((right - left) + 1.5f * oneDistance) * scaleFactor, 
				((top - bottom) + 1.5f * oneDistance) * scaleFactor});
			bg->setPosition({0, (top + bottom) / 2 + firstButton->getParent()->getPositionY()});
			bg->setScale(1 / scaleFactor);
		}
		baseNode->setID("RaZooM");
		return baseNode;
	}
};



class $modify(MyEditorUI, EditorUI) {
	bool init(LevelEditorLayer *editorLayer) {
		GLOBAL.editorUI = this;
		if(!EditorUI::init(editorLayer)) return false;

		// prevent overlapping with my menus
		this->getChildByID("build-tabs-menu")->setZOrder(6); 

		this->getChildByID("editor-buttons-menu")->setZOrder(6);
		this->getChildByID("layer-menu")->setZOrder(6);

		return true;
	}
};