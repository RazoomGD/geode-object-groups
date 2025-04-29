#include "ObjectGroups.hpp"
#include "EditorUI.hpp"


static const char* bgIdToName[12] = {
    "GJ_button_01.png", // 0
    "GJ_button_01.png",
    "GJ_button_02.png",
    "GJ_button_03.png",
    "GJ_button_04.png",
    "GJ_button_05.png",
    "GJ_button_06.png",
    "OG_button_07.png"_spr,
    "OG_button_08.png"_spr,
    "OG_button_09.png"_spr,
    "OG_button_10.png"_spr,
    "OG_button_11.png"_spr,
};

// fix color sprite bad position bug
void setGameObjScalePosition(GameObject* obj, CCPoint pos, float scaleMultiplier) {
    if (obj->m_colorSprite) {
        auto sprPos = obj->m_colorSprite->getPosition();
        auto sprSc = obj->m_colorSprite->getScale();
        obj->setPosition(pos);
        obj->setScale(obj->getScale() * scaleMultiplier);
        obj->m_colorSprite->setPosition(sprPos);
        obj->m_colorSprite->setScale(sprSc);
    } else {
        obj->setPosition(pos);
        obj->setScale(obj->getScale() * scaleMultiplier);
    }
}

CreateMenuItem* getCustomCreateBtn(short id, int bg, bool doRegister) {
    std::array<short,4> a = {id, 0, 0, 0};
    return getCustomCreateBtn(a, bg, doRegister);
}

// negative id is id used by object groups, not by the game
CreateMenuItem* getCustomObjectCreateBtn(EditorUI* editor, short negativeId, int validBg) {
    
    auto objStr = GameManager::get()->stringForCustomObject(negativeId); // call my hook
    if (objStr.empty()) {
        objStr = "1,914,2,0,3,105,31,SSB1c2Vk;1,914,2,0,3,75,31,dG8gYmUgYQ==;1,914,2,0,3,45,31,Y3VzdG9t;1,914,2,0,3,15,31,b2JqZWN0;";
    }

    auto arr = CCArray::create();
    auto spr = editor->spriteFromObjectString(objStr, false, false, 0, arr, nullptr, nullptr);

    for (auto* el : CCArrayExt<GameObject*>(arr)) {
        setColorToGameObjectNew(el, true);
    }

    spr->setScale(std::min(32.f / spr->getContentHeight(), 32.f / spr->getContentWidth()));

    auto cmi = editor->getCreateBtn(1, validBg);
    auto buttonSpr = static_cast<ButtonSprite*>(cmi->getNormalImage());
    if (auto obj = buttonSpr->m_subSprite) {
        obj->setVisible(false);
    }
    buttonSpr->addChild(spr);
    spr->setPosition({20,21});

    cmi->m_objectID = negativeId;
    cmi->setTag(negativeId); // compat. with creative mode
    return cmi;
}

