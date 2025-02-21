#include "ObjectGroups.hpp"

class MoreOptionsPopup : public Popup<void*> {
private:
    const float m_width = 260.f;
    const float m_height = 210.f;

protected:
    bool setup(void*) override {
        m_closeBtn->setVisible(false);
        setTitle("Advanced Options");

        auto menu = CCMenu::create();
        menu->setContentSize(m_mainLayer->getContentSize());
        m_mainLayer->addChildAtPosition(menu, Anchor::Center);

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
        auto spr = ButtonSprite::create("Create tab icon from\nselected objects", "bigFont.fnt", "GJ_button_01.png", scale1);
        spr->setScale(scale2);
        auto btn1 = CCMenuItemSpriteExtra::create(spr, this, menu_selector(MoreOptionsPopup::onCreateTabIcon));
        menu->addChildAtPosition(btn1, Anchor::Top, ccp(0, -50));

        
        return true;
    }

    void onClose(CCObject* sender) override {
        Popup::onClose(sender);
    }

public:
    static MoreOptionsPopup* create() {
        auto ret = new MoreOptionsPopup();
        if (ret && ret->initAnchored(ret->m_width, ret->m_height, 0)) {
            ret->autorelease();
            return ret; 
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

private:
    void onCreateTabIcon(CCObject*) {
        Global::get().m_editorUI->createIconForTheTabFromSelectedObjects();
    }

    void onInfoBtn(CCObject*) {
        auto winWidth = CCDirector::sharedDirector()->getWinSize().width;
        createQuickPopup("More Options", 
            "todo: description", // todo
            "ok", nullptr, winWidth * .8, nullptr, true, true
        );
    }
    
};