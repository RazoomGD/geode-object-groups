#include "ObjectGroups.hpp"

#include <Geode/ui/GeodeUI.hpp>

class MoreOptionsPopup : public Popup<void*> {
private:
    const float m_width = 220.f;
    const float m_height = 220.f;

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
        CCSprite* spr = ButtonSprite::create("Create tab icon from\nselected objects", "bigFont.fnt", "GJ_button_01.png", scale1);
        spr->setScale(scale2);
        auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(MoreOptionsPopup::onCreateTabIcon));
        menu->addChildAtPosition(btn, Anchor::Top, ccp(0, -55));
        
        spr = ButtonSprite::create("Create new group\nfrom layout", "bigFont.fnt", "GJ_button_03.png", scale1);
        spr->setScale(scale2);
        btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(MoreOptionsPopup::onGroupFromLayout));
        menu->addChildAtPosition(btn, Anchor::Top, ccp(0, -95));
        
        spr = ButtonSprite::create("Copy group\nto clipboard", "bigFont.fnt", "GJ_button_05.png", scale1);
        spr->setScale(scale2);
        btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(MoreOptionsPopup::copyFocusedGroupAsJson));
        menu->addChildAtPosition(btn, Anchor::Top, ccp(-btn->getScaledContentWidth()/2-5, -135));
        
        spr = ButtonSprite::create("Copy tab to\nclipboard", "bigFont.fnt", "GJ_button_05.png", scale1);
        spr->setScale(scale2);
        btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(MoreOptionsPopup::copyCurrentTabAsJson));
        menu->addChildAtPosition(btn, Anchor::Top, ccp(btn->getScaledContentWidth()/2+5, -135));

        spr = ButtonSprite::create("Paste group(s)\nfrom clipboard", "bigFont.fnt", "GJ_button_04.png", scale1);
        spr->setScale(scale2);
        btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(MoreOptionsPopup::pasteGroupsFromJson));
        menu->addChildAtPosition(btn, Anchor::Top, ccp(0, -175));

        spr = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
        spr->setScale(0.6f);
        btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(MoreOptionsPopup::onModSettings));
        menu->addChildAtPosition(btn, Anchor::BottomRight, ccp(-30, 30));
        
        if (isDeveloperMode()) {
            spr = ButtonSprite::create("Paste old\nformat", "bigFont.fnt", "GJ_button_01.png", scale1);
            spr->setScale(scale2);
            btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(MoreOptionsPopup::devPasteOldFormat));
            menu->addChildAtPosition(btn, Anchor::TopRight, ccp(50, -50));
        }

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
        onClose(nullptr);
    }

    void onGroupFromLayout(CCObject*) {
        Global::get().m_editorUI->onNewGroupFromLayoutButton(nullptr);
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
        Global::OGSettings updatedSettings;
        updatedSettings.update();

        // editor reload required
        bool editorReloadReq = (
            Global::get().m_settings.m_groupBtnColor != updatedSettings.m_groupBtnColor ||
            Global::get().m_settings.m_extraTabsCount != updatedSettings.m_extraTabsCount
        );
        if (editorReloadReq) {
            alert("Some of the changed settings <co>require editor reload</c>");
        }

        // group reload required
        bool groupReloadReq = (
            Global::get().m_settings.m_showNames != updatedSettings.m_showNames ||
            Global::get().m_settings.m_groupBgColor!= updatedSettings.m_groupBgColor
        );
        if (groupReloadReq) {
            Global::get().m_settings.m_showNames = updatedSettings.m_showNames;
            Global::get().m_settings.m_groupBgColor = updatedSettings.m_groupBgColor;
            Global::get().m_editorUI->execForeachGroup([](Group* g, int){g->setUpdateRequired(true);});
        }

        // nothing is required
        Global::get().m_settings.m_autoClose = updatedSettings.m_autoClose;
    }

    void copyFocusedGroupAsJson(CCObject*) {
        if (auto cmi = Global::get().m_editorUI->getFocusedCmi()) {
            if (auto group = static_cast<Group*>(cmi->getUserObject(CMI_USER_OBJ_ID))) {
                if (!group->isSingle()) {
                    auto json = group->toJson();
                    matjson::Value arr(std::vector<matjson::Value>{json});
                    clipboard::write(arr.dump());
                    shortAlert("Copied to clipboard!");
                    return;
                }
            }
            auto idStr = std::to_string(cmi->m_objectID);
            auto str = std::string("[{\n    \"obj\": ") + idStr + "\n}]";
            clipboard::write(str);
            shortAlert("Copied to clipboard!");
            return;
        }
        alert("<cr>Group is not selected</c>");
    }

    void copyCurrentTabAsJson(CCObject*) {
        if (auto bar = Global::get().m_editorUI->m_createButtonBar) {
            auto json = Global::get().m_editorUI->barToJsonValue(bar);
            clipboard::write(json.dump());
            shortAlert("Copied to clipboard!");
        } else {
            alert("Can't copy contents of the current tab\n"
                        "(because it is not controlled by <cy>Object Groups</c>)");
        }
    }

    void devPasteOldFormat(CCObject*) {
        auto str = clipboard::read();
        auto res = matjson::parse(fmt::format("[{}]", str));
        if (res.isErr()) return;
        auto json = res.unwrap();
        std::vector<int> ids;
        for (auto& val : json) {
            ids.push_back(*val.asInt().ok());
        }
        if (ids.size() == 0) return;
        if (!EditorUI::get()->m_selectedObject) {
            return shortAlert("no selected pivot");
        }
        auto p = EditorUI::get()->m_selectedObject->getPosition() + ccp(60, 0);
        std::string objStr;
        int i = 0;
        for (auto id : ids) {
            objStr.append(fmt::format("1,{},2,{},3,{};", id, p.x, p.y));
            p = p + ccp(60, 0);
            if (i++ == 5) {
                p = p - ccp(6*60, 60);
                i = 0;
            }
        }
        LevelEditorLayer::get()->createObjectsFromString(objStr, false, false);
    }

    void pasteGroupsFromJson(CCObject*) {
        auto parsed = matjson::parse(clipboard::read());
        if (auto maybeJson = parsed.ok()) {

            if (!Global::get().m_editorUI->m_createButtonBar->getUserObject(BAR_USER_OBJ_ID)) {
                alert("Can't create a button in this tab");
                return;
            }

            auto json = *maybeJson;
            if (!json.isArray()) {
                json = matjson::Value(std::vector<matjson::Value>{json});
            }

            auto buttons = CCArray::create();
            int groupCount = 0;
            for (auto& val : json) {
                if (auto group = Group::createFromJsonValue(val, true)) {
                    group->setUserCreated(true);
                    buttons->addObject(group->getCmi());
                    if (!group->isSingle()) groupCount++;
                }
            }
            int total = buttons->count();
            if (total == 0) {
                alert("<cj>Nothing was found :(</c>\nCheck that there are no mistakes in json");
                return;
            }

            buttons->retain();

            createQuickPopup("Object Groups", 
                fmt::format("Are you sure you want to paste <cy>{}</c> buttons \n(<cy>{}</c> groups "
                            "and <cy>{}</c> objects) from <cp>clipboard</c>?", 
                            total, groupCount, total - groupCount),
                "No", "Yes", 
                [buttons, this] (auto, bool isBtn2) {
                    if (isBtn2) {
                        Global::get().m_editorUI->addButtonsAndReloadCurrentBar(buttons);
                        Global::get().m_hasUnsavedOGChanges = true;
                        shortAlert(fmt::format("Pasted {} buttons!", buttons->count()).c_str(), 2);
                    } else {
                        shortAlert("Nothing happened!");
                    }
                    buttons->release();
                    onClose(nullptr);
                }
            );
            return;
        }
            
        alert("Couldn't paste from clipboard!\n<cr>BAD JSON FORMAT</c>");
    }

    void onInfoBtn(CCObject*) {
        auto winWidth = CCDirector::sharedDirector()->getWinSize().width;
        createQuickPopup("More Options", 
"- <co>Create tab icon from selected</c>: creates new icon for the current \
tab from selected objects (select nothing to reset to default)\n\
- <co>New group from layout</c>: tries to put selected objects in a new group while \
preserving their relative positions from editor\n\
- <co>Copy group/tab</c>: copies focused group or entire tab to clipboard in <cl>json</c> format\n\
- <co>Paste group(s)</c>: adds groups from <cl>json</c> content of clipboard to the current tab\n\
- <cr>WARNING</c>: I strongly recommend <cr>NOT USING</c> copy/paste json options for creating groups. \
Instead, use <cp>'New group'</c> and <cp>'New group from layout'</c>",
            "ok", nullptr, winWidth * .8, nullptr, true, true
        );
    }
    
};

