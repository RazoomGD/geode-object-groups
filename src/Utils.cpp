#include "ObjectGroups.hpp"
#include "EditorUI.hpp"


// colors: 1-green, 2-cyan, 3-pink, 4-gray, 5-darker gray, 6-red
CreateMenuItem* getCustomCreateBtn(int id, int bg, bool doRegister) {
    CreateMenuItem* btn;
    auto editor = Global::get().m_editorUI;
    if (bg <= 6) {
        btn = editor->getCreateBtn(id, bg);
    } else {
        btn = editor->getCreateBtn(id, 1);
        if (auto btnSpr = typeinfo_cast<ButtonSprite*>(btn->getChildren()->objectAtIndex(0)))
        switch (bg) {
            case 7: {btnSpr->updateBGImage("OG_button_07.png"_spr); break;}
            case 8: {btnSpr->updateBGImage("OG_button_08.png"_spr); break;}
            case 9: {btnSpr->updateBGImage("OG_button_09.png"_spr); break;}
            case 10: {btnSpr->updateBGImage("OG_button_10.png"_spr); break;}
            case 11: {btnSpr->updateBGImage("OG_button_11.png"_spr); break;}
            default: break;
        }
    }
    if (!doRegister && editor->m_createButtonArray->lastObject() == btn) {
        editor->m_createButtonArray->removeLastObject();
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
        if (auto gameObj = typeinfo_cast<GameObject*>(spr->m_subSprite)) {
            setColorToGameObjectNew(gameObj, isBright);
        }
    }
}