// colors: 1-green, 2-cyan, 3-pink, 4-gray, 5-darker gray, 6-red
CreateMenuItem* getCustomCreateBtn(std::array<short, 4> const &ids, int bg, bool doRegister) {
    auto editor = Global::editor();
    int validBg = (bg <= 6) ? bg : 1;
    CreateMenuItem* btn;

    if (ids[0] > 0) {
        btn = editor->getCreateBtn(ids[0], validBg);
    } else {
        btn = getCustomObjectCreateBtn(editor, ids[0], validBg);
    }

    if (!doRegister && editor->m_createButtonArray->lastObject() == btn) {
        editor->m_createButtonArray->removeLastObject();
    }
    if (validBg != bg) {
        static_cast<ButtonSprite*>(btn->getNormalImage())->updateBGImage(bgIdToName[bg]);        
    }

    // handle extra objects
    GameObject* objects[4];
    short objCount = 1;
    if (ids[1] != 0) objects[objCount++] = static_cast<GameObject*>(
        static_cast<ButtonSprite*>(getCustomCreateBtn(ids[1], 1, false)->getNormalImage())->m_subSprite);
    if (ids[2] != 0) objects[objCount++] = static_cast<GameObject*>(
        static_cast<ButtonSprite*>(getCustomCreateBtn(ids[2], 1, false)->getNormalImage())->m_subSprite);
    if (ids[3] != 0) objects[objCount++] = static_cast<GameObject*>(
        static_cast<ButtonSprite*>(getCustomCreateBtn(ids[3], 1, false)->getNormalImage())->m_subSprite);

    if (objCount > 1) {
        auto btnSpr = static_cast<ButtonSprite*>(btn->getNormalImage());
        objects[0] = static_cast<GameObject*>(btnSpr->m_subSprite);
        for (int i = 0; i < objCount; i++) {
            if (i != 0) btnSpr->addChild(objects[i], 1);
        }
        if (objCount == 2) {
            setGameObjScalePosition(objects[0], ccp(12,22), 0.48);
            setGameObjScalePosition(objects[1], ccp(28,22), 0.48);
        } else {
            setGameObjScalePosition(objects[0], ccp(12,30), 0.48);
            setGameObjScalePosition(objects[1], ccp(28,30), 0.48);
            if (objCount == 4) {
                setGameObjScalePosition(objects[2], ccp(12,13), 0.48);
                setGameObjScalePosition(objects[3], ccp(28,13), 0.48);
            } else {
                setGameObjScalePosition(objects[2], ccp(20,13), 0.48);
            }
        }
    }
    
    btn->m_baseScale = 1;
    btn->setScale(1);
    btn->updateSprite();
    return btn;
}

void setColorToCreateBtnNew(CreateMenuItem* cmi, bool isBright) {
    ccColor3B color = isBright ? ccc3(255, 255, 255) : ccc3(127, 127, 127);
    if (auto spr = cmi->getChildByType<ButtonSprite>(0)) {
        if (spr->m_subBGSprite) {
            spr->m_subBGSprite->setColor(color); // button bg sprite
        }
        for (int i = 0; ; i++) {
            if (auto gameObj = spr->getChildByType<GameObject>(i)) {
                setColorToGameObjectNew(gameObj, isBright);
            } else break;
        }
    }
}

void setColorToGameObjectNew(GameObject* gameObj, bool isBright) {
    // ! mostly decompiled code of EditorUI::updateCreateMenu() that sets the color
    ccColor3B color = isBright ? ccc3(255, 255, 255) : ccc3(127, 127, 127);
    int objId;
    if ((int)gameObj->m_classType == 1) {
        bool cVar14;
        if (gameObj->m_customColorType == 0) {
            cVar14 = gameObj->m_maybeNotColorable;
        } else {
            cVar14 = (gameObj->m_customColorType == 1);
        }
        if (cVar14 || gameObj->m_colorSprite || 
            gameObj->m_baseColor->m_defaultColorID == 0x3ec || 
            gameObj->m_baseColor->m_defaultColorID == 0x0  || 
            /* (*(char *)(gameObj + 0xdf) == '\0') */ false) {
            goto LAB_14010da56;
        }
        color = isBright ? ccc3(200, 200, 255) : ccc3(100, 100, 127);
        goto LAB_14010dac2;
    }
LAB_14010da56:
    objId = gameObj->m_objectID;
    bool bVar12;
    if (objId < 0x531) {
        if (((objId == 0x530) || (objId == 0x396)) || (objId == 0x397)) goto LAB_14010daad;
        bVar12 = (objId == 0x52f);
LAB_14010da89:
        if (bVar12) goto LAB_14010daad;
        auto piVar3 = gameObj->m_baseColor;
        if (piVar3 != 0) {
            objId = piVar3->m_colorID;
            if ((piVar3->m_defaultColorID == objId) || (objId == 0x0)) {
                objId = piVar3->m_defaultColorID;
            }
            if (objId == 0x3f2) goto LAB_14010daad;
        }
    }
    else {
        if (objId != 0x630) {
            bVar12 = (objId == 0x7dc);
            goto LAB_14010da89;
        }
LAB_14010daad:
        color = isBright ? ccc3(0, 0, 0) : ccc3(127, 127, 127);
    }
LAB_14010dac2:
    gameObj->setObjectColor(color);
    color = isBright ? ccc3(200, 200, 255) : ccc3(100, 100, 127);
    gameObj->setChildColor(color);
}


