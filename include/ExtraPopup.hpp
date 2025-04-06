#include "Group.hpp"


// specify parameters for the setup function in the Popup<...> template
class ExtraOptionsPopup : public Popup<Group*> {
private:
    Ref<Group> m_myGroup;
    struct {
        bool m_groupHasSelectedCmi;
        uint32_t m_row;
        uint32_t m_column;
    } m_groupCmiInfo;

    Ref<CCArray> m_addAndRemoveButtons;
    CCLabelBMFont* m_groupSizeLabel;
    const float m_width = 260.f;
    const float m_height = 210.f;

protected:
    bool setup(Group* group) override {
        m_myGroup = group;
        m_addAndRemoveButtons = CCArray::create();
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
        
        const float textInputScale = 1; // broken when < 1
        auto nameInput = TextInput::create(
            (m_width - nameLabel->getScaledContentWidth() - 20 - 5 - 20) / textInputScale, "(empty)");
        nameInput->setString(group->getName(), false);
        nameInput->setCommonFilter(CommonFilter::Any);
        nameInput->setCallback([nameInput, group](const std::string& str) {
            auto name = toValidString(str.c_str());
            nameInput->setString(name, false);
            group->updateName(name, group->getCmi());
            Global::get().m_hasUnsavedOGChanges = true;
        });
        m_mainLayer->addChild(nameInput);
        nameInput->setAnchorPoint({0,0.5});
        nameInput->setScale(textInputScale);
        nameInput->setPosition(ccp(nameLabel->getScaledContentWidth() + 5 + 20, m_height - 55));

        m_groupSizeLabel = CCLabelBMFont::create("", "bigFont.fnt");
        m_mainLayer->addChild(m_groupSizeLabel);
        m_groupSizeLabel->setScale(0.3);
        m_groupSizeLabel->setPosition(ccp(m_width / 2, m_height - 85));
        
        const float scale1 = 0.8, scale2 = 0.35 / scale1; // adjust button padding
        auto spr = ButtonSprite::create("Add column right", "bigFont.fnt", "GJ_button_01.png", scale1);
        spr->setScale(scale2);
        auto btn1 = CCMenuItemSpriteExtra::create(spr, this, menu_selector(ExtraOptionsPopup::onAddColRight));
        spr = ButtonSprite::create("Add column left", "bigFont.fnt", "GJ_button_01.png", scale1);
        spr->setScale(scale2);
        auto btn2 = CCMenuItemSpriteExtra::create(spr, this, menu_selector(ExtraOptionsPopup::onAddColLeft));
        spr = ButtonSprite::create("Delete column", "bigFont.fnt", "GJ_button_06.png", scale1);
        spr->setScale(scale2);
        auto btn3 = CCMenuItemSpriteExtra::create(spr, this, menu_selector(ExtraOptionsPopup::onColRemove));
        spr = ButtonSprite::create("Add row below", "bigFont.fnt", "GJ_button_01.png", scale1);
        spr->setScale(scale2);
        auto btn4 = CCMenuItemSpriteExtra::create(spr, this, menu_selector(ExtraOptionsPopup::onAddRowBottom));
        spr = ButtonSprite::create("Add row above", "bigFont.fnt", "GJ_button_01.png", scale1);
        spr->setScale(scale2);
        auto btn5 = CCMenuItemSpriteExtra::create(spr, this, menu_selector(ExtraOptionsPopup::onAddRowTop));
        spr = ButtonSprite::create("Delete row", "bigFont.fnt", "GJ_button_06.png", scale1);
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
                m_addAndRemoveButtons->addObject(btn);
                posY -= 20;
            }
            posX = m_width / 2;
        }
        
        spr = ButtonSprite::create("Set icon", "bigFont.fnt", "GJ_button_05.png", scale1);
        spr->setScale(scale2);
        auto btn7 = CCMenuItemSpriteExtra::create(spr, this, menu_selector(ExtraOptionsPopup::onSetIcon));
        menu->addChild(btn7);
        btn7->setPosition(20 + btn7->getScaledContentWidth() / 2, m_height - 180);

        // secret buttons
        if (isDeveloperMode()) {
            // spr = ButtonSprite::create("Create in\neditor", "bigFont.fnt", "GJ_button_01.png", scale1);
            // spr->setScale(scale2);
            // menu->addChildAtPosition(
            //     CCMenuItemSpriteExtra::create(spr, this, menu_selector(ExtraOptionsPopup::devCreateInEditor)), 
            //     Anchor::TopRight, ccp(50, -20));
        }

        updateGroupInfoLabel();
        updateGroupCmiInfo();
        
        return true;
    }