void setColorToGameObjectNew(GameObject* gameObj, bool isBright) {
    // ! mostly decompiled code of EditorUI::updateCreateMenu() that sets the color
    ccColor3B color = isBright ? ccc3(255, 255, 255) : ccc3(127, 127, 127);
    int objId;
    if (gameObj->m_classType == 1) {
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
//     auto cmi = CreateMenuItem::create(buttonSpr, nullptr /* ? */, Global::get().m_editorUI, selector);
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
    return darkerButtonBgObjIds.contains(objId) ? 5 : 4;
}

int getGroupBtnColor() {
    return Global::get().m_settings.m_groupColor;
}

std::string toValidString(const char* txt) {
    std::string s;
    for (; *txt; txt++) {
        char c = *txt;
        if ((c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9')) {
            s += c;
        } else {
            const char* a = " -_.,!?()[]{}<>|/:=+*";
            for (; *a; a++) {
                if (*a == c) {
                    s += c;
                    break;
                }
            }
        }
    }
    return s;
}

// floating alert
void shortAlert(const char* text, float timeSec) {
    const float scale = 0.7;
    const float scaleFactor = 2; // for CCScale9Sprite not to be destroyed
    auto label = CCLabelBMFont::create(text, "bigFont.fnt");

    auto bg = CCScale9Sprite::create("OG_info_label_bg.png"_spr, {0,0,80,80});
    bg->setContentSize(label->getContentSize() * scaleFactor + ccp(20,20));
    bg->setScale(1 / scaleFactor);
    
    auto base = CCNodeRGBA::create();
    base->setCascadeOpacityEnabled(true);
    base->setID("info-label"_spr);
    base->addChild(bg, 1);
    base->addChild(label, 2);
    base->setPosition(CCDirector::get()->getWinSize() / 2);
    base->setScale(scale);
    CCScene::get()->addChild(base, 199);

    auto endFunc = CallFuncExt::create([base](){base->setVisible(false);}); // fix screen blink
    auto endFunc2 = CallFuncExt::create([base](){base->removeFromParent();});

    base->runAction(CCSequence::create(
        CCDelayTime::create(timeSec/2), CCFadeTo::create(timeSec/2, 0), endFunc, 
        CCDelayTime::create(0.2), endFunc2, nullptr
    ));
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

    for (auto& obj : objects) {
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
    
    for (auto& obj : objects) {
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
    0b0111111111111101111111111111111111111001110001111111111111111111Ui64, // ids 0-63
    0b1111111111111110111111111111111111110101011100110111111111111111Ui64, // ids 64-127
    0b1111111111111101111111111111111111111111111111111111111111111011Ui64, // ...
    0b1111111111111111111111011111101111111111111111111111111111111111Ui64,
    0b1111110111111111111101111111111101010011110101000101010100010100Ui64,
    0b0101111111010100010100010100010101010010000111111111111111100000Ui64,
    0b0000000011111111000001111111111000011110000000000000000000000011Ui64,
    0b1111111111111111111111111111111111111111111111111111111111111111Ui64,
    0b1111111111111111111111111111111111111111111111111111111111111111Ui64,
    0b1111111111111111111111111111111111111111111111111111111111111111Ui64,
    0b1111111111111111111111111111111111100011111111111111111111111111Ui64,
    0b1111111111111001111111111111111111111110110100001111111100111111Ui64,
    0b1111111100000000000000000000000000000001111111111111111111111111Ui64,
    0b1100000001111111101001111101011100011111111001101111111111110111Ui64,
    0b1001111111111111001111111101111111111111111111111111111111111111Ui64,
    0b1100111111111111110011111111111110000111111111110111111111111110Ui64,
    0b1111111111111111111111111111111111111111111111110001111111111111Ui64,
    0b1111111111111111111111111111111010111111000011111111111111111111Ui64,
    0b1111111111111111111111111111111111111111111111111111111111100000Ui64,
    0b0000111111111111111111111111111111111111111111111111111111111111Ui64,
    0b1111111111111111111111111111111111111111101001111111111000111111Ui64,
    0b1111111111111111111111111111111111111111111111111111000000000000Ui64,
    0b0000000000000000000000011111111111111111111111111111111110000001Ui64,
    0b1100000000000000000000001000000000010011111111111111111111111111Ui64,
    0b1111100000000000111111111111111111000000000000111111111111111111Ui64,
    0b1111111111111111111111111111111111111111111111111111111111111111Ui64,
    0b1111111111111111111111111111111111111111111111111111111111111111Ui64,
    0b1111111111111111111111111111111111111111111111111111111111111111Ui64,
    0b1111111111111111111111111111110111111111111111111111111111111111Ui64,
    0b1111111111111111111111111111111111111111111111111111111111111101Ui64,
    0b1111111110011111111100000000000000000000000010000000000000000000Ui64,
    0b0000000000000000000000000000100110001111111111111111111111111111Ui64,
    0b1111111100000011111111111111111111111111111111111111111111111111Ui64,
    0b1111111111111111111111111111111111111111111111111111111111111111Ui64,
    0b1111111111111111111111111111111111111111111111111111111111111111Ui64,
    0b1111111111111111111111111111111111111111111111111111111111111111Ui64,
    0b1111111111111111111111111111111111111111111111111111111111111111Ui64,
    0b1111111111111111111111111111111111111111111111111111111111111111Ui64,
    0b1111111111111111111111111111111111111111111111111111111111111111Ui64,
    0b1111111111111111111111111111111111111111111111111111111111111111Ui64,
    0b1111111111111111111111111111111111111111111111111111111111111111Ui64,
    0b1111111111111111111111111111111111111111111111111111111111111111Ui64,
    0b1111111111111001100011111111111111111111111111111111111111111111Ui64,
    0b1111111111111111111001001111111111111111111111111111111111111111Ui64,
    0b1111111111111111111111111111111111111111111111111111111111111111Ui64,
    0b1111111111111111110111111101011111111101111111111111111111111111Ui64,
    0b1111111111111111111111111111111111111111111111111111111111101111Ui64,
    0b1111111111111111100101111111111111111111111111111111111111111111Ui64,
    0b1111111111111111111111111100011111111111111111111111111111111111Ui64,
    0b1111111111111111111111111111111111111111111111111111111111111111Ui64,
    0b1111111111111111111111111111111111111111111111111111111111111111Ui64,
    0b1111111111111111111111111111111111111111111111111111111111111111Ui64,
    0b1111111111111111111111111111111111111111111111111111111111111111Ui64,
    0b1111111111111111111111111111111111111111111111111111111111111111Ui64,
    0b1111111111111111111111111111111111111111111111111111111111111111Ui64,
    0b1111111111111111111111111111111111111111111111111111111111111111Ui64,
    0b1111111111111111111111111111111101111111111111111111111111110111Ui64,
    0b1111111111111110000000000000000000000000000000000000111111111111Ui64,
    0b1111111111111111111111111111111111111111111111111111111111111111Ui64,
    0b1111111111111111111111111111111111111111111111111111111111111111Ui64,
    0b1111111111111111111111111111111111111111111111111111111111111111Ui64,
    0b1111111111111111111111111111111111111111111111111111111111111111Ui64,
    0b1111111111111111111111111111111111111111111111111111111111111111Ui64,
    0b1111111111111111111111111111111111111111111111111111111111111111Ui64,
    0b1111111111111111111111111111111111111111111111111111111111111111Ui64,
    0b1111111111111111111111111111111111111111111111111111111111111111Ui64,
    0b1111111111111111111111111111111111111111111111111111111111111111Ui64,
    0b1111111111111111111111111111111111111111111111111111111111111111Ui64,
    0b1111111111111111111111111111111111000000000000000111111111111111Ui64,
    0b1111111111111111111111111111111111111111111111111111111111111111Ui64,
    0b1111111111111111111111111111111111111111111111111111111111110000Ui64
};

// true, if you can have this object in editor
bool isObjIdExistsFast(short id) {
    if (id <= 0 || id > 4539) return false;
    short index = id % 64;
    short row = id / 64;
    uint64_t num = isIdExists[row];
    uint64_t mask = 0b1000000000000000000000000000000000000000000000000000000000000000Ui64 >> index;
    bool exists = (num & mask);
    return exists;
}