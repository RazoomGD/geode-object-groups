#include "EditorUI.hpp"

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


$override
bool MyEditorUI::init(LevelEditorLayer* editorLayer) {
	Global::get().m_editorUI = this;
	Global::get().m_isEditMode = false;
	Global::get().m_settings.update();
	
	if (!EditorUI::init(editorLayer)) return false;

	// prevent overlapping with my menus
	if (auto ch = this->getChildByID("build-tabs-menu")) ch->setZOrder(6); 
	if (auto ch = this->getChildByID("editor-buttons-menu")) ch->setZOrder(6);
	if (auto ch = this->getChildByID("layer-menu")) ch->setZOrder(6);

	setupExtraTabs(Global::get().m_settings.m_extraTabsCount);

	const float scale = getBetterEditInterfaceScale();

	m_fields->rowMenu = setupRowMenu(scale);
	m_fields->toggleMenu = setupToggleMenu(scale);

	auto frame = CCSprite::create("OG_button_frame.png"_spr);
	frame->setAnchorPoint({0,0});
	m_fields->buttonFrame = CCNode::create();
	m_fields->buttonFrame->addChild(frame);
	m_fields->buttonFrame->setID("razoom.object-groups.frame");

	toggleEditGroupsMode(nullptr);
	toggleEditGroupsMode(nullptr);

	return true;
}

$override
void MyEditorUI::toggleMode(CCObject* sender) {
	EditorUI::toggleMode(sender);
	if (auto menu = m_fields->rowMenu) {
		menu->setVisible(sender == m_buildModeBtn && Global::get().m_isEditMode);
		m_fields->toggleMenu->setVisible(sender == m_buildModeBtn);
	}
}

$override
void MyEditorUI::updateCreateMenu(bool p0) {
	EditorUI::updateCreateMenu(p0);
	
	// darken all buttons with the selected object 
	// (as we now can have more buttons of type than 1)
	int indx = m_selectedObjectIndex;
	if (indx > 0) {
		CCArrayExt<CreateMenuItem*> buttons = m_createButtonArray;
		for (auto* btn : buttons) {
			if (btn->m_objectID == indx) {
				setColorToCreateBtnNew(btn, false);
			}
		}
	}
}

$override
void MyEditorUI::onCreateButton(CCObject* sender) {
	EditorUI::onCreateButton(sender);
	auto cmi = typeinfo_cast<CreateMenuItem*>(sender);
	if (cmi && m_selectedObjectIndex == cmi->m_objectID) {
		setSelectedCmi(cmi);
	} else {
		setSelectedCmi(nullptr);
	}
}