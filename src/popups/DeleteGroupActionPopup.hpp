#pragma once

#include "ObjectGroups.hpp"

#include <Geode/ui/GeodeUI.hpp>

class DeleteGroupActionPopup : public Popup {
private:
    const float m_width = 290.f;
    const float m_height = 150.f;

    std::function<void()> m_onDelete = nullptr;
    std::function<void()> m_onUngroup = nullptr;

protected:

    bool init(std::function<void()> onDelete, std::function<void()> onUngroup) {
        if (!Popup::init(m_width, m_height, "GJ_square01.png"))
            return false;

        m_onDelete = onDelete;
        m_onUngroup = onUngroup;
            
        // m_closeBtn->setVisible(false);
        auto str = "Select delete mode for this group";
        setTitle(str, "goldFont.fnt", 0.7f, 35.f);

        float scale1 = 0.8, scale2 = 0.7, width = 120, height = 80;

        CCSprite* spr = ButtonSprite::create("Ungroup\nbuttons", width, width, scale1, true, "bigFont.fnt", "GJ_button_05.png", height);
        spr->setScale(scale2);
        auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(DeleteGroupActionPopup::onUngroupFunc));
        m_buttonMenu->addChildAtPosition(btn, Anchor::Top, ccp(-60, -95));
        
        spr = ButtonSprite::create("Delete\nbuttons", width, width, scale1, true, "bigFont.fnt", "GJ_button_06.png", height);
        spr->setScale(scale2);
        btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(DeleteGroupActionPopup::onDeleteFunc));
        m_buttonMenu->addChildAtPosition(btn, Anchor::Top, ccp(60, -95));
        
        return true;
    }

public:
    static DeleteGroupActionPopup* create(std::function<void()> onDelete, std::function<void()> onUngroup) {
        auto ret = new DeleteGroupActionPopup();
        if (ret && ret->init(onDelete, onUngroup)) {
            ret->autorelease();
            return ret; 
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

private:
    void onDeleteFunc(CCObject*) {
        if (m_onDelete) m_onDelete();
        onClose(nullptr);
    }

    void onUngroupFunc(CCObject*) {
        if (m_onUngroup) m_onUngroup();
        onClose(nullptr);
    }
};