// CreateMenuItem* getCreateMenuItemButton(CCSprite* sprite, SEL_MenuHandler selector, const char* buttonTexture) {
//     auto buttonSpr = ButtonSprite::create(sprite, 0x20, 0x0, 32.0, 1.0, true /* ? */, buttonTexture, true);
//     auto cmi = CreateMenuItem::create(buttonSpr, nullptr /* ? */, Global::editor(), selector);
//     cmi->m_objectID = 0;
//     return cmi;
// }


void getBarSize(int* rows, int* cols) {
    *cols = GameManager::sharedState()->getIntGameVariable("0049");
    *rows = GameManager::sharedState()->getIntGameVariable("0050");
}

// lol these numbers are really hardcoded in RobTop's code
static const std::set<short> darkerButtonBgObjIds = {
    146, 147, 204, 206, 673, 674, 1340, 1340, 1341, 1342, 1343, 1344, 1345,
    144, 145, 205, 459, 
    498, 499, 500, 501, 277, 278, 719, 721, 990, 992, 1120, 1122, 1123, 1124, 1125, 1126, 1127, 1132, 1133,
    1134, 1135, 1136, 1137, 1138, 1139, 1241, 1242, 1243, 1244, 1245, 1246,
    1292, 1010, 1009, 1271, 1272, 1760, 1761, 1887, 1011, 1012, 1013, 1269, 1270, 1293, 1762, 1763, 1888,
    740, 741, 742
};

int getItemBtnColor(short objId) {
    if (objId < 0) return 5;
    return darkerButtonBgObjIds.contains(objId) ? 5 : 4;
}

int getGroupBtnColor() {
    return Global::get().m_settings.m_groupBtnColor;
}

std::string getFontFileById(int id) {
    id--;
    if (id == -1) {
        return "goldFont.fnt";
    } else if (id == 0) {
        return "bigFont.fnt";
    } else if (id < 10) {
        return fmt::format("gjFont0{}.fnt", id);
    } else {
        return fmt::format("gjFont{}.fnt", id);
    }
}

