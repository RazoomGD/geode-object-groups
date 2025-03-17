#include "ObjectGroups.hpp"

#include <matjson.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/ui/GeodeUI.hpp>


// this thing is needed to inform the users about the most important updates
// of the mod, especially chash-fix updates. (And also to annoy them)
// This is isolated class except Global::get().m_isCurrentVersionSafe

class UpdateNotificationManager : public CCNode {
private:

    EventListener<web::WebTask> m_versionInfoTaskListener;
    std::string m_updateMessage;

public:
    static UpdateNotificationManager* get() {
        static UpdateNotificationManager* instance = nullptr;
        if (!instance) {
            instance = new UpdateNotificationManager();
            if (instance && instance->init()) {
                // ret->autorelease(); no autorelease Singleton
            } else {
                CC_SAFE_DELETE(instance);
                instance = nullptr;
            }
        }
        return instance;
    }

    void goodMorning() {
        letsGo();
    }

    const std::string& getUpdateMsg() {
        return m_updateMessage;
    }

private:

    void letsGo() {
        web::WebRequest req = web::WebRequest();
        std::string url = "https://razoomgd.github.io/mods/update_notifications/object_groups.json";
        req.timeout(std::chrono::seconds(15));
        auto task = req.get(url);

        m_versionInfoTaskListener.bind([this](web::WebTask::Event* e){
            if (web::WebResponse* value = e->getValue()) {
                // The request finished!
                auto maybeJson = value->json();
                if (value->code() != 200) {
                    log::error("web request returned bad result (code: {})", value->code());
                } else if (!maybeJson.isOk()) {
                    log::error("web request returned invalid json");
                } else {
                    handleWebResponse(*maybeJson);
                }
            } else if (web::WebProgress* progress = e->getProgress()) {
                // The request is still in progress...
            } else if (e->isCancelled()) {
                // Our request was cancelled
            }
        });
        m_versionInfoTaskListener.setFilter(task);
    }

    bool handleWebResponse(matjson::Value resp) {
        auto currVersion = Mod::get()->getMetadata().getVersion().toNonVString();

        auto config = resp["config"];
        if (!config.isArray()) return false;

        for (auto& el : config) {
            auto version = el["version"];
            if (version.isString() && *version.asString() == currVersion) {
                bool isUnsafe = el["isUnsafe"].asBool().unwrapOr(false);
                std::string updateMsg = el["userMessage"].asString().unwrapOr("");

                Global::get().m_isCurrentVersionSafe = !isUnsafe;
                m_updateMessage = updateMsg;
                // scheduleAnnoyingPopupAfterTransition();
                callAfterTransition([](){
                    createQuickPopup("Object Groups Update",
                        UpdateNotificationManager::get()->getUpdateMsg(),
                        "Open Geode", "I don't want",
                        [](auto, bool isBtn2) {
                            if (isBtn2) return;
                            openInfoPopup(Mod::get());
                        }, true, true
                    );
                });
                return true;
            }
        }
        return false;
    }
};