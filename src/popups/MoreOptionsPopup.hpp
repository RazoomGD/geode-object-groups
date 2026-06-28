#pragma once

#include "ObjectGroups.hpp"
#include "FileOptionsPopup.hpp"

#include <Geode/ui/GeodeUI.hpp>

class MoreOptionsPopup : public Popup {
private:
    const float m_width = 220.f;
    const float m_height = 220.f;

protected:

    bool init() override {
        if (!Popup::init(m_width, m_height))
            return false;
            
        m_closeBtn->setVisible(false);
        setTitle("Advanced Options");

        auto menu = m_buttonMenu;

        auto okBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("ok", "goldFont.fnt", "GJ_button_01.png", 1),
            this, menu_selector(MoreOptionsPopup::onClose));
        menu->addChildAtPosition(okBtn, Anchor::Bottom);

        auto infoSpr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
        infoSpr->setScale(0.75);
        auto infoBtn = CCMenuItemSpriteExtra::create(
            infoSpr, this, menu_selector(MoreOptionsPopup::onInfoBtn));
        menu->addChildAtPosition(infoBtn, Anchor::TopRight, ccp(-18, -18));

        const float scale1 = 0.8, scale2 = 0.35 / scale1; // adjust button padding
        CCSprite* spr = ButtonSprite::create("Create tab icon from\nselected objects", "bigFont.fnt", "GJ_button_01.png", scale1);
        spr->setScale(scale2);
        auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(MoreOptionsPopup::onCreateTabIcon));
        menu->addChildAtPosition(btn, Anchor::Top, ccp(0, -55));
        
        spr = ButtonSprite::create("Create group\nfrom layout", "bigFont.fnt", "GJ_button_03.png", scale1);
        spr->setScale(scale2);
        btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(MoreOptionsPopup::onGroupFromLayout));
        menu->addChildAtPosition(btn, Anchor::Top, ccp(-btn->getScaledContentWidth()/2-5, -95));
        
        spr = ButtonSprite::create("New custom\nobject", "bigFont.fnt", "GJ_button_03.png", scale1);
        spr->setScale(scale2);
        btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(MoreOptionsPopup::onAddAsSingleCustomObject));
        menu->addChildAtPosition(btn, Anchor::Top, ccp(btn->getScaledContentWidth()/2+5, -95));

        spr = ButtonSprite::create("Copy obj/grp\nto clipboard", "bigFont.fnt", "GJ_button_05.png", scale1);
        spr->setScale(scale2);
        btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(MoreOptionsPopup::onCopyGroupAsJson));
        menu->addChildAtPosition(btn, Anchor::Top, ccp(-btn->getScaledContentWidth()/2-5, -135));
        
        spr = ButtonSprite::create("Copy tab to\nclipboard", "bigFont.fnt", "GJ_button_05.png", scale1);
        spr->setScale(scale2);
        btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(MoreOptionsPopup::onCopyTabAsJson));
        menu->addChildAtPosition(btn, Anchor::Top, ccp(btn->getScaledContentWidth()/2+5, -135));

        spr = ButtonSprite::create("Paste group(s)\nfrom clipboard", "bigFont.fnt", "GJ_button_04.png", scale1);
        spr->setScale(scale2);
        btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(MoreOptionsPopup::onPasteGroupsFromJson));
        menu->addChildAtPosition(btn, Anchor::Top, ccp(0, -175));

        spr = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
        spr->setScale(0.6f);
        btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(MoreOptionsPopup::onModSettings));
        menu->addChildAtPosition(btn, Anchor::BottomRight, ccp(-30, 30));

		spr = CircleButtonSprite::create(CCLabelBMFont::create("FILE", "goldFont.fnt"), CircleBaseColor::Green, CircleBaseSize::Small);
		spr->setScale(0.8f);
		btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(MoreOptionsPopup::onFileBtn));
        menu->addChildAtPosition(btn, Anchor::BottomLeft, ccp(30, 30));
        
        return true;
    }

    void onClose(CCObject* sender) override {
        Popup::onClose(sender);
    }

