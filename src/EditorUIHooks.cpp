#include "EditorUI.hpp"
#include "UpdateNotificationManager.hpp"

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


void MyEditorUI::showUI(bool show) {
	EditorUI::showUI(show);
	if (auto children = m_fields->rowMenu->getChildren()) {
		for (int i = 0; i < children->count(); i++) {
			auto btn = static_cast<CCNode*>(children->objectAtIndex(i));
			btn->setVisible(show);
		}
	}
	if (auto children = m_fields->toggleMenu->getChildren()) {
		for (int i = 0; i < children->count(); i++) {
			auto btn = static_cast<CCNode*>(children->objectAtIndex(i));
			btn->setVisible(show);
		}
	}
}

// CreateMenuItem* MyEditorUI::getCreateBtn(int id, int bg) {
// 	log::debug("call");
// 	auto ret = EditorUI::getCreateBtn(id, bg);
// 	log::debug("ret");
// 	return ret;
// }

bool MyEditorUI::init(LevelEditorLayer* editorLayer) {
	Global::get().m_editorUI = this;
	Global::get().m_isEditMode = false;
	Global::get().m_hasUnsavedOGChanges = false;
	Global::get().m_settings.update();
	
	if (!EditorUI::init(editorLayer)) return false;

	// prevent overlapping with my menus
	getChildByID("build-tabs-menu")->setZOrder(6); 
	getChildByID("editor-buttons-menu")->setZOrder(6);
	getChildByID("layer-menu")->setZOrder(6);

	setupExtraTabs(Global::get().m_settings.m_extraTabsCount);
	setupVanillaTabs();

	const float scale = getBetterEditInterfaceScale();

	m_fields->rowMenu = setupRowMenu(scale);
	m_fields->toggleMenu = setupToggleMenu(scale);

	auto frame = CCSprite::create("OG_button_frame.png"_spr);
	frame->setAnchorPoint({0,0});
	m_fields->buttonFrame = CCNode::create();
	m_fields->buttonFrame->addChild(frame);
	m_fields->buttonFrame->setID("frame"_spr);

	toggleEditGroupsMode(nullptr);
	toggleEditGroupsMode(nullptr);

	if (Global::get().m_isFirstEditorEnter || !Global::get().m_isCurrentVersionSafe) {
		// notify user about an important update
		UpdateNotificationManager::get()->goodMorning();
		Global::get().m_isFirstEditorEnter = false;
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
	auto cmi = typeinfo_cast<CreateMenuItem*>(sender);
	if (cmi && m_selectedObjectIndex == cmi->m_objectID) {
		setFocusedCmi(cmi);
	} else {
		setFocusedCmi(nullptr);
	}
	if (indexBefore != m_selectedObjectIndex) {
		setNewSelectedGroupCmi(nullptr); // deselect
	}

	log::debug("on create button");
}