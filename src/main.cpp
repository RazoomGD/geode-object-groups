#include <Geode/Geode.hpp>

using namespace geode::prelude;

// #include <Geode/modify/EditButtonBar.hpp>
#include <Geode/modify/EditorUI.hpp>

// colors: 1-green, 2-blue, 3-pink, 4-gray, 5-darker gray, 6-red

class $modify(EditorUI) {
	bool init(LevelEditorLayer* editorLayer) {
		if (!EditorUI::init(editorLayer)) return false;
		log::debug("hello");
		return true;
	}
};