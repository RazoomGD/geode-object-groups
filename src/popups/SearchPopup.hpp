#pragma once

#include "Group.hpp"

class GroupSearchPopup : public Popup {
private:
    TextInput* m_input;
    float m_width = 120;
    float m_height = 88;

protected:
    bool init() override {
        if (!Popup::init(m_width, m_height))
            return false;

        setTitle("Search groups:");

        const float height= 50;
        const float width = CCDirector::sharedDirector()->getWinSize().width * 2 / 3;
        
        m_input = TextInput::create(width, "(empty)");
        m_input->setCommonFilter(CommonFilter::Any);
        m_input->setCallback([](const std::string& str) {
            Global::editor()->performSearchResult(str);
        });
        m_mainLayer->addChildAtPosition(m_input, Anchor::Center);
        m_input->getBGSprite()->setOpacity(155);
        m_input->focus();

        auto deleteIconSpr = CCSprite::createWithSpriteFrameName("GJ_deleteIcon_001.png");
        deleteIconSpr->setScale(0.7);
        auto deleteIconBtn = CCMenuItemSpriteExtra::create(deleteIconSpr, this, menu_selector(GroupSearchPopup::onClose));
        m_buttonMenu->addChildAtPosition(deleteIconBtn, Anchor::Center, {width / 2 - 15, 0});

        m_bgSprite->setVisible(false);
        m_closeBtn->setVisible(false);
        this->setOpacity(0);
      
        return true;
    }

    
    bool ccTouchBegan(CCTouch*, CCEvent*) override {
        if (m_input->getInputNode()->m_selected) {
            return true; // swallow
        }
        return false;
    }
    void ccTouchMoved(CCTouch*, CCEvent*) override {}
    void ccTouchEnded(CCTouch*, CCEvent*) override {}
    void ccTouchCancelled(CCTouch*, CCEvent*) override {}
    
public:

    void onClose(CCObject* sender) override {
        Popup::onClose(sender);
    }

    static GroupSearchPopup* create() {
        auto ret = new GroupSearchPopup();
        if (ret && ret->init()) {
            ret->autorelease();
            return ret; 
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};