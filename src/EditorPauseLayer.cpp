#include "EditorUI.hpp"

#include <Geode/modify/EditorPauseLayer.hpp>

static const char* unsavedTxt = "You have <co>unsaved</c> changes in <cy>Object Groups</c> configuration.\n"
                    "They will be lost if you exit the editor\n<cj>Do you want to exit the editor?</c>";


class $modify(EditorPauseLayer) {

    struct Fields {
        bool justExit = false;
    };

    void updatePinnedGroupsState() {
        Global::get().m_pinnedGroupsStates.clear();
        if (Global::get().m_settings.m_keepPinned && Global::editor()) {
            if (auto pg = Global::editor()->m_fields->pinnedGroups) {
                for (auto group : CCArrayExt<Group*>(pg->getChildren())) {
                    auto worldPos = group->convertToWorldSpace(ccp(0,0));
                    Global::get().m_pinnedGroupsStates[group->getGroupUID()] = worldPos;
                }
            }
        }
    }

    void onExitEditor(CCObject* sender) {

        if (m_fields->justExit || !Global::get().m_hasUnsavedOGChanges) {
            updatePinnedGroupsState();
            return EditorPauseLayer::onExitEditor(sender);
        }
        
        createQuickPopup("Object Groups", unsavedTxt,
            "Exit", "No", 
            [this, sender] (auto, bool isBtn2) {
                if (!isBtn2) {
                    m_fields->justExit = true;
                    updatePinnedGroupsState();
                    EditorPauseLayer::onExitEditor(sender);
                    m_fields->justExit = false;
                }
            }, true, true
        );

    }

    // void onExitNoSave(CCObject* sender) {
    //     log::debug("click onExitNoSave");
    //     EditorPauseLayer::onExitNoSave(sender);
    // }

    // void onSaveAndExit(CCObject* sender) {
    //     log::debug("click onSaveAndExit");
    //     EditorPauseLayer::onSaveAndExit(sender);
    // }

    void onSaveAndPlay(CCObject* sender) {
        
        if (m_fields->justExit || !Global::get().m_hasUnsavedOGChanges) {
            updatePinnedGroupsState();
            return EditorPauseLayer::onSaveAndPlay(sender);
        }

        createQuickPopup("Object Groups", unsavedTxt,
            "Exit", "No", 
            [this, sender] (auto, bool isBtn2) {
                if (!isBtn2) {
                    m_fields->justExit = true;
                    updatePinnedGroupsState();
                    EditorPauseLayer::onSaveAndPlay(sender);
                    m_fields->justExit = false;
                }
            }, true, true
        );

    }

};