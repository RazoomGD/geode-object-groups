#pragma once

#include "ObjectGroups.hpp"

#include <Geode/ui/GeodeUI.hpp>

class FileOptionsPopup : public Popup {
private:
    const float m_width = 200.f;
    const float m_height = 160.f;

protected:

    bool init() override {
        if (!Popup::init(m_width, m_height))
            return false;
            
        // m_closeBtn->setVisible(false);
        setTitle("File Options");

        auto menu = m_buttonMenu;

        auto infoSpr = CCSprite::createWithSpriteFrameName("GJ_infoIcon_001.png");
        infoSpr->setScale(0.75);
        auto infoBtn = CCMenuItemSpriteExtra::create(
            infoSpr, this, menu_selector(FileOptionsPopup::onInfoBtn));
        menu->addChildAtPosition(infoBtn, Anchor::TopRight, ccp(-18, -18));

        const float scale1 = 0.8, scale2 = 0.35 / scale1; // adjust button padding
        CCSprite* spr = ButtonSprite::create("Open config dir", "bigFont.fnt", "GJ_button_01.png", scale1);
        spr->setScale(scale2);
        auto btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(FileOptionsPopup::onOpenConfigDir));
        menu->addChildAtPosition(btn, Anchor::Top, ccp(0, -50));
        
        spr = ButtonSprite::create("Save obj/group to file", "bigFont.fnt", "GJ_button_05.png", scale1);
        spr->setScale(scale2);
        btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(FileOptionsPopup::onCopyGroupAsJson));
        menu->addChildAtPosition(btn, Anchor::Top, ccp(0, -75));
        
        spr = ButtonSprite::create("Save tab to file", "bigFont.fnt", "GJ_button_05.png", scale1);
        spr->setScale(scale2);
        btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(FileOptionsPopup::onCopyTabAsJson));
        menu->addChildAtPosition(btn, Anchor::Top, ccp(0, -100));

        spr = ButtonSprite::create("Load group(s) from file", "bigFont.fnt", "GJ_button_04.png", scale1);
        spr->setScale(scale2);
        btn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(FileOptionsPopup::onPasteGroupsFromJson));
        menu->addChildAtPosition(btn, Anchor::Top, ccp(0, -125));

        return true;
    }

    void onClose(CCObject* sender) override {
        Popup::onClose(sender);
    }

public:
    static FileOptionsPopup* create() {
        auto ret = new FileOptionsPopup();
        if (ret && ret->init()) {
            ret->autorelease();
            return ret; 
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

protected:

    void selectAndWriteFile(std::string data) {
        file::FilePickOptions opt = {.defaultPath = std::nullopt, .filters = {{"json", {"*.json"}}}};
        auto fut = file::pick(file::PickMode::SaveFile, opt);
        async::spawn(std::move(fut), [data, me = Ref(this)](file::PickResult res){
            if (res && *res) {
                auto path = **res;
                if (file::writeString(path, data)) {
                    shortAlert("Saved!");
                    me->onClose(nullptr);
                    return;
                }
            }
            me->onClose(nullptr);
            shortAlert("Error!");
        });
    } 


    void onOpenConfigDir(CCObject*) {
        auto path = Mod::get()->getConfigDir();
        file::openFolder(path);
    }


    void onCopyGroupAsJson(CCObject*) {
        auto editor = Global::editor();
        if (auto cmi = editor->getFocusedCmi()) {
            auto res = copyGroupAsJson(cmi);
            selectAndWriteFile(res);
            return;
        }
        alert("<cr>Button is not selected</c>");
    }


    void onCopyTabAsJson(CCObject*) {
        auto editor = Global::editor();
        if (auto bar = editor->getCurrentTabIfAllowed()) {
            auto res = copyTabAsJson(bar);
            selectAndWriteFile(res);
            return;
        }
        alert("You can't copy contents of the current tab, "
            "because it is not controlled by <cy>Object Groups</c>");
    }


    void onPasteGroupsFromJson(CCObject*) {
        file::FilePickOptions opt = {.defaultPath = std::nullopt, .filters = {{"json", {"*.json"}}}};
        auto fut = file::pick(file::PickMode::OpenFile, opt);

        async::spawn(std::move(fut), [me = Ref(this)](file::PickResult res){
            if (res && *res) {
                auto path = **res;
                // read file
                if (auto maybeStr = file::readString(path)) {
                    pasteGroupsFromJsonToCurrentTab(*maybeStr);
                    me->onClose(nullptr);
                }
            }
        });
    }

    void onInfoBtn(CCObject*) {
        auto winWidth = CCDirector::sharedDirector()->getWinSize().width;
        createQuickPopup("File Options", 
            "- <co>Open config directory</c>: open directory with the main configuration file (<cj>OGv2_config.json</c>). "
            "Your entire group configuration and custom objects are stored in this file\n"
            "- <co>Save object/group/tab</c>: save focused button (object or group) or entire tab to file in <cl>json</c> format\n"
            "- <co>Load group(s)</c>: add groups from <cl>json</c> content of the file to the current tab\n"
            "- <cp>Note</c>: You can use these options to share you groups or custom objects with other creators or make backups",
            "ok", nullptr, winWidth * .8, nullptr, true, true
        );
    }
    
};

