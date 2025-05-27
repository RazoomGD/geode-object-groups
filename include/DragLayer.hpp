#pragma once

#include "Group.hpp"

class GroupDragLayer : public CCLayer {
private:
    Group* m_group = nullptr;
    CCScale9Sprite* m_overlay = nullptr;
    CCPoint m_relativeCursorPos;

public:
    static GroupDragLayer* create(Group* group) {
        auto ret = new GroupDragLayer;
        if (ret && ret->init(group)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }


    bool init(Group* group) {
        if (!CCLayer::init()) return false;
        m_group = group;
        m_overlay = CCScale9Sprite::create("square02b_001.png", {0, 0, 80, 80});
        m_overlay->setScale(0.5);
        m_overlay->setOpacity(0);
        m_overlay->setAnchorPoint({0,0});
        addChild(m_overlay);
        
        setTouchMode(kCCTouchesOneByOne);
        setTouchEnabled(true);
        setTouchPriority(-128);
        ignoreAnchorPointForPosition(false);

        return true;
    }


    void setContentSize(const CCSize& sz) override {
        CCLayer::setContentSize(sz);
        if (m_overlay) m_overlay->setContentSize(sz * 2);
    }


    bool ccTouchBegan(CCTouch* touch, CCEvent* event) override {
        if (!static_cast<MyEditorUI*>(EditorUI::get())->m_fields->pinnedGroups->isVisible()) {
            return false;
        }
        if (!isGroupVisibleOnScreen(m_group)) {
            return false;
        }
        auto const point = m_group->convertToNodeSpace(touch->getLocation());
        if (this->boundingBox().containsPoint(point)) {
            if (!m_group->isPinned()) {
                if (!Global::get().m_settings.m_pinGestures) {
                    return false;
                }
                m_group->switchPinState(); // pin
            }
            m_overlay->setOpacity(100);
            m_overlay->setColor(ccc3(250, 250, 250));
            m_relativeCursorPos = point;
            return true;
        }
        return false;
    }


    bool isOutOfWindow() {
        auto pos = m_group->getPosition();
        auto sc = m_group->getParent()->getScale();
        auto win = CCDirector::get()->getWinSize();
        return (pos.y + getPositionY() - 0.5 * 0.5 * getContentHeight()) < 0 || 
            (pos.x - 0.5 * 0.5 * getContentWidth()) < 0 || 
            (pos.x + 0.5 * 0.5 * getContentWidth()) > win.width / sc ||
            (pos.y + getPositionY() + 0.5 * 0.5 * getContentHeight()) > win.height / sc;
    }


    void ccTouchMoved(CCTouch* touch, CCEvent* event) override {
        if (!m_group->getParent()) return;

        auto const point = m_group->convertToNodeSpace(touch->getLocation());
        auto const delta = m_relativeCursorPos - point;
        m_group->setPosition(m_group->getPosition() - delta);

        if (Global::get().m_settings.m_pinGestures) {
            if (isOutOfWindow()) {
                m_overlay->setColor(ccc3(250, 0, 0));
            } else {
                m_overlay->setColor(ccc3(250, 250, 250));
            }
        }
    }


    void ccTouchEnded(CCTouch* touch, CCEvent* event) override {
        if (!m_group->getParent()) return;
        m_overlay->setOpacity(0);
        if (Global::get().m_settings.m_pinGestures && isOutOfWindow()) {
            m_group->switchPinState(); // unpin
        }
    }
};
