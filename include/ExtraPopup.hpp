#include "Group.hpp"


// specify parameters for the setup function in the Popup<...> template
class ExtraOptionsPopup : public Popup<Group*> {
private:
    Ref<Group> m_myGroup;
    CCLabelBMFont* m_groupSizeLabel;
    const float m_width = 260.f;
    const float m_height = 210.f;
protected:
    bool setup(Group* group) override {
        m_myGroup = group;
        m_closeBtn->setVisible(false);
        setTitle("Extra Group Options");

        auto menu = CCMenu::create();
        menu->setContentSize(m_mainLayer->getContentSize());
        m_mainLayer->addChildAtPosition(menu, Anchor::Center);

        auto infoSpr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
        infoSpr->setScale(0.75);
        auto infoBtn = CCMenuItemSpriteExtra::create(
            infoSpr, this, menu_selector(ExtraOptionsPopup::onInfoBtn));
        menu->addChildAtPosition(infoBtn, Anchor::TopRight, ccp(-18, -18));
        
        auto okBtn = CCMenuItemSpriteExtra::create(
            ButtonSprite::create("ok", "goldFont.fnt", "GJ_button_01.png", 1),
            this, menu_selector(ExtraOptionsPopup::onClose));
        menu->addChildAtPosition(okBtn, Anchor::Bottom);
        
        auto nameLabel = CCLabelBMFont::create("Name: ", "bigFont.fnt");
        m_mainLayer->addChild(nameLabel);
        nameLabel->setAnchorPoint({0,0.5});
        nameLabel->setScale(0.5);
        nameLabel->setPosition(ccp(20, m_height - 55));
        
        auto nameInput = TextInput::create(
            (m_width - nameLabel->getScaledContentWidth() - 20 - 5 - 20) / 0.8, "(empty)");
        nameInput->setString(group->getName(), false);
        nameInput->setCommonFilter(CommonFilter::Any);
        nameInput->setCallback([nameInput, group](const std::string& str) {
            auto name = toValidString(str.c_str());
            nameInput->setString(name, false);
            group->setName(name);
        });
        m_mainLayer->addChild(nameInput);
        nameInput->setAnchorPoint({0,0.5});
        nameInput->setScale(0.8);
        nameInput->setPosition(ccp(nameLabel->getScaledContentWidth() + 5 + 20, m_height - 55));

        m_groupSizeLabel = CCLabelBMFont::create("", "bigFont.fnt");
        m_mainLayer->addChild(m_groupSizeLabel);
        m_groupSizeLabel->setScale(0.3);
        m_groupSizeLabel->setPosition(ccp(m_width / 2, m_height - 84));
        updateSizeLabel();
        
        const float scale1 = 0.8, scale2 = 0.35 / scale1; // adjust button padding
        auto spr = ButtonSprite::create("Add column right", "bigFont.fnt", "GJ_button_01.png", scale1);
        spr->setScale(scale2);
        auto btn1 = CCMenuItemSpriteExtra::create(spr, this, menu_selector(ExtraOptionsPopup::onAddColRight));
        spr = ButtonSprite::create("Add column left", "bigFont.fnt", "GJ_button_01.png", scale1);
        spr->setScale(scale2);
        auto btn2 = CCMenuItemSpriteExtra::create(spr, this, menu_selector(ExtraOptionsPopup::onAddColLeft));
        spr = ButtonSprite::create("Remove column", "bigFont.fnt", "GJ_button_06.png", scale1);
        spr->setScale(scale2);
        auto btn3 = CCMenuItemSpriteExtra::create(spr, this, menu_selector(ExtraOptionsPopup::onColRemove));
        spr = ButtonSprite::create("Add row below", "bigFont.fnt", "GJ_button_01.png", scale1);
        spr->setScale(scale2);
        auto btn4 = CCMenuItemSpriteExtra::create(spr, this, menu_selector(ExtraOptionsPopup::onAddRowBottom));
        spr = ButtonSprite::create("Add row above", "bigFont.fnt", "GJ_button_01.png", scale1);
        spr->setScale(scale2);
        auto btn5 = CCMenuItemSpriteExtra::create(spr, this, menu_selector(ExtraOptionsPopup::onAddRowTop));
        spr = ButtonSprite::create("Remove row", "bigFont.fnt", "GJ_button_06.png", scale1);
        spr->setScale(scale2);
        auto btn6 = CCMenuItemSpriteExtra::create(spr, this, menu_selector(ExtraOptionsPopup::onRowRemove));

        CCMenuItemSpriteExtra* buttons[6] = {btn5, btn4, btn6, btn1, btn2, btn3};
        float posX = 20;
        for (int i = 0; i < 2; i++) { // 2 columns, 3 rows
            float posY = m_height - 110;
            for (int j = 0; j < 3; j++) {
                auto btn = buttons[3*i+j];
                menu->addChild(btn);
                float offset = btn->getScaledContentWidth() / 2;
                btn->setPosition(posX + offset, posY);
                posY -= 20;
            }
            posX = m_width / 2;
        }

        spr = ButtonSprite::create("Set icon", "bigFont.fnt", "GJ_button_05.png", scale1);
        spr->setScale(scale2);
        auto btn7 = CCMenuItemSpriteExtra::create(spr, this, menu_selector(ExtraOptionsPopup::onSetIcon));
        menu->addChild(btn7);
        btn7->setPosition(20 + btn7->getScaledContentWidth() / 2, m_height - 180);

        return true;
    }

public:
    static ExtraOptionsPopup* create(Group* group) {
        auto ret = new ExtraOptionsPopup();
        if (ret->initAnchored(ret->m_width, ret->m_height, group)) {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }

private:
    void updateSizeLabel() {
        float h = m_myGroup->getMatrix().size();
        float w = (h > 0) ? m_myGroup->getMatrix()[0].size() : 0;
        m_groupSizeLabel->setString(fmt::format("Group size: Rows: {}. Columns: {}.", h, w).c_str());
    }

    void onClose(CCObject* sender) override {
        m_myGroup->updateMenu();
        Popup::onClose(sender);
    }

    void onInfoBtn(CCObject* sender) {
        auto winWidth = CCDirector::sharedDirector()->getWinSize().width;
        geode::createQuickPopup("Extra options explanation", 
"<co>- Group name</c>: name of the group. Name isn't shown if it's empty or \
if this option is disabled in mod settings.\n\
<co>- Add (...)</c>: create new empty row/column at the specified location \
relative to the <cj>focused button</c>.\n\
<co>- Remove (...)</c>: remove the row/column containing the <cj>focused button</c>.\n\
<co>- Set icon</c>: set object shown on the group button (for that, exactly 1 object must be selected).\n\
<co>Note</c>: If there are no <cj>focused button</c> within the group, <co>Add</c> and <co>Remove</c> \
buttons won't do anything",
            "ok", nullptr, winWidth * .8, nullptr, true, true
        );
    }

    void onAddColRight(CCObject* sender) {
        // code
    }

    void onAddColLeft(CCObject* sender) {
        // code
    }

    void onAddRowTop(CCObject* sender) {
        // code
    }

    void onAddRowBottom(CCObject* sender) {
        // code
    }

    void onColRemove(CCObject* sender) {
        // code
        onClose(sender);
    }

    void onRowRemove(CCObject* sender) {
        // code
        onClose(sender);
    }

    void onSetIcon(CCObject* sender) {
        auto selected = EditorUI::get()->m_selectedObject;
        if (selected == nullptr) {
            FLAlertLayer::create("Object Groups", "<cr>Icon not updated!</c> You must select \
exactly 1 object in editor to update group icon", "ok")->show();
        } else {
            short id = selected->m_objectID;
            m_myGroup->updateObjId(id);
        }
        onClose(sender);
    }
};
    