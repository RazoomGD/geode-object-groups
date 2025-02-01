#pragma once

#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/EditButtonBar.hpp>

using namespace geode::prelude;



enum class BtnType {Item, Group, GroupItem};

// each button in EditButtonBar must have this object
struct BtnInfo : CCObject {
	SEL_MenuHandler m_itemSelector;
	BtnType m_btnType;
	int* m_groupObj;

	BtnInfo(BtnType type, SEL_MenuHandler defaultSelector, int* groupObj=nullptr);

	void onClick(CCObject* sender);
};