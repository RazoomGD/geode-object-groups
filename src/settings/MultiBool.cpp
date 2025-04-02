#include <Geode/loader/SettingV3.hpp>
#include <Geode/loader/Mod.hpp>
#include <matjson.hpp>

using namespace geode::prelude;

std::set<int> strToSet(std::string str) {
    std::string s;
    std::set<int> set;
    std::istringstream ss(str);
    while (std::getline(ss, s, ',')) {
        set.insert(std::atoi(s.c_str()));
    }
    return set;
}   

std::string setToStr(std::set<int> set) {
    std::string s;
    for (auto i : set) s += std::to_string(i) + ",";
    if (s.size()) s.pop_back();
    return s;
}

class MultiBoolSetting : public SettingBaseValueV3<std::string> {
public:
    std::vector<std::string> m_labels;

    static Result<std::shared_ptr<SettingV3>> parse(std::string const& key, std::string const& modID, matjson::Value const& json) {
        auto res = std::make_shared<MultiBoolSetting>();
        auto root = checkJson(json, "MultiBoolSetting");
        res->parseBaseProperties(key, modID, root);

        if (auto val = root.has("labels")) {
            if (val.isArray()) {
                auto arr = *val.json().asArray();
                for (auto& str : arr) {
                    if (str.isString()) {
                        res->m_labels.push_back(*str.asString());
                    }
                }
            }
        }
        return root.ok(std::static_pointer_cast<SettingV3>(res));
    }

    SettingNodeV3* createNode(float width) override;
};

class MultiBoolSettingNodeV3 : public SettingValueNodeV3<MultiBoolSetting> {
protected:

    std::vector<CCMenuItemToggler*> m_togglers;

    bool init(std::shared_ptr<MultiBoolSetting> setting, float width) {
        if (!SettingValueNodeV3::init(setting, width)) return false;

        auto set = strToSet(getValue());

        for (int i = 0; i < setting->m_labels.size(); i++) {
            auto txt = CCLabelBMFont::create(setting->m_labels[i].c_str(), "bigFont.fnt");
            txt->setScale(0.5f);
            auto tog = CCMenuItemToggler::createWithStandardSprites(this, menu_selector(MultiBoolSettingNodeV3::onToggle), 0.55f);
            tog->toggle(set.contains(i));
            tog->setTag(i);
            getButtonMenu()->addChild(txt);
            getButtonMenu()->addChild(tog);
            m_togglers.push_back(tog);
        }

        getButtonMenu()->setContentWidth(220);
        getButtonMenu()->setLayout(RowLayout::create()->setAutoScale(false)->setGap(-5)->setAxisAlignment(AxisAlignment::End));

        updateState(nullptr);
        
        return true;
    }
    
    void onResetToDefault() override {
        SettingValueNodeV3::onResetToDefault();
        auto set = strToSet(getValue());
        for (int i = 0; i < m_togglers.size(); i++) {
            m_togglers[i]->toggle(set.contains(i));
        }
    }

    void onToggle(CCObject* sender) {
        auto tog = static_cast<CCMenuItemToggler*>(sender);
        auto set = strToSet(getValue());

        if (tog->isOn()) {
            set.erase(tog->getTag());
        } else {
            set.insert(tog->getTag());
        }
        setValue(setToStr(set), tog);
    }

public:
    static MultiBoolSettingNodeV3* create(std::shared_ptr<MultiBoolSetting> setting, float width) {
        auto ret = new MultiBoolSettingNodeV3();
        if (ret && ret->init(setting, width)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

SettingNodeV3* MultiBoolSetting::createNode(float width) {
    return MultiBoolSettingNodeV3::create(
        std::static_pointer_cast<MultiBoolSetting>(shared_from_this()),
        width
    );
}

$execute {
    (void) Mod::get()->registerCustomSettingType("multi-bool", &MultiBoolSetting::parse);
}
