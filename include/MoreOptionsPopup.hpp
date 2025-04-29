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
        
        spr = ButtonSprite::create("Create group\nfrom layout", "bigFont.fnt", "GJ_button_03.png", scale1);
        spr->setScale(scale2);
        btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(MoreOptionsPopup::onGroupFromLayout));
        menu->addChildAtPosition(btn, Anchor::Top, ccp(-btn->getScaledContentWidth()/2-5, -95));
        
        spr = ButtonSprite::create("New custom\nobject", "bigFont.fnt", "GJ_button_03.png", scale1);
        spr->setScale(scale2);
        btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(MoreOptionsPopup::onAddAsSingleCustomObject));
        menu->addChildAtPosition(btn, Anchor::Top, ccp(btn->getScaledContentWidth()/2+5, -95));

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
            // spr = ButtonSprite::create("Paste old\nformat", "bigFont.fnt", "GJ_button_01.png", scale1);
            // spr->setScale(scale2);
            // btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(MoreOptionsPopup::devPasteOldFormat));
            // menu->addChildAtPosition(btn, Anchor::TopRight, ccp(50, -50));
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
            old.m_enableSearchTab != _new.m_enableSearchTab
        );
        if (editorReloadReq) {
            alert("Some of the changed settings <co>require editor reload</c>");
        }

        // group reload required
        bool groupReloadReq = (
            old.m_showNames != _new.m_showNames ||
            old.m_groupBgColor != _new.m_groupBgColor ||
            old.m_font != _new.m_font ||
            old.m_pinButton != _new.m_pinButton
        );
        if (groupReloadReq) {
            old.m_showNames = _new.m_showNames;
            old.m_groupBgColor = _new.m_groupBgColor;
            old.m_font = _new.m_font;
            old.m_pinButton = _new.m_pinButton;
            Global::editor()->execForeachGroup([](Group* g, int){g->setUpdateRequired(true);});
        }
        
        // nothing is required
        old.m_pinGestures = _new.m_pinGestures;
        old.m_autoClose = _new.m_autoClose;
        old.m_shiftAdd = _new.m_shiftAdd;
    }


    inline void writeClipboard(matjson::Value const &groupsArray, std::set<short> const &customIds) {
        auto custom = Global::editor()->getCustomObjects(customIds);
        if (custom.empty()) {
            clipboard::write(matjson::makeObject({
                {"config", groupsArray}
            }).dump());
        } else {
            clipboard::write(matjson::makeObject({
                {"config", groupsArray},
                {"custom", custom}
            }).dump());
        }
        shortAlert("Copied to clipboard!", 0.75f);
    }


    void copyFocusedGroupAsJson(CCObject*) {
        auto editor = Global::editor();
        if (auto cmi = editor->getFocusedCmi()) {
            std::set<short> custom;
            if (auto group = static_cast<Group*>(cmi->getUserObject(CMI_USER_OBJ_ID))) {
                if (!group->isSingle()) {
                    auto json = matjson::Value(std::vector<matjson::Value>{group->toJson(custom)});
                    writeClipboard(json, custom);
                    onClose(nullptr);
                    return;
                }
            }
            if (cmi->m_objectID < 0) custom.insert(cmi->m_objectID);
            auto json = matjson::Value(std::vector<matjson::Value>{matjson::makeObject({{"obj", cmi->m_objectID}})});
            writeClipboard(json, custom);
            onClose(nullptr);
            return;
        }
        alert("<cr>Group is not selected</c>");
    }


    void copyCurrentTabAsJson(CCObject*) {
        auto editor = Global::editor();
        if (auto bar = editor->m_createButtonBar) {
            if (bar->getUserObject(BAR_USER_OBJ_ID) != nullptr) {
                std::set<short> custom;
                auto json = editor->barToJsonValue(bar, custom);
                writeClipboard(json, custom);
                onClose(nullptr);
                return;
            }
        }
        alert("You can't copy contents of the current tab "
            "because it is not controlled by <cy>Object Groups</c>");
    }


    void pasteGroupsFromJson(CCObject*) {
        auto parsed = matjson::parse(clipboard::read());
        if (auto maybeJson = parsed.ok()) {

            if (!Global::editor()->m_createButtonBar->getUserObject(BAR_USER_OBJ_ID)) {
                alert("You can't create a button in this tab");
                return;
            }

            auto json = *maybeJson;

            std::map<int, std::string> customAll;
            auto fcustom = json["custom"];
            if (fcustom.isObject()) {
                for (auto& [key, value] : fcustom) {
                    customAll.insert({std::atoi(key.c_str()), value.asString().unwrapOr("")});
                }
            }

            auto newGroups = CCArray::create();
            int groupCount = 0;

            auto fgroups = json["config"];
            if (fgroups.isArray()) {
                for (auto& val : fgroups) {
                    if (auto group = Group::createFromJsonValue(val, true)) {
                        newGroups->addObject(group);
                        group->setUserCreated(true);
                        if (!group->isSingle()) groupCount++;
                    }
                }
            }

            int total = newGroups->count();
            if (total == 0) {
                alert("<cj>Nothing was found :(</c>\nCheck that there are no mistakes in json. "
                    "(You can copy other groups as json to see the expected json format)");
                return;
            }

            newGroups->retain();

            createQuickPopup("Object Groups", 
                fmt::format("Are you sure you want to paste <cy>{}</c> buttons \n(<cy>{}</c> groups "
                            "and <cy>{}</c> objects) from <cp>clipboard</c>?", 
                            total, groupCount, total - groupCount),
                "No", "Yes", 
                [newGroups, this, customAll] (auto, bool isBtn2) {
                    if (isBtn2) {
                        auto buttons = CCArray::create();
                        for (auto *group : CCArrayExt<Group*>(newGroups)) {
                            group->remapCustomObjects(customAll);
                            buttons->addObject(group->getCmi());
                        }
                        Global::editor()->addButtonsAndReloadCurrentBar(buttons);
                        Global::get().m_hasUnsavedOGChanges = true;
                        shortAlert(fmt::format("Pasted {} buttons!", buttons->count()).c_str());
                    } else {
                        shortAlert("Nothing happened!");
                    }
                    newGroups->release();
                    onClose(nullptr);
                }
            );
            return;
        }
            
        alert("Couldn't paste from clipboard!\n<cr>INCORRECT JSON FORMAT</c>");
    }

    void onInfoBtn(CCObject*) {
        auto winWidth = CCDirector::sharedDirector()->getWinSize().width;
        createQuickPopup("More Options", 
            "- <co>Create tab icon from selected</c>: creates new icon for the current "
            "tab from selected objects (select nothing to reset to default)\n"
            "- <co>Create group from layout</c>: tries to put selected objects in a new group while "
            "preserving their relative positions from editor\n"
            "- <co>New custom object/tab</c>: creates a <cy>custom object</c> "
            "from selected objects and adds it to the tab\n"
            "- <co>Copy group/tab</c>: copies focused group or entire tab to clipboard as <cl>json</c>\n"
            "- <co>Paste group(s)</c>: adds groups from <cl>json</c> content of clipboard to the current tab\n"
            "- <cp>Note</c>: You can use copy/paste json options to share you groups with other people",
            
            "ok", nullptr, winWidth * .95, nullptr, true, true
        );
    }
    
};

