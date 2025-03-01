#include "Group.hpp"


class GroupSearchPopup : public Popup<void*> {
private:
    TextInput* m_input;

protected:
    bool setup(void*) override {
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

    static GroupSearchPopup* create(void*) {
        auto ret = new GroupSearchPopup();
        if (ret && ret->initAnchored(120, 88, 0)) {
            ret->autorelease();
            return ret; 
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};