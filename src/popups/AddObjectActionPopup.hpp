#pragma once

#include "ObjectGroups.hpp"

#include <Geode/ui/GeodeUI.hpp>

class AddObjectActionPopup : public Popup {
private:
    const float m_width = 290.f;
    const float m_height = 150.f;

    std::function<void()> m_onAddMulti = nullptr;
    std::function<void()> m_onAddCustom = nullptr;

protected:

    bool init(std::function<void()> onAddMulti, std::function<void()> onAddCustom, int count) {
        if (!Popup::init(m_width, m_height, "GJ_square01.png"))
            return false;

        m_onAddMulti = onAddMulti;
        m_onAddCustom = onAddCustom;
            
        // m_closeBtn->setVisible(false);
        auto str = fmt::format("You've selected {} objects.\nWhat do you want to create?", count);
        setTitle(str, "goldFont.fnt", 0.7f, 35.f);

        float scale1 = 0.8, scale2 = 0.7, width = 120, height = 80;

        CCSprite* spr = ButtonSprite::create("Multiple\nobjects", width, width, scale1, true, "bigFont.fnt", "GJ_button_04.png", height);
        spr->setScale(scale2);
        auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(AddObjectActionPopup::onAddMultiFunc));
        m_buttonMenu->addChildAtPosition(btn, Anchor::Top, ccp(-60, -95));
        
        spr = ButtonSprite::create("Custom\nobject", width, width, scale1, true, "bigFont.fnt", "GJ_button_05.png", height);
        spr->setScale(scale2);
        btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(AddObjectActionPopup::onAddCustomFunc));
        m_buttonMenu->addChildAtPosition(btn, Anchor::Top, ccp(60, -95));
        
        // spr = ButtonSprite::create("Cancel", width, width, scale1, true, "bigFont.fnt", "GJ_button_04.png", height);
        // spr->setScale(scale2);
        // btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(AddObjectActionPopup::onClose));
        // m_buttonMenu->addChildAtPosition(btn, Anchor::Top, ccp(0, -160));

        return true;
    }

public:
    static AddObjectActionPopup* create(std::function<void()> onAddMulti, std::function<void()> onAddCustom, int count) {
        auto ret = new AddObjectActionPopup();
        if (ret && ret->init(onAddMulti, onAddCustom, count)) {
            ret->autorelease();
            return ret; 
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

private:
    void onAddMultiFunc(CCObject*) {
        if (m_onAddMulti) m_onAddMulti();
        onClose(nullptr);
    }

    void onAddCustomFunc(CCObject*) {
        if (m_onAddCustom) m_onAddCustom();
        onClose(nullptr);
    }
};

