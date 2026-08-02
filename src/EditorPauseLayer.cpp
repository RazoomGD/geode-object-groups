#include "EditorUI.hpp"

#include <Geode/modify/EditorPauseLayer.hpp>

#ifndef GEODE_IS_IOS // on IOS see MyEditorUI::onPause

class $modify(EditorPauseLayer) {

    struct Fields {
        bool justExit = false;
    };

    void createConfirmPopup(std::function<void()> confirmYes) {
        createQuickPopup("Object Groups",
            "You have <co>unsaved</c> changes in <cy>Object Groups</c> configuration.\n"
            "They will be lost if you exit the editor\n"
            "<cj>Do you want to exit the editor?</c>",
            "Exit", "No", 
            [this, confirmYes] (auto, bool isBtn2) {
                if (!isBtn2) {
                    m_fields->justExit = true;
                    updatePinnedGroupsState();
                    confirmYes();
                }
            }, true, true
        );
    }

    void onExitEditor(CCObject* sender) {
        if (m_fields->justExit || !Global::get().m_hasUnsavedOGChanges) {
            updatePinnedGroupsState();
            return EditorPauseLayer::onExitEditor(sender);
        }
        createConfirmPopup([this, sender]{EditorPauseLayer::onExitEditor(sender);});
    }

    // void onExitNoSave(CCObject* sender) {
    //     log::info("click onExitNoSave");
    //     EditorPauseLayer::onExitNoSave(sender);
    // }

    void onSaveAndExit(CCObject* sender) {
        if (m_fields->justExit || !Global::get().m_hasUnsavedOGChanges) {
            updatePinnedGroupsState();
            return EditorPauseLayer::onSaveAndExit(sender);
        }
        createConfirmPopup([this, sender]{EditorPauseLayer::onSaveAndExit(sender);});
    }

    // void onResume(CCObject* sender) {
    //     log::info("click onResume");
    //     EditorPauseLayer::onResume(sender);
    // }

    void onSaveAndPlay(CCObject* sender) {
        if (m_fields->justExit || !Global::get().m_hasUnsavedOGChanges) {
            updatePinnedGroupsState();
            return EditorPauseLayer::onSaveAndPlay(sender);
        }
        createConfirmPopup([this, sender]{EditorPauseLayer::onSaveAndPlay(sender);});
    }

};

#endif
