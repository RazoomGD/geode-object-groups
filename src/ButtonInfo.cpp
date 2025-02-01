#include "ObjectGroups.hpp"


BtnInfo::BtnInfo(BtnType type, SEL_MenuHandler defaultSelector, int* groupObj) {
    m_itemSelector = defaultSelector;
    m_btnType = type;
    m_groupObj = groupObj;
    this->autorelease();
}

void BtnInfo::onClick(CCObject* sender) {
    log::debug("clicked");
}