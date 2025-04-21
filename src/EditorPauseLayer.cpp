#include "EditorUI.hpp"

#include <Geode/modify/EditorPauseLayer.hpp>

#define UNSAVED_POPUP_TXT "You have <co>unsaved</c> changes in <cy>Object Groups</c> configuration.\n"\
                    "They will be lost if you exit the editor\n<cj>Do you want to exit the editor?</c>"


class $modify(EditorPauseLayer) {

    struct Fields {
        bool justExit = false;
    };

    void onExitEditor(CCObject* sender) {

        if (m_fields->justExit || !Global::get().m_hasUnsavedOGChanges) {
            return EditorPauseLayer::onExitEditor(sender);
        }
        
        createQuickPopup("Object Groups", UNSAVED_POPUP_TXT,
            "Exit", "No", 
            [this, sender] (auto, bool isBtn2) {
                if (!isBtn2) {
                    m_fields->justExit = true;
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
            return EditorPauseLayer::onSaveAndPlay(sender);
        }

        createQuickPopup("Object Groups", UNSAVED_POPUP_TXT,
            "Exit", "No", 
            [this, sender] (auto, bool isBtn2) {
                if (!isBtn2) {
                    m_fields->justExit = true;
                    EditorPauseLayer::onSaveAndPlay(sender);
                    m_fields->justExit = false;
                }
            }, true, true
        );

    }

};