public:

    static MoreOptionsPopup* create() {
        auto ret = new MoreOptionsPopup();
        if (ret && ret->init()) {
            ret->autorelease();
            return ret; 
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

protected:

    void onCreateTabIcon(CCObject*) {
        Global::editor()->createIconForTheTabFromSelectedObjects();
        onClose(nullptr);
    }

    void onGroupFromLayout(CCObject*) {
        Global::editor()->onNewGroupFromLayoutButton(nullptr);
        onClose(nullptr);
    }

    void onAddAsSingleCustomObject(CCObject*) {
        Global::editor()->onAddAsSingleCustomObjectButton(nullptr);
        onClose(nullptr);
    }

    void onModSettings(CCObject*) {
        auto popup = openSettingsPopup(Mod::get(), true);
        if (popup) popup->setID("mod-settings"_spr);
        this->schedule(schedule_selector(MoreOptionsPopup::controlChangedSettings));
    }

    void controlChangedSettings(float) {
        if (CCScene::get()->getChildByID("mod-settings"_spr)) {
            return;
        }
        
        this->unschedule(schedule_selector(MoreOptionsPopup::controlChangedSettings));

        // handle changed settings
        Global::OGSettings _new;
        _new.update();
        auto &old = Global::get().m_settings;

        // editor reload required
        bool editorReloadReq = (
            old.m_groupBtnColor != _new.m_groupBtnColor||
            old.m_extraTabs != _new.m_extraTabs ||
            // old.m_enableSearchTab != _new.m_enableSearchTab ||
            old.m_enableGoToObject != _new.m_enableGoToObject ||
            old.m_appendDeleted != _new.m_appendDeleted || 
            old.m_coloredCustomObjects != _new.m_coloredCustomObjects
        );
        if (editorReloadReq) {
            alert("Some of the changed settings <co>require editor reload</c>");
        }

        // group reload required
        bool groupReloadReq = (
            old.m_showNames != _new.m_showNames ||
            old.m_font != _new.m_font ||
            old.m_pinButton != _new.m_pinButton
        );
        if (groupReloadReq) {
            old.m_showNames = _new.m_showNames;
            old.m_font = _new.m_font;
            old.m_pinButton = _new.m_pinButton;
            Global::editor()->execForeachGroup([](Group* g, int){g->setUpdateRequired(true);});
        }

        // pin layer reload is required
        if (old.m_groupCustomScale != _new.m_groupCustomScale) {
            old.m_groupCustomScale = _new.m_groupCustomScale;
            Global::editor()->m_fields->pinnedGroupsNode->setScale(getTabScale());
        }
        
        // nothing is required
        old.m_pinGestures = _new.m_pinGestures;
        old.m_autoClose = _new.m_autoClose;
        old.m_shiftAdd = _new.m_shiftAdd;
        old.m_keepPinned = _new.m_keepPinned;
        old.m_groupBgColor = _new.m_groupBgColor;
        old.m_groupBgHoverColor = _new.m_groupBgHoverColor;
        old.m_hoverMode = _new.m_hoverMode;
        old.m_ignoreHoveredGroups = _new.m_ignoreHoveredGroups;
    }


    void onCopyGroupAsJson(CCObject*) {
        auto editor = Global::editor();
        if (auto cmi = editor->getFocusedCmi()) {
            auto res = copyGroupAsJson(cmi);
            clipboard::write(res);
            shortAlert("Copied!");
            onClose(nullptr);
            return;
        }
        alert("<cr>Button is not selected</c>");
    }


    void onCopyTabAsJson(CCObject*) {
        auto editor = Global::editor();
        if (auto bar = editor->getCurrentTabIfAllowed()) {
            auto res = copyTabAsJson(bar);
            clipboard::write(res);
            shortAlert("Copied!");
            onClose(nullptr);
            return;
        }
        alert("You can't copy contents of the current tab, "
            "because it is not controlled by <cy>Object Groups</c>");
    }


    void onPasteGroupsFromJson(CCObject*) {
        auto str = clipboard::read();
        onClose(nullptr);
        pasteGroupsFromJsonToCurrentTab(str);
    }

    void onFileBtn(CCObject*) {
        onClose(nullptr);
        FileOptionsPopup::create()->show();
    }

    void onInfoBtn(CCObject*) {
        auto winWidth = CCDirector::sharedDirector()->getWinSize().width;
        createQuickPopup("More Options", 
            "- <co>Create tab icon from selected</c>: creates new icon for the current "
            "tab from selected objects (select nothing to reset to default)\n"
            "- <co>Create group from layout</c>: tries to put selected objects into a new group while "
            "preserving their relative positions from editor\n"
            "- <co>New custom object</c>: creates a <cy>custom object</c> "
            "from selected objects and adds it to the tab\n"
            "- <co>Copy object/group/tab</c>: copies focused button (object or group) or entire tab to "
            "the clipboard as <cl>json</c>\n"
            "- <co>Paste group(s)</c>: adds groups from <cl>json</c> content of clipboard to the current tab\n"
            "- <cp>Note</c>: You can use copy/paste json options to share you groups with other creators",
            
            "ok", nullptr, winWidth * .8, nullptr, true, true
        );
    }
    
};