public:
    static ExtraOptionsPopup* create(Group* group) {
        auto ret = new ExtraOptionsPopup();
        if (ret && ret->initAnchored(ret->m_width, ret->m_height, group)) {
            ret->autorelease();
            return ret; 
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

private:
    void updateGroupInfoLabel() {
        float h = m_myGroup->getMatrix().size();
        float w = (h > 0) ? m_myGroup->getMatrix()[0].size() : 0;
        int count = 0;
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                if (m_myGroup->getMatrix()[i][j] != 0) {
                    count++;
                }
            }
        }
        m_groupSizeLabel->setString(fmt::format(
            "Group: Rows: {}. Columns: {}. Items: {}.", h, w, count).c_str());
    }

    void updateGroupCmiInfo() {
        uint32_t btnX, btnY;
        if (m_myGroup->getSelectedItemPosition(&btnX, &btnY)) {
            m_groupCmiInfo.m_groupHasSelectedCmi = true;
            m_groupCmiInfo.m_column = btnX;
            m_groupCmiInfo.m_row = btnY;
        } else {
            m_groupCmiInfo.m_groupHasSelectedCmi = false;
        }
        updateButtons();
    }

    // void devCreateInEditor(CCObject*) {
    //     if (auto obj = EditorUI::get()->m_selectedObject) {
    //         CCPoint end;
    //         auto str = m_myGroup->toString(obj->getPosition(), &end);
    //         LevelEditorLayer::get()->createObjectsFromString(str, 1, 1);
    //     } else {
    //         shortAlert("Pivot object not selected");
    //     }
    //     onClose(nullptr);
    // }

    void updateButtons() {
        bool active = m_groupCmiInfo.m_groupHasSelectedCmi;
        for (int i = 0; i < m_addAndRemoveButtons->count(); i++) {
            auto btn = static_cast<CCMenuItemSpriteExtra*>(m_addAndRemoveButtons->objectAtIndex(i));
            btn->setEnabled(active);
            auto btnSpr = static_cast<ButtonSprite*>(btn->getChildByTag(1));
            btnSpr->m_BGSprite->setOpacity(active ? 255 : 100);
            btnSpr->m_label->setOpacity(active ? 255 : 100);
        }
    }

    void onClose(CCObject* sender) override {
        m_myGroup->updateMenu();
        Popup::onClose(sender);
    }

    void onInfoBtn(CCObject*) {
        auto winWidth = CCDirector::sharedDirector()->getWinSize().width;
        createQuickPopup("Extra options explanation", 
"- <co>Group name</c>: name of the group. Name isn't shown if it's empty or \
if this option is disabled in mod settings.\n\
- <co>Add (...)</c>: creates new empty row/column at the specified location \
relative to the <cj>focused button</c>.\n\
- <co>Remove (...)</c>: removes row/column with the <cj>focused button</c>.\n\
- <co>Set icon</c>: updates object(s) shown on the group button (you can use from 1 to 4 objects).\n\
- <cy>Note</c>: If there is no <cj>focused button</c> within the group, <co>Add</c> and <co>Remove</c> \
buttons will be inactive",
            "ok", nullptr, winWidth * .8, nullptr, true, true
        );
    }

    void onAddColRight(CCObject*) {
        if (!m_groupCmiInfo.m_groupHasSelectedCmi) return;
        m_myGroup->addColumn(m_groupCmiInfo.m_column+1);
        m_myGroup->updateMenu(false);
        m_myGroup->setSelectedCmiWithPosition(m_groupCmiInfo.m_column, m_groupCmiInfo.m_row);
        updateGroupInfoLabel();
        updateGroupCmiInfo();
        Global::get().m_hasUnsavedOGChanges = true;
    }

    void onAddColLeft(CCObject*) {
        if (!m_groupCmiInfo.m_groupHasSelectedCmi) return;
        m_myGroup->addColumn(m_groupCmiInfo.m_column);
        m_myGroup->updateMenu(false);
        m_myGroup->setSelectedCmiWithPosition(m_groupCmiInfo.m_column+1, m_groupCmiInfo.m_row);
        updateGroupInfoLabel();
        updateGroupCmiInfo();
        Global::get().m_hasUnsavedOGChanges = true;
    }

    void onAddRowTop(CCObject*) { 
        if (!m_groupCmiInfo.m_groupHasSelectedCmi) return;
        m_myGroup->addRow(m_groupCmiInfo.m_row);
        m_myGroup->updateMenu(false);
        m_myGroup->setSelectedCmiWithPosition(m_groupCmiInfo.m_column, m_groupCmiInfo.m_row+1);
        updateGroupInfoLabel();
        updateGroupCmiInfo();
        Global::get().m_hasUnsavedOGChanges = true;
    }

    void onAddRowBottom(CCObject*) {
        if (!m_groupCmiInfo.m_groupHasSelectedCmi) return;
        m_myGroup->addRow(m_groupCmiInfo.m_row+1);
        m_myGroup->updateMenu(false);
        m_myGroup->setSelectedCmiWithPosition(m_groupCmiInfo.m_column, m_groupCmiInfo.m_row);
        updateGroupInfoLabel();
        updateGroupCmiInfo();
        Global::get().m_hasUnsavedOGChanges = true;
    }

    void onColRemove(CCObject*) {
        if (!m_groupCmiInfo.m_groupHasSelectedCmi) return;
        m_myGroup->deleteColumn(m_groupCmiInfo.m_column);
        Global::editor()->setNewFocusedCmi(nullptr);
        m_myGroup->updateMenu(false);
        Global::get().m_hasUnsavedOGChanges = true;
        onClose(nullptr);
    }

    void onRowRemove(CCObject*) {
        if (!m_groupCmiInfo.m_groupHasSelectedCmi) return;
        m_myGroup->deleteRow(m_groupCmiInfo.m_row);
        Global::editor()->setNewFocusedCmi(nullptr);
        m_myGroup->updateMenu(false);
        Global::get().m_hasUnsavedOGChanges = true;
        onClose(nullptr);
    }

    void onSetIcon(CCObject*) {
        auto selected = EditorUI::get()->getSelectedObjects();
        if (selected->count() == 0 || selected->count() > 4) {
            alert(fmt::format("<cr>Icon not updated!</c> You must select 1, 2, 3 or 4 \
object(s) in editor to update group icon\n(Now selected <cy>{}</c>)", selected->count()).c_str());
        } else {
            std::array<short, 4> ids = {0};
            for (int i = 0; i < selected->count(); i++) {
                ids[i] = static_cast<GameObject*>(selected->objectAtIndex(i))->m_objectID;
            }
            m_myGroup->updateObjId(ids);
            Global::get().m_hasUnsavedOGChanges = true;
        }
        onClose(nullptr);
    }
};
    