std::string toValidString(const char* txt) {
    std::stringstream s;
    for (; *txt; txt++) {
        char c = *txt;
        if ((c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9')) {
            s << c;
        } else {
            const char* a = " -_.,'!?()[]{}<>|/:=+*";
            for (; *a; a++) {
                if (*a == c) {
                    s << c;
                    break;
                }
            }
        }
    }
    return s.str();
}

// floating alert
void shortAlert(const char* text, float timeSec) {
    auto alert = TextAlertPopup::create(text, timeSec, 0.6, 150, "bigFont.fnt");
    EditorUI::get()->addChild(alert, 199);
}

static std::list<std::function<void()>> tatQueue;

struct WaitTransitionObject : public CCObject {
    void checkScene(float) {
        auto scene = CCScene::get();
        if (typeinfo_cast<CCTransitionScene*>(scene)) {
            return; // means we are in transition scene now
        }
        GameManager::get()->unschedule(schedule_selector(WaitTransitionObject::checkScene));
        for(auto& func : tatQueue) func();
        tatQueue.clear();
    }
};

void callAfterTransition(std::function<void()> func) {
    tatQueue.push_back(func);
    GameManager::get()->schedule(schedule_selector(WaitTransitionObject::checkScene), 0);
}

std::vector<std::vector<short>> divideGridAlignedObjects(CCArrayExt<GameObject*> objects) {
    const float minGap = 15.0;
    const float maxWidth = 30.0;

    std::vector<std::vector<short>> result;
    const std::vector<std::vector<short>> emptyVector;
    std::map<GameObject*, std::pair<int, int>> objInfo; // <obj, <rowIdx, columnIdx>>

    // divide into rows
    std::sort(objects.begin(), objects.end(), [](GameObject* a, GameObject* b) {
        return a->getPositionY() > b->getPositionY(); // descending
    });

    float rowBegin = objects[0]->getPositionY();
    float rowLast = rowBegin;
    int rowIdx = 0;

    for (auto* obj : objects) {
        if (obj->getPositionY() < rowLast - minGap) {
            // found a gap => start a new row from this object
            rowIdx++;
            rowBegin = obj->getPositionY();
            rowLast = rowBegin;
            objInfo.insert({obj, {rowIdx, 0}});
        } else {
            if (obj->getPositionY() < rowBegin - maxWidth) {
                // row is too wide, so we assume that objects are not grid-aligned
                return emptyVector;
            } else {
                rowLast = obj->getPositionY();
                objInfo.insert({obj, {rowIdx, 0}});
            }
        }
    }

    // divide into columns
    std::sort(objects.begin(), objects.end(), [](GameObject* a, GameObject* b) {
        return a->getPositionX() < b->getPositionX(); // ascending
    });

    float columnBegin = objects[0]->getPositionX();
    float columnLast = columnBegin;
    int columnIdx = 0;
    
    for (auto* obj : objects) {
        if (obj->getPositionX() > columnLast + minGap) {
            // found a gap => start a new column from this object
            columnIdx++;
            columnBegin = obj->getPositionX();
            columnLast = columnBegin;
            objInfo.at(obj).second = columnIdx;
        } else {
            if (obj->getPositionX() > columnBegin + maxWidth) {
                // column is too wide, so we assume that objects are not grid-aligned
                return emptyVector;
            } else {
                columnLast = obj->getPositionX();
                objInfo.at(obj).second = columnIdx;
            }
        }
    }

    // find intersections => put into matrix
    for (int i = 0; i < rowIdx+1; i++) {
        std::vector<short> newRow(columnIdx+1, 0);
        result.push_back(newRow);
    }

    for (auto& el : objInfo) {
        int row = el.second.first;
        int col = el.second.second;
        if (result[row][col] != 0 && result[row][col] != el.first->m_objectID) {
            // two distinct objects in one cell => assume objects are not grid-aligned
            return emptyVector;
        }
        result[row][col] = el.first->m_objectID;
    }

    return result; // tomorrow I will have no idea how it works
}

static const uint64_t isIdExists[] = {
    0b0111111111111101111111111111111111111001110001111111111111111111ULL, // ids 0-63
    0b1111111111111110111111111111111111110101011100110111111111111111ULL, // ids 64-127
    0b1111111111111101111111111111111111111111111111111111111111111011ULL, // ...
    0b1111111111111111111111011111101111111111111111111111111111111111ULL,
    0b1111110111111111111101111111111101010011110101000101010100010100ULL,
    0b0101111111010100010100010100010101010010000111111111111111100000ULL,
    0b0000000011111111000001111111111000011110000000000000000000000011ULL,
    0b1111111111111111111111111111111111111111111111111111111111111111ULL,
    0b1111111111111111111111111111111111111111111111111111111111111111ULL,
    0b1111111111111111111111111111111111111111111111111111111111111111ULL,
    0b1111111111111111111111111111111111100011111111111111111111111111ULL,
    0b1111111111111001111111111111111111111110110100001111111100111111ULL,
    0b1111111100000000000000000000000000000001111111111111111111111111ULL,
    0b1100000001111111101001111101011100011111111001101111111111110111ULL,
    0b1001111111111111001111111101111111111111111111111111111111111111ULL,
    0b1100111111111111110011111111111110000111111111110111111111111110ULL,
    0b1111111111111111111111111111111111111111111111110001111111111111ULL,
    0b1111111111111111111111111111111010111111000011111111111111111111ULL,
    0b1111111111111111111111111111111111111111111111111111111111100000ULL,
    0b0000111111111111111111111111111111111111111111111111111111111111ULL,
    0b1111111111111111111111111111111111111111101001111111111000111111ULL,
    0b1111111111111111111111111111111111111111111111111111000000000000ULL,
    0b0000000000000000000000011111111111111111111111111111111110000001ULL,
    0b1100000000000000000000001000000000010011111111111111111111111111ULL,
    0b1111100000000000111111111111111111000000000000111111111111111111ULL,
    0b1111111111111111111111111111111111111111111111111111111111111111ULL,
    0b1111111111111111111111111111111111111111111111111111111111111111ULL,
    0b1111111111111111111111111111111111111111111111111111111111111111ULL,
    0b1111111111111111111111111111110111111111111111111111111111111111ULL,
    0b1111111111111111111111111111111111111111111111111111111111111101ULL,
    0b1111111110011111111100000000000000000000000010000000000000000000ULL,
    0b0000000000000000000000000000100110001111111111111111111111111111ULL,
    0b1111111100000011111111111111111111111111111111111111111111111111ULL,
    0b1111111111111111111111111111111111111111111111111111111111111111ULL,
    0b1111111111111111111111111111111111111111111111111111111111111111ULL,
    0b1111111111111111111111111111111111111111111111111111111111111111ULL,
    0b1111111111111111111111111111111111111111111111111111111111111111ULL,
    0b1111111111111111111111111111111111111111111111111111111111111111ULL,
    0b1111111111111111111111111111111111111111111111111111111111111111ULL,
    0b1111111111111111111111111111111111111111111111111111111111111111ULL,
    0b1111111111111111111111111111111111111111111111111111111111111111ULL,
    0b1111111111111111111111111111111111111111111111111111111111111111ULL,
    0b1111111111111001100011111111111111111111111111111111111111111111ULL,
    0b1111111111111111111001001111111111111111111111111111111111111111ULL,
    0b1111111111111111111111111111111111111111111111111111111111111111ULL,
    0b1111111111111111110111111101011111111101111111111111111111111111ULL,
    0b1111111111111111111111111111111111111111111111111111111111101111ULL,
    0b1111111111111111100101111111111111111111111111111111111111111111ULL,
    0b1111111111111111111111111100011111111111111111111111111111111111ULL,
    0b1111111111111111111111111111111111111111111111111111111111111111ULL,
    0b1111111111111111111111111111111111111111111111111111111111111111ULL,
    0b1111111111111111111111111111111111111111111111111111111111111111ULL,
    0b1111111111111111111111111111111111111111111111111111111111111111ULL,
    0b1111111111111111111111111111111111111111111111111111111111111111ULL,
    0b1111111111111111111111111111111111111111111111111111111111111111ULL,
    0b1111111111111111111111111111111111111111111111111111111111111111ULL,
    0b1111111111111111111111111111111101111111111111111111111111110111ULL,
    0b1111111111111110000000000000000000000000000000000000111111111111ULL,
    0b1111111111111111111111111111111111111111111111111111111111111111ULL,
    0b1111111111111111111111111111111111111111111111111111111111111111ULL,
    0b1111111111111111111111111111111111111111111111111111111111111111ULL,
    0b1111111111111111111111111111111111111111111111111111111111111111ULL,
    0b1111111111111111111111111111111111111111111111111111111111111111ULL,
    0b1111111111111111111111111111111111111111111111111111111111111111ULL,
    0b1111111111111111111111111111111111111111111111111111111111111111ULL,
    0b1111111111111111111111111111111111111111111111111111111111111111ULL,
    0b1111111111111111111111111111111111111111111111111111111111111111ULL,
    0b1111111111111111111111111111111111111111111111111111111111111111ULL,
    0b1111111111111111111111111111111111000000000000000111111111111111ULL,
    0b1111111111111111111111111111111111111111111111111111111111111111ULL,
    0b1111111111111111111111111111111111111111111111111111111111110000ULL
};

// true, if you can have this object in editor
bool isObjIdExistsFast(short id) {
    if (id < 0) return true;
    if (id > 4539) return false;
    short index = id % 64;
    short row = id / 64;
    uint64_t num = isIdExists[row];
    uint64_t mask = 0b1000000000000000000000000000000000000000000000000000000000000000ULL >> index;
    bool exists = (num & mask);
    return exists;
}

// get ids without duplicates preserving their order
std::vector<short> getUniqueIds(CCArrayExt<GameObject*> objects) {
    std::set<short> idsOnce;
    std::vector<short> ids;
    for (auto* obj : objects) {
        short id = obj->m_objectID;
        if (!idsOnce.contains(id)) {
            idsOnce.insert(id);
            ids.push_back(id);
        }
    }
    return ids;
}


CreateMenuItem* cloneGroupCmi(CreateMenuItem* cmi, Group* group) {
    CreateMenuItem* ret;
    if (group->isSingle()) { // get classic cmi but with set userObject
        // ret = getCustomCreateBtn(group->getObjId(), getItemBtnColor(group->getObjId()));
        return nullptr; // error
    } else { // get cmi with set userObject and custom selector
        ret = getCustomCreateBtn(group->getObjIds(), getGroupBtnColor(), false);
        ret->m_pfnSelector = cmi->m_pfnSelector;
        ret->m_pListener = cmi->m_pListener;
        ret->m_objectID = 0;
        ret->setTag(0); // compat with creative mode
        group->updateName(group->getName(), ret); // update cmi user obj
    }
    ret->setUserObject(CMI_USER_OBJ_ID, group); // set group
    setColorToCreateBtnNew(ret, true);
    return ret;
}


// inspired by https://habr.com/ru/articles/671136/ (Коэффициент Сёренсена)
float computeMatchRatio(const std::string& target, const std::string& query) {

    if (target.size() == 0 || query.size() == 0) return 0;

    // do something for too short strings
    if (target.size() == 1 || query.size() == 1) {
        const std::string& str = (target.size() == 1) ? query : target;
        char ch = (target.size() == 1) ? std::tolower(target[0]) : std::tolower(query[0]);
        for (int i = 0; i < str.size(); i++) {
            if (ch == std::tolower(str[i])) {
                return 1.f / str.size();
            }
        }
        return 0;
    }
    
    // algorithm (amount of identical bi-grams + small bonus for sequential or close bi-grams)

    std::map<uint16_t, uint16_t> queryBiGrams; // map<bi-gram, count>

    // fill query bi-grams
    for (int i = 0; i < query.size() - 1; i++) {
        uint8_t two[2] = {(uint8_t)std::tolower(query[i]), (uint8_t)std::tolower(query[i+1])};
        auto iter = queryBiGrams.find(*(uint16_t*)two);

        if (iter != queryBiGrams.end()) {
            (*iter).second++;
        } else {
            queryBiGrams.insert({*(uint16_t*)two, 1});
        }
    }

    float count = 0;
    float bonus = 1;
    for (int i = 0; i < target.size() - 1; i++) {
        uint8_t two[2] = {(uint8_t)std::tolower(target[i]), (uint8_t)std::tolower(target[i+1])};
        auto iter = queryBiGrams.find(*(uint16_t*)two);

        if (iter != queryBiGrams.end() && (*iter).second) {
            (*iter).second--;
            count += 1 + (bonus - 1);
            bonus *= 1.3f;
        } else {
            bonus = std::max(1.f, bonus / 1.1f);
        }
    }

    // return 2.f * count / (query.size() - 1 + target.size() - 1);
    return count;